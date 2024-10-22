#ifndef VICVEC_H
#define VICVEC_H

#include <iostream>
#include <stdexcept>

template <typename T>
class VictorVector {
private:
    T* arr;          // Pointer to the dynamically allocated array
    int capacity;    // Total capacity of the array
    int current;     // Current number of elements in the array

public:
    // Constructor
    VictorVector(size_t initialCapacity = 1, T defaultValue = T()) 
        : capacity(initialCapacity), current(0) {  // Initialize 'current' to 0
        arr = new T[capacity];
        for (size_t i = 0; i < capacity; ++i) {
            arr[i] = defaultValue;
        }
    }

    // Destructor
    ~VictorVector() {
        delete[] arr;
    }

    // Get the number of elements in the vector
    size_t size() const {
        return current;
    }

    // Get the total capacity of the vector
    size_t getCapacity() const {
        return capacity;
    }

    // Check if the vector is empty
    bool empty() const {
        return current == 0;  // Return true if no elements
    }

    // Access element by index with bounds checking
    T& operator[](size_t index) {
        if (index >= capacity) {
            throw std::out_of_range("Index out of range");
        }
        return arr[index];
    }

    const T& operator[](size_t index) const {
        if (index >= capacity) {
            throw std::out_of_range("Index out of range");
        }
        return arr[index];
    }

    // Append an element to the end of the vector
    void push_back(const T& element) {
        if (current == capacity) {
            resize(capacity * 2, T());  // Double the capacity and initialize new elements with the default value
        }
        arr[current++] = element;
    }

    // Get the last element in the vector
    T& back() {
        if (current == 0) {
            throw std::out_of_range("Vector is empty");
        }
        return arr[current - 1];
    }

    // Remove the last element of the vector
    void pop_back() {
        if (current == 0) {
            throw std::out_of_range("Vector is empty");
        }
        current--;
    }

    // Clear all elements in the vector
    void clear() {
        current = 0;
    }

    // Iterator support for range-based for loops
    T* begin() {
        return arr;  // Pointer to the first element
    }

    T* end() {
        return arr + current;  // Pointer to one past the last element
    }

    const T* begin() const {
        return arr;  // For const objects, pointer to the first element
    }

    const T* end() const {
        return arr + current;  // For const objects, pointer to one past the last element
    }

    // Private function to resize the array when needed
    void resize(size_t newCapacity, T defaultValue = T()) {
        T* newArr = new T[newCapacity];

        // Copy elements only up to the number of valid elements
        for (size_t i = 0; i < current; ++i) {
            newArr[i] = arr[i];
        }

        // Initialize new elements with the default value
        for (size_t i = current; i < newCapacity; ++i) {
            newArr[i] = defaultValue;
        }

        delete[] arr;
        arr = newArr;
        capacity = newCapacity;
    }


};

#endif
