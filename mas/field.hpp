
#ifndef FIELD_HPP
#define FIELD_HPP

#include <vector>
#include <cassert>
#include <sstream>
#include <algorithm>

namespace {
    constexpr const size_t length   = 10;
    constexpr const size_t lanes    =  1;
}

/*
 * describes the environment, has two derived classes
 * - field: the complete type for the environment
 * - sect: a section sent to each agent
 * each position consists of a tuple of two unsinged values
 * - first: the id of the current agent (ids should be > 0)
 * - second: the agents speed
 * (0/0) is in the top left corner
 */
template<size_t Columns, size_t Rows>
class base {

    using speed = unsigned;
    using id = unsigned;
    using is_vec = std::vector<std::pair<id,speed>>;

 public:

    static constexpr size_t num_elements = Rows * Columns;

    base(base&&) = default;
    base(const base&) = default;
    base& operator=(base&&) = default;
    base& operator=(const base&) = default;

    base() : m_data(num_elements) {
        std::fill(std::begin(m_data), std::end(m_data), std::make_pair(0,0));
    }

    explicit base(is_vec d) : m_data(std::move(d)) {
        assert(m_data.size() == num_elements);
    }

    inline float& operator()(size_t column, size_t row) {
        return m_data[row * Columns + column];
    }

    inline const float& operator()(size_t column, size_t row) const {
        return m_data[row * Columns + column];
    }

    inline float& operator()(std::pair<size_t,size_t> coord) {
        return m_data[coord.second * Columns + coord.first];
    }

    inline const float& operator()(std::pair<size_t,size_t> coord) const {
        return m_data[coord.second * Columns + coord.first];
    }

    typedef typename is_vec::const_iterator const_iterator;

    const_iterator begin() const { return m_data.begin(); }

    const_iterator end() const { return m_data.end(); }

    is_vec& data() { return m_data; }

    const is_vec& data() const { return m_data; }

 private:

    is_vec m_data;

};

template<size_t Columns, size_t Rows>
inline bool operator==(const base<Columns, Rows>& lhs,
                       const base<Columns, Rows>& rhs) {
    return equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<size_t Columns, size_t Rows>
inline bool operator!=(const base<Columns, Rows>& lhs,
                       const base<Columns, Rows>& rhs) {
    return !(lhs == rhs);
}

template<size_t Columns, size_t Rows>
std::string to_string(const base<Columns,Rows>& f) {
    std::ostringstream oss;
    oss.fill(' ');
    for (size_t row = 0; row < Rows; ++row) {
        for (size_t column = 0; column < Columns; ++column) {
            oss << f(column, row);
        }
        oss << '\n';
    }
    return oss.str();
}

template<size_t Columns, size_t Rows>
class sect : public base<Columns,Rows> {

    using speed = unsigned;
    using id = unsigned;
    using is_vec = std::vector<std::pair<id,speed>>;

 public:


 private:


};

template<size_t Columns, size_t Rows>
class field : public base<Columns,Rows> {

    using speed = unsigned;
    using id = unsigned;
    using is_vec = std::vector<std::pair<id,speed>>;

 public:

    template<size_t C, size_t R>
    sect<C, R> get_section(size_t lane, size_t from, size_t to) {
        assert(to - from == R);
        size_t upper = lane == 0 ? 0 : lane - 1;
        size_t lower = lane >= lanes - 1 ? lane : lane + 1;
        assert(upper - lower == C);
        return get_section_impl<C, R>(from, upper, to, lower);
    }

 private:

    template<size_t C, size_t R>
    base<C,R> get_section_impl(size_t from, size_t upper, size_t to, size_t lower) {
        assert(from < C && to < C && upper < R && lower < R);
        assert(C == to - from && R == lower - upper);
        is_vec new_base(C * R);
        for (size_t i = 0; i < R; ++i) {
            std::copy(this->begin() + (from + i * C),
                      this->begin() + (to + i * C),
                      std::begin(new_base) + (i * C));
        }
        return sect<C, R>(std::move(new_base));
    }
};


using field_type = field<length,lanes>;

#endif // FIELD_HPP
