
#ifndef AGENT_HPP
#define AGENT_HPP

#include "cppa/cppa.hpp"

class agent : public cppa::event_based_actor {
//class agent : public cppa::sb_actor<agent> {
 public:

    agent(cppa::actor_ptr env, uint32_t max_speed,
          uint32_t max_accel, uint32_t max_decel);

    void init();

 private:

    uint32_t m_id;
    uint32_t m_max_speed;
    uint32_t m_max_accel;
    uint32_t m_max_decel;
    uint32_t m_pref_speed;
    cppa::actor_ptr m_env;
};

#endif // AGENT_HPP
