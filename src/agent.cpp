#include <chrono>
#include <cstdlib>

#include "mas/mas.hpp"
#include "mas/agent.hpp"
#include "mas/field.hpp"

using namespace std;
using namespace cppa;


agent::agent(cppa::actor_ptr env, uint32_t max_speed,
             uint32_t max_accel, uint32_t max_decel)
    : m_max_speed(max_speed)
    , m_max_accel(max_accel)
    , m_max_decel(max_decel)
    , m_env(env)
{
    srand(time(0));
    m_pref_speed = rand() % (m_max_speed + 1);
}

void agent::init() {
    send(m_env, atom("register"));
    become(
        on(atom("spawn"), arg_match) >> [=] (const sectn_type& section,
                                             uint32_t lane, ident_type id) {
            m_id = id;
            uint32_t x = 0;
            uint32_t y = lane;
            if (section(uint32_t(0), lane).first == 0) {
//                reply(atom("conquor"), m_id, m_pref_speed, make_pair(x,y), false);
                reply(atom("conquor"), m_id, uint32_t(1), make_pair(x,y), false);
            }
            else {
                reply(atom("error"), atom("move"), id);
            }
            aout << "[" << id << "] -> ("  << x << "/" << y
                 << ")" << endl;
        },
        on(atom("drive"), arg_match) >> [=] (sectn_type section,
                                             coord_type pos) {
            auto speed = section(pos.first, pos.second).second;
            auto x = pos.first;
            auto y = pos.second;
//            section.mark_unavailable(pos);
            uint32_t acceleration = 0;
//            auto lanes = section.rows();

            // change lane if your speed maximum increses
            // otherwise drive to the "left" lane

            // find possibile positions and mark the maximum speed in each
            section.mark_available(pos, speed, m_max_accel, m_max_decel);

            // speed up if

            bool done = false;
            if ( x >= field_len) {
                done = true;
            }
//            else {
//                // calculate new speed
////                if (speed < m_pref_speed) {
////                    ++speed;
////                }
//            }

            reply(atom("conquor"), m_id, speed, make_pair(x,y), done);
            aout << "[" << m_id << "] (" << pos.first << "/"
                 << pos.second << ") -> (" << x << "/" << y << ")" << endl;
            if (done) {
                aout << "[" << m_id << "] done" << endl;
            }
        },
        on(atom("quit")) >> [=] {
            quit();
        },
        others() >> [=]() {
            aout << "[" << m_id << "] Unexpected message: '"
                 << to_string(last_dequeued()) << "'.\n";
        }
    );
}
