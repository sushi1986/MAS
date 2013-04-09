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
#include "mas/environment.hpp"

#include "cppa/opt.hpp"

using namespace std;
using namespace cppa;

void environment::init() {
    become (
        on(atom("quit")) >> [=] {
            quit();
        },
        others() >> [=]() {
            aout << "[ENV] Unexpected message: '"
                 << to_string(last_dequeued()) << "'.\n";
        }
    );
}

int main(int argc, char* argv[]) {


    uint16_t port{20283};
    options_description desc;
    bool args_valid = match_stream<string>(argv + 1, argv + argc) (
        on_opt1('P', "port", &desc, "set port (default:20283)") >> rd_arg(port),
        on_opt0('h', "help", &desc, "print help") >> print_desc_and_exit(&desc)
    );
    if (!args_valid) print_desc_and_exit(&desc)();

    auto env = spawn<environment>();

    try {
        publish(env, port);
    } catch(bind_failure&) {
        cout << "problem binding server to port: " << port << "'." << endl;
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
    send(env, atom("quit"));

    await_all_others_done();
    shutdown();

    return EXIT_SUCCESS;
}

