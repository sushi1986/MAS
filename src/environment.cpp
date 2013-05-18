#include "mas/mas.hpp"
#include "mas/field.hpp"
#include "mas/environment.hpp"

using namespace std;
using namespace cppa;

void environment::init() {
    become (
        on(atom("register")) >> [=]() {
            cout << "Received 'register' message." << endl;
            auto sndr = self->last_sender();
            auto pos  = make_pair(0,0);
            auto id   = m_id_gen++;
            m_cars.insert(std::make_pair(id, car{id, sndr, pos}));
            reply(atom("ack"), atom("register"));
        },
        on(atom("dance")) >> [=] {
            //find first car
            // for(auto& car : m_cars) {
            //     sync_send(car.second.m_actor, atom("drive"),
            //               m_field.get_section(car.second.m_pos)).then(
            //         others() >> [=]() {
            //             cout << "[ENV] Unexpected message: '"
            //                  << to_string(last_dequeued()) << "'.\n";
            //         }
            //     );
            // }
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
