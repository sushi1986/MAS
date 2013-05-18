#include <atomic>

#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <map>
#include <utility>

#include "cppa/cppa.hpp"

#include "mas/field.hpp"

class environment : public cppa::event_based_actor {

    using coord_type = std::pair<size_t,size_t>;

    struct car {
        unsigned m_id;
        cppa::actor_ptr m_actor;
        coord_type m_pos;

        car(unsigned id, cppa::actor_ptr ptr, size_t x, size_t y)
            : m_id(id)
            , m_actor(ptr)
            , m_pos(std::make_pair(x,y)) { }

        car(unsigned id, cppa::actor_ptr ptr, std::pair<size_t,size_t> pos)
            : m_id(id)
            , m_actor(ptr)
            , m_pos(pos) { }
    };

 public:

    void init();

 private:

    field_type m_field;
    std::map<unsigned,car> m_cars;
    std::atomic<unsigned> m_id_gen;

};

#endif // ENVIRONMENT_HPP
