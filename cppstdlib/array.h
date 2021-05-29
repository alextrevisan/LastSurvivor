#include <initializer_list>
namespace std
{
template<typename T, size_t SIZE>
class array
{
    public:
    array()
    {

    }
    array(std::initializer_list<T> data)
    {
        if constexpr(requires{data.size() == SIZE;}) 
        {
            size_t index = 0;
            for(auto& element: data)
                elements[index++] = element;
        }
    }
    array(const T data[])
    :elements(data)
    {

    }
    T& operator[](const size_t index)
    {
        return elements[index];
    }
    const T& operator[](const size_t index) const
    {
        return elements[index];
    }
    private:
    T elements[SIZE] = {};
    size_t size;
};
}