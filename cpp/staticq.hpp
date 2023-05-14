#ifndef STATICQ_H
#define STATICQ_H

#include <cstring>
#include <ostream>
#include "utils.hpp"

template <typename T>
class StaticQ
{
public:
    StaticQ() : StaticQ(1) {}

    StaticQ(int capacity)
    {
        this->capacity = capacity;
        array = new T[capacity];
    }

    ~StaticQ()
    {
        delete[] array;
    }

    void push(T element)
    {
        if (size < capacity) {
            array[tail] = element;
            tail = mod(tail+1, capacity);
            size++;
        }
    }

    T* pop()
    {
        if (size == 0) return nullptr;
        T* return_me = &(array[head]);
        head = mod(head+1, capacity);
        size--;
        return return_me;
    }

    int get_size() const
    {
        return size;
    }

    int get_space() const
    {
        return capacity - size;
    }

    StaticQ& operator=(const StaticQ& other)
    {
        this->capacity = other.capacity;
        this->size = other.size;
        this->head = other.head;
        this->tail = other.tail;
        delete[] this->array;
        this->array = new T[other.capacity];
        memcpy(this->array, other.array, sizeof(T) * other.capacity);
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const StaticQ& dt)
    {
        for (int i = 0; i < dt.size; i++) {
            os << dt.array[mod(i+dt.head, dt.capacity)] << ' ';
        }
        os << "[size " << dt.size << " space " << dt.capacity - dt.size << " head " << dt.head << " tail " << dt.tail << "]\n";

        return os;
    }
private:
    int capacity, size = 0, head = 0, tail = 0;
    T* array;
};

#endif
