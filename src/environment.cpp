
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <algorithm>

#include "mas/mas.hpp"
#include "mas/field.hpp"
#include "mas/environment.hpp"

using namespace std;
using namespace cppa;

environment::environment() : m_id_gen(first_id) {
    std::srand(std::time(0));
}

void environment::init() {
    become (
        on(atom("register")) >> [=]() {
            m_new.emplace_back(self->last_sender());
            cout << "Received 'register' message. "
                 << m_new.size() << " agents waiting."
                 << endl;
            if (m_agents.empty()) {
                send(self, atom("divide"));
            }
        },
        on(atom("divide")) >> [=] {
            cout << "dividing stuff" << endl;
            // create silces
            vector<sectn_type> sects;
            uint32_t s = 0;
            uint32_t e = sectn_len - 1;
            do {
                sects.push_back(m_current.get_section<sectn_len,lanes>(s,e));
                s += sectn_len;
                e += sectn_len;
            } while (e < field_len);
            // cars in the first zone of the first section
            auto h = sects.front();
            distribute_zone(h, 0);
            // for each slice
            for (auto s : sects) {
            //     find all cars in middle zone and send them the slice
                distribute_zone(s, 1);
            }
            // cars in the last zone of the last section
            auto t = sects.back();
            distribute_zone(t, 2);
            // select up to lanes other cars and let them start
            vector<uint32_t> free_spaces;
            for (uint32_t i = 0; i < lanes; ++i) {
                if (h(0,i).first == 0) free_spaces.emplace_back(i);
            }
            random_shuffle(begin(free_spaces), end(free_spaces));
            auto rnd = rand() % (free_spaces.size() + 1);
            cout << "Up to " << rnd << " new agents this turn." << endl;
            auto end = min(m_new.begin() + rnd, m_new.end());
            for (auto itr = m_new.begin(); itr != end; ++itr) {
                actor_ptr agent = (*itr);
                auto id = m_id_gen++;
                auto lane = free_spaces.back();
                send(agent, atom("spawn"), h, lane, id);
                m_agents[id] = agent;
                free_spaces.pop_back();
            }
            m_new.erase(m_new.begin(), end);
        },
        on(atom("conquor"), arg_match) >> [=] (uint32_t id,
                                               uint32_t speed,
                                               coord_type pos) {
            m_next(pos.first, pos.second) = make_pair(id, speed);
            // collect results
            // create new map
            // if all results are there
            //   start divided again
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

void environment::distribute_zone(const sectn_type& section, uint32_t zone) {
    uint32_t from = zone * zone_len;
    uint32_t to   = (zone + 1) * zone_len;
    for (uint32_t row = 0; row < lanes; ++row) {
        for (uint32_t col = from; col < to; ++col) {
            auto t = section(col,row);
            if (t.first > 0) {
                cout << "Found agent with id: " << t.first
                     << " at (" << col << "/" << row << ")." << endl;
                auto itr = m_agents.find(t.first);
                if (itr != m_agents.end()) {
                    send(itr->second, atom("drive"), section, make_pair(col,row));
                }
            }
        }
    }
}

