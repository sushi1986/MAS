#ifndef MAS_HPP
#define MAS_HPP

#include "mas/field.hpp"

namespace {

    constexpr const uint32_t first_id  = 1; // first id is 1; 0 -> no agent

    constexpr const uint32_t max_accel = 1; // 1: ~ 30 km/h
    constexpr const uint32_t max_decel = 2; // 2: ~ 60 km/h
    constexpr const uint32_t max_speed = 5; // 5: ~150 km/h

    constexpr const uint32_t zone_len  = max_speed;
    constexpr const uint32_t field_len = 3 * zone_len; // >= 3 * max_speed
    constexpr const uint32_t sectn_len = 3 * zone_len; // >= 3 * max_speed

    constexpr const uint32_t lanes = 3; // >= 1

    using field_type = field<field_len,lanes>;
    using sectn_type = field<sectn_len,lanes>;
    using coord_type = std::pair<uint32_t,uint32_t>;
    using ident_type = uint32_t;

    using sectn_getter =
        const std::vector<std::pair<uint32_t,uint32_t>>& (sectn_type::*)() const;
    using sectn_setter =
        void (sectn_type::*)(std::vector<std::pair<uint32_t,uint32_t>>);
}

/*

section for max_speed 3
this section would be sent to each car in the middle zone
and if it is the first section to cars not on the field
and to cars in the last zone of the last section

|-+-+-|-+-+-|-+-+-|
|-+-+-|-+-+-|-+-+-|
|-+-+-|-+-+-|-+-+-|
|-+-+-|-+-+-|-+-+-|

*/
#endif
