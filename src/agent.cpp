
#include <chrono>

#include "mas/mas.hpp"
#include "mas/agent.hpp"
#include "mas/field.hpp"

#include "cppa/opt.hpp"

using namespace std;
using namespace cppa;


agent::agent(cppa::actor_ptr env): m_speed(1), m_env(env) { }

void agent::init() {
    send(m_env, atom("register"));
    become(
        on(atom("spawn"), arg_match) >> [=] (const sectn_type& section,
                                             uint32_t lane, ident_type id) {
            m_id = id;
            cout << "Received spawn with id: " << id
                 << " and lane: " << lane
                 << endl;
             if (section(uint32_t(0), lane).first == 0) {
                reply(atom("conquor"), m_id, m_speed, make_pair(uint32_t(0),lane));
             }
            // has no position on the field yet
            // start depending on speed
        },
        on(atom("drive"), arg_match) >> [=] (const sectn_type& /*section*/,
                                             coord_type /*pos*/) {
            cout << "Received drive." << endl;
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
