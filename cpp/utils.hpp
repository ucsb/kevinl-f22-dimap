#ifndef UTILS_H
#define UTILS_H

#include <random>
#include <chrono>
#include <iostream>

extern unsigned seed;

typedef unsigned char color_t;

// Avoids negative remainders
int mod(int a, int b);

template <typename T>
void print_array(std::ostream& os, T* array, int size)
{
    for (int i = 0; i < size; i++)
    {
        os << array[i] << ' ';
    }
    os << '\n';
}

template <typename T>
void print_array(std::ostream& os, T* array, int h, int w)
{
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            os << array[j + (w * i)] << ' ';
        }
        os << '\n';
    }
}

template <typename T>
void print_array(T* array, int size)
{
    print_array<T>(std::cout, array, size);
}

template <typename T>
void print_array(T* array, int h, int w)
{
    print_array<T>(std::cout, array, h, w);
}

#endif
