#include <vector>
#include <string>
#include <sstream>
#include <iostream>

#include <cppa/opt.hpp>
#include <cppa/cppa.hpp>

#include "mas/mas.hpp"
#include "mas/agent.hpp"
#include "mas/environment.hpp"

using namespace std;
using namespace cppa;

inline std::vector<std::string> split(const std::string& str, char delim) {
    std::vector<std::string> result;
    std::stringstream strs{str};
    std::string tmp;
    while (std::getline(strs, tmp, delim)) result.push_back(tmp);
    return result;
}

auto main (int argc, char** argv) -> int {

    sectn_getter g = &sectn_type::data;
    sectn_setter s = &sectn_type::data;
    announce<sectn_type>(make_pair(g, s));
    announce<pair<uint32_t,uint32_t>>(&pair<uint32_t,uint32_t>::first,
                                      &pair<uint32_t,uint32_t>::second);
    // announce<coord_type>(&coord_type::first, &coord_type::second);

    uint16_t port{20283};
    bool is_environment{false};
    uint32_t number_of_agents{1};
    string host;

    options_description desc;

    bool args_valid = match_stream<string>(argv + 1, argv + argc) (
        on_opt0('e', "environment", &desc, "start an environment",        "general") >> set_flag(is_environment),
        on_opt1('P', "port",        &desc, "set port (default:20283)",    "general") >> rd_arg(port),
        on_opt0('h', "help",        &desc, "print help",                  "general") >> print_desc_and_exit(&desc),
        on_opt1('H', "host",        &desc, "set server host",             "agent"  ) >> rd_arg(host),
        on_opt1('a', "agents",      &desc, "number of agents (default:1", "agent"  ) >> rd_arg(number_of_agents)
    );
    if (!args_valid) print_desc_and_exit(&desc)();

    if (is_environment) {
        auto env = spawn<environment>();

        try { publish(env, port); }
        catch (bind_failure&) {
            cerr << "problem binding server to port: " << port << "'." << endl;
            return EXIT_FAILURE;
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
                        cout << "available commands:\n - /quit\n";
                    }
                );
            }
        };
        send(env, atom("quit"));
    }
    else { // is agent
        cout << "Connecting to '" << host << "' on port '" << port << "'.\n";

        actor_ptr env;
        try { env = remote_actor(host, port); }
        catch (std::exception& e) {
            cerr << "Can not connect to server: " << e.what() << endl;
            return EXIT_FAILURE;
        }

        vector<actor_ptr> running_actors;
        for (uint32_t i{0}; i < number_of_agents; ++i) {
            running_actors.push_back(spawn<agent>(env));
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
                        cout << "available commands:\n - /quit\n";
                    }
                );
            }
        };
        for (auto& a : running_actors) {
            send(a, atom("quit"));
        }
    }


    await_all_others_done();
    shutdown();

    return EXIT_SUCCESS;
}
