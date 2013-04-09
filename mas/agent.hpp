
#ifndef AGENT_HPP
#define AGENT_HPP

#include "cppa/cppa.hpp"

class agent : public cppa::event_based_actor {
 public:

    agent(cppa::actor_ptr env, uint32_t speed);

    void init();

 private:

    uint32_t m_speed;
    cppa::actor_ptr m_env;
};

#endif // AGENT_HPP
