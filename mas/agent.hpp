
#ifndef AGENT_HPP
#define AGENT_HPP

#include "cppa/cppa.hpp"

class agent : public cppa::event_based_actor {
 public:

    agent(cppa::actor_ptr env);

    void init();

 private:

    uint32_t m_id;
    uint32_t m_speed;
    cppa::actor_ptr m_env;
};

#endif // AGENT_HPP
