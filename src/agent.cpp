
#include <chrono>

#include "mas/mas.hpp"
#include "mas/agent.hpp"
#include "mas/field.hpp"

#include "cppa/opt.hpp"

using namespace std;
using namespace cppa;


agent::agent(cppa::actor_ptr env): m_env(env) { }

void agent::init() {
    send(m_env, atom("register"));
    become(
        on(atom("ack"), atom("register")) >> [=] {
            cout << "Registered in environment." << endl;
        },
        on(atom("drive"), arg_match) >> [=] (/*const section_type& section*/) {
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
