#pragma once

#include <cstdlib>
#include <string>
#include <stdexcept>
#include <cstring>
#include <sstream>

template<class T>
class vec {
private:
    T *array = nullptr;
    size_t size = 0;
    size_t capacity = 0;
public:
    vec(size_t size){
        this->size = size;
        capacity = size;
        array = new T[size]();
    }

    size_t get_size() const { return size; }

    size_t get_capacity() const { return capacity; }

    void push_back(T element);

    void resize_capacity(size_t new_capacity);

    /// @brief resize with changing capacity
    void resize(size_t new_size);

    void resize(size_t new_size, size_t new_capacity);

    void replace(T element, size_t ind) const;

    T &at(size_t i) const;

    T &operator[](size_t i) const { return at(i); };

    void clear();

    std::string to_string() const;

    ~vec() { clear(); }
};
template<class T>
void vec<T>::resize(size_t new_size, size_t new_capacity)
{
    if (new_capacity < new_size)
        throw std::runtime_error("capacity is smaller than size");

    resize_capacity(new_capacity);
    size = new_size;
}
template<class T>
void vec<T>::resize(size_t new_size)
{
    if(new_size > capacity)
        resize_capacity(new_size * 2);
    size = new_size;

}
template<class T>
void vec<T>::resize_capacity(size_t new_capacity)
{
    capacity = new_capacity;
    T* narr = new T[capacity];
    for(size_t i = 0; i < size; i++)
        narr[i] = std::move(array[i]);

    delete[] array;
    array = narr;

}

template<class T>
std::string vec<T>::to_string() const {
    std::stringstream ss;
    ss << "{array ptr: " << array << ", size: " << size << ", capacity: " << capacity << "}" << std::endl;
    for(size_t i = 0; i < get_size() && i < 2; i++){
        ss << "[" << i << "] = " << array[i] << std::endl;
    }
    if (size > 3) ss << "..." << std::endl;
    if(size > 2) ss << "[" << size-1 << "] = " << array[size-1] << std::endl;

    return ss.str();
}

template<class T>
void vec<T>::clear() {
    delete[] array;
    array = nullptr;
    size = capacity = 0;
}

template<class T>
void vec<T>::replace(T element, size_t ind) const {
    this->at(ind) = element;
}

template<class T>
void vec<T>::push_back(T element) {
    if (capacity == 0) {
        capacity = 1;
        array = new T[1];
    } else if (size + 1 > capacity) {
        resize_capacity(capacity * 2);
    }
    array[size++] = element;
}

template<class T>
T &vec<T>::at(size_t i) const {
    if (i >= size)
        throw std::runtime_error("index out-of-bounds");
    return array[i];
}
