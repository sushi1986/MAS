#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <map>
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

    field_type m_next;
    field_type m_current;
    std::atomic<ident_type> m_id_gen;
    std::vector<cppa::actor_ptr> m_new;
    std::map<ident_type,cppa::actor_ptr> m_agents;
};

#endif // ENVIRONMENT_HPP
