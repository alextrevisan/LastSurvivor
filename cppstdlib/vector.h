#include <initializer_list>
namespace std
{
template<typename T>
class vector
{
public:
    vector()
    :size(0), capacity(1)
    {
        elements = new T[1];
    }

    vector(std::initializer_list<T> data)
    {
        size = 0;
        if(data.size() > capacity)
            reserve(data.size());

        for(auto& value: data)
        {
            elements[size++] = value;
        }
    }
    ~vector()
    {
        delete[] elements;
    }   

    void reserve(size_t newsize)
    {
        if (newsize <= capacity) return;
        resize(newsize);
    }

    void resize(size_t newsize)
    {
        if(newsize < size)
            size = newsize;

        T* p = new T[newsize];

        for (int i = 0; i < size; ++i)
            p[i] = elements[i];

        delete[] elements;
        elements = p;
        capacity = newsize;
    }
    
    void push_back(const T& value)
    {
        if(size>capacity)
            reserve(capacity+4);
        elements[size++] = value;
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
    T* elements = nullptr;
    size_t size;
    size_t capacity;
};
} //namespace std