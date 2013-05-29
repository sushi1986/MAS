#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <map>
#include <set>
#include <atomic>
#include <utility>

#include "cppa/cppa.hpp"

#include "mas/mas.hpp"
#include "mas/field.hpp"

class environment : public cppa::event_based_actor {

 public:

    environment();

    void init();

 private:

    void distribute_zone(const sectn_type& section, uint32_t zone);
    void spawn_waiting(const sectn_type& section);

    // bool m_running;
    uint32_t m_done;
    field_type m_next;
    field_type m_current;
    std::set<ident_type> m_awaiting;
    std::atomic<ident_type> m_id_gen;
    std::vector<cppa::actor_ptr> m_new;
    std::map<ident_type,cppa::actor_ptr> m_agents;
};

#endif // ENVIRONMENT_HPP
