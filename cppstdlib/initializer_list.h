#include <stddef.h>

namespace std {

template<typename T>
class initializer_list
{
public:
    using value_type = T;
    using reference = const T&;
    using const_reference = const T&;
    using size_type = size_t;
    using iterator = const T*;
    using const_iterator = const T*;

private:
    iterator  m_array;
    size_type m_len;

    constexpr initializer_list(const_iterator itr, size_type st)
        : m_array(itr), m_len(st) { }

public:
    constexpr initializer_list() noexcept : m_array(0), m_len(0) { }
    constexpr size_type size() const noexcept { return m_len; }
    constexpr const_iterator begin() const noexcept { return m_array; }
    constexpr const_iterator end() const noexcept { return begin() + size(); }
}; //class initializer_list

} //namespace std
