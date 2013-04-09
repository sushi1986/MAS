
#include <chrono>

#include "mas/mas.hpp"
#include "mas/agent.hpp"

#include "cppa/opt.hpp"

using namespace std;
using namespace cppa;


agent::agent(cppa::actor_ptr env, uint32_t speed) : m_speed(speed), m_env(env) { }

void agent::init() {
    send(m_env, atom("register"));
    become(
        on(atom("action")) >> [=] () {
            // calculate action
            // answer to server
        },
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
    uint32_t speed{1};

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

    aout << "Connecting to '" << host
         << "' on port '" << port << "'.\n";


    auto env = remote_actor(host, port);

    vector<actor_ptr> running_actors;
    for (uint32_t i{0}; i < number_of_actors; ++i) {
        running_actors.push_back(spawn<agent>(env, speed));
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
