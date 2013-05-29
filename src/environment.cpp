#include <ctime>
#include <chrono>
#include <cstdlib>
#include <iterator>
#include <iostream>
#include <algorithm>

#include "mas/mas.hpp"
#include "mas/field.hpp"
#include "mas/environment.hpp"

using namespace std;
using namespace cppa;

environment::environment() : m_done(0), m_id_gen(first_id) {
    srand(time(0));
}

void environment::init() {
    delayed_send(self, chrono::seconds(1), atom("divide"));
    become (
        on(atom("register")) >> [=]() {
            m_new.emplace_back(self->last_sender());
        },
        on(atom("divide")) >> [=] {
            vector<sectn_type> sects;   // all slices from the field
            uint32_t s = 0;
            uint32_t e = sectn_len - 1;
            do {
                sects.push_back(m_current.get_section<sectn_len,lanes>(s,e));
                s += sectn_len;
                e += sectn_len;
            } while (e < field_len);
            distribute_zone(sects.front(), 0); // first zone of the first section
            for (auto s : sects) {
                distribute_zone(s, 1); // all "middle" zones
            }
            distribute_zone(sects.back(), 2); // last zone of the last section
            spawn_waiting(sects.front()); // spawn new agents
            if (m_awaiting.empty()) {
                delayed_send(self, chrono::seconds(1), atom("divide"));
                // m_running = false;
            }
        },
        // collect results
        on(atom("conquor"), arg_match) >> [=] (uint32_t id,
                                               uint32_t speed,
                                               coord_type pos,
                                               bool done) {
            // m_awaiting.erase(id); // got answer from agent
            auto itr = m_awaiting.find(id);
            if (itr != m_awaiting.end()) {
                m_awaiting.erase(itr);
            }

            if (done) {
                cout << "Agent " << id
                     << " is done! (total: " << ++m_done
                     << ")" << endl;
                auto itr = m_agents.find(id);
                if (itr != m_agents.end()) {
                    m_agents.erase(itr);
                }
            }
            else {
                if (m_next(pos.first, pos.second).first != 0) {
                    cout << "[ERR] Agent " << id
                         << " wants to move into an space occupied by "
                         << m_next(pos.first, pos.second).first << "."
                         << endl;
                }
                else {
                    cout << "Agent " << id  << " to ("
                         << pos.first << "/" << pos.second << ")"
                         << " with " << speed << " s/u." << endl;
                    m_next(pos.first, pos.second) =
                        make_pair(id, speed); // create new map
                }
            }
            if (m_awaiting.empty()) { // if all results are there
                m_current = m_next;
                m_next.clear();
                delayed_send(self, chrono::seconds(1),
                             atom("divide")); // start divided again
                cout << "#  ##  ##  ## ## ##  #" << endl;
            }
        },
        on(atom("error"), atom("move"), arg_match) >> [=] (uint32_t id) {
            cout << "[ERR] Agent " << id << " could not make its move. "
                    "He will be kicked." << endl;
            auto itr1 = m_awaiting.find(id);
            if (itr1 != m_awaiting.end()) {
                m_awaiting.erase(itr1);
            }
            auto itr2 = m_agents.find(id);
            if (itr2 != m_agents.end()) {
                m_agents.erase(itr2);
            }
        },
        on(atom("quit")) >> [=] {
            quit();
        },
        // todo: trap exit messages ...
        others() >> [=]() {
            aout << "[ERR] Unexpected message: '"
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
                auto itr = m_agents.find(t.first);
                if (itr != m_agents.end()) {
                    send(itr->second, atom("drive"), section, make_pair(col,row));
                    m_awaiting.insert(itr->first); // wait for agents answer
                }
            }
        }
    }
}

void environment::spawn_waiting(const sectn_type& section) {
    vector<uint32_t> free_spaces; // free spaces in the first column
    for (uint32_t i = 0; i < lanes; ++i) {
        if (section(0,i).first == 0) free_spaces.emplace_back(i);
    }
    random_shuffle(begin(free_spaces), end(free_spaces));
     auto rnd = rand() % (free_spaces.size() + 1); // more diversity
    /*
    for (auto i = rnd; i > 0; --i) {
        if(!m_new.empty()) {
            auto agent = m_new.back();
            auto id    = m_id_gen++;
            auto lane  = free_spaces.back();
            cout << "Spawning agent " << id << "." << endl;
            send(agent, atom("spawn"), section, lane, id);
            m_awaiting.insert(id); // wait for agents answer
            m_agents[id] = agent;
            m_new.pop_back();
            free_spaces.pop_back();
        }
    }
    */
    auto end = min(m_new.begin() + rnd, m_new.end());
    for (auto itr = m_new.begin(); itr != end; ++itr) { // tell agents to spawn
        actor_ptr agent = (*itr);
        auto id = m_id_gen++;
        auto lane = free_spaces.back();
        send(agent, atom("spawn"), section, lane, id);
        m_awaiting.insert(id); // wait for agents answer
        m_agents[id] = agent;
        free_spaces.pop_back();
    }
    m_new.erase(m_new.begin(), end); // remove spawned agents
}
