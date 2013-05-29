#ifndef FIELD_HPP
#define FIELD_HPP

#include <vector>
#include <cstdlib>
#include <cassert>
#include <sstream>
#include <iostream>
#include <algorithm>

/*
 * describes the environment, has two derived classes
 * - field: the complete type for the environment
 * - sectn: a section sent to each agent
 * each position consists of a tuple of two unsinged values
 * - first: the id of the current agent (ids should be > 0)
 * - second: the agents speed
 * (0/0) is in the top left corner
 */
template<uint32_t Columns, uint32_t Rows>
class field {

    using speed = uint32_t;
    using id = uint32_t;
    using elem = std::pair<id,speed>;
    using is_vec = std::vector<elem>;

 public:

    static constexpr size_t num_elements = Rows * Columns;

    field(field&&) = default;
    field(const field&) = default;
    field& operator=(field&&) = default;
    field& operator=(const field&) = default;

    field() : m_data(num_elements) {
        std::fill(std::begin(m_data), std::end(m_data), std::make_pair(0,0));
    }

    explicit field(is_vec d) : m_data(std::move(d)) {
        assert(m_data.size() == num_elements);
    }

    inline elem& operator()(uint32_t column, uint32_t row) {
        return m_data[row * Columns + column];
    }

    inline const elem& operator()(uint32_t column, uint32_t row) const {
        return m_data[row * Columns + column];
    }

    inline elem& operator()(std::pair<uint32_t,uint32_t> coord) {
        return m_data[coord.second * Columns + coord.first];
    }

    inline const elem& operator()(std::pair<uint32_t,uint32_t> coord) const {
        return m_data[coord.second * Columns + coord.first];
    }

    template<uint32_t C, uint32_t R>
    field<C, R> get_section(uint32_t from, uint32_t to) {
        assert((to - from) + 1 == C);
        return get_section_impl<C, R>(from, to, 0, Rows - 1);
    }

    typedef typename is_vec::const_iterator const_iterator;

    const_iterator begin() const { return m_data.begin(); }

    const_iterator end() const { return m_data.end(); }

    is_vec& data() { return m_data; }

    const is_vec& data() const { return m_data; }

    void data(is_vec d) { m_data = std::move(d); }

    void clear() {
        std::fill(std::begin(m_data), std::end(m_data), std::make_pair(0,0));
    }

    void mark_unavailable(std::pair<uint32_t,uint32_t> coord) {
        for (uint32_t r = Rows; r > 0 ; --r) {
            for (uint32_t c = Columns; c > 0; --c) {
                auto x = c - 1; // can't check if >= 0 (always true)
                auto y = r - 1; // therefor have to correct by 1
                if (y != coord.second && x != coord.first) {
                    handle_position(x,y);
                }
            }
        }
    }

    uint32_t rows() {
        return Rows;
    }

    uint32_t cols() {
        return Columns;
    }

 private:

    template<uint32_t C, uint32_t R>
    field<C,R> get_section_impl(uint32_t from,  uint32_t to,
                                uint32_t upper, uint32_t lower) {
        assert(C == to - from + 1 && R == lower - upper + 1);
        is_vec new_field(C * R);
        for (uint32_t i = 0; i < R; ++i) {
            std::copy(this->begin() + (from + i * Columns),
                      this->begin() + (to + i * Columns) + 1,
                      std::begin(new_field) + (i * C));
        }
        return field<C, R>{std::move(new_field)};
    }

    void handle_position(uint32_t col, uint32_t row) {
        auto t = (*this)(col, row);
        if (t.first != 0) {
            auto speed = t.second;
            for (uint32_t i = 0; i <= speed; ++i) {
                if (col+i < Columns) {
                    (*this)(col+i, row) = t;
                }
            }
        }
    }

    is_vec m_data;

};

template<uint32_t Columns, uint32_t Rows>
inline bool operator==(const field<Columns, Rows>& lhs,
                       const field<Columns, Rows>& rhs) {
    return equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<uint32_t Columns, uint32_t Rows>
inline bool operator!=(const field<Columns, Rows>& lhs,
                       const field<Columns, Rows>& rhs) {
    return !(lhs == rhs);
}

template<uint32_t Columns, uint32_t Rows>
std::string to_string(const field<Columns,Rows>& b) {
    std::ostringstream oss;
    oss.fill(' ');
    for (uint32_t row = 0; row < Rows; ++row) {
        for (uint32_t column = 0; column < Columns; ++column) {
            auto tmp = b(column, row);
            oss << "(" << tmp.first << "/" << tmp.second << ")";
        }
        oss << '\n';
    }
    return oss.str();
}

#endif // FIELD_HPP
