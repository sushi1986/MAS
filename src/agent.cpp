/******************************************************************************\
 *           ___        __                                                    *
 *          /\_ \    __/\ \                                                   *
 *          \//\ \  /\_\ \ \____    ___   _____   _____      __               *
 *            \ \ \ \/\ \ \ '__`\  /'___\/\ '__`\/\ '__`\  /'__`\             *
 *             \_\ \_\ \ \ \ \L\ \/\ \__/\ \ \L\ \ \ \L\ \/\ \L\.\_           *
 *             /\____\\ \_\ \_,__/\ \____\\ \ ,__/\ \ ,__/\ \__/.\_\          *
 *             \/____/ \/_/\/___/  \/____/ \ \ \/  \ \ \/  \/__/\/_/          *
 *                                          \ \_\   \ \_\                     *
 *                                           \/_/    \/_/                     *
 *                                                                            *
 * Copyright (C) 2011-2013                                                    *
 * Dominik Charousset <dominik.charousset@haw-hamburg.de>                     *
 * Raphael Hiesgen <raphael.hiesgen@haw-hamburg.de>                           *
 *                                                                            *
 * This file is part of libcppa.                                              *
 * libcppa is free software: you can redistribute it and/or modify it under   *
 * the terms of the GNU Lesser General Public License as published by the     *
 * Free Software Foundation, either version 3 of the License                  *
 * or (at your option) any later version.                                     *
 *                                                                            *
 * libcppa is distributed in the hope that it will be useful,                 *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                       *
 * See the GNU Lesser General Public License for more details.                *
 *                                                                            *
 * You should have received a copy of the GNU Lesser General Public License   *
 * along with libcppa. If not, see <http://www.gnu.org/licenses/>.            *
\******************************************************************************/


#include "mas/mas.hpp"
#include "mas/agent.hpp"

#include "cppa/opt.hpp"

using namespace std;
using namespace cppa;


agent::agent(cppa::actor_ptr env) : m_env(env) { }

void agent::init() {
    become(
        on(atom("quit")) >> [=] {
            quit();
        },
        others() >> [=]() {
            aout << "[ENV] Unexpected message: '"
                 << to_string(last_dequeued()) << "'.\n";
        }
    );
}


int main (int argc, char* argv[]) {

    string host;
    uint16_t port{20283};
    uint32_t number_of_actors{1};

    options_description desc;
    bool args_valid = match_stream<string>(argv + 1, argv + argc) (
        on_opt1('H', "host", &desc,
                "set server host")
                >> rd_arg(host),
        on_opt1('P', "port", &desc,
                "set port (default: 20283)")
                >> rd_arg(port),
        on_opt1('a', "actors", &desc,
                "set number of actors started by this client")
                >> rd_arg(number_of_actors),
        on_opt0('h', "help", &desc,
                "print help")
                >> print_desc_and_exit(&desc)
    );
    if (!args_valid || host.empty() || number_of_actors < 1) {
        print_desc_and_exit(&desc)();
    }

    aout << "Starting client(s). Connecting to '" << host
         << "' on port '" << port << "'.\n";


    auto server = remote_actor(host, port);

    vector<actor_ptr> running_actors;
    for (uint32_t i{0}; i < number_of_actors; ++i) {
        running_actors.push_back(spawn<agent>(server));
    }

    for (bool done{false}; !done;){
        string input;
        getline(cin, input);
        if (input.size() > 0) {
            input.erase(input.begin());
            vector<string> values{split(input, ' ')};
            match (values) (
                on("quit") >> [&] {
                    done = true;
                },
                others() >> [&] {
                    aout << "available commands:\n - /quit\n";
                }
            );
        }
    };
    for (auto& a : running_actors) {
        send(a, atom("quit"));
    }

    await_all_others_done();
    shutdown();

    return EXIT_SUCCESS;
}
