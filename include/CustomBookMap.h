#ifndef CUSTOM_BOOK_MAP_H
#define CUSTOM_BOOK_MAP_H

#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>

const int INITIAL_TABLE_SIZE = 100;

class CustomBookMap {
private:
    struct Entry {
        int key;
        std::string value;
        bool occupied;
        bool deleted;

        Entry() : key(0), value(""), occupied(false), deleted(false) {}
    };

    std::vector<Entry> table;
    int size;
    int capacity;

    // Simple hash function
    int hash(int key) const {
        return key % capacity;
    }

    void resize() {
        std::vector<Entry> oldTable = table;
        capacity *= 2;
        table = std::vector<Entry>(capacity);
        size = 0;

        for (const auto& entry : oldTable) {
            if (entry.occupied && !entry.deleted) {
                insert(entry.key, entry.value);
            }
        }
    }

public:
    CustomBookMap() : size(0), capacity(INITIAL_TABLE_SIZE) {
        table.resize(capacity);
    }

    void insert(int key, const std::string& value) {
        if (size >= capacity / 2) {
            resize();
        }

        int idx = hash(key);
        while (table[idx].occupied && !table[idx].deleted) {
            if (table[idx].key == key) {
                table[idx].value = value; // Update if the key already exists
                return;
            }
            idx = (idx + 1) % capacity;
        }

        table[idx].key = key;
        table[idx].value = value;
        table[idx].occupied = true;
        table[idx].deleted = false;
        size++;
    }

    std::string get(int key) const {
        int idx = hash(key);
        int startIdx = idx;
        while (table[idx].occupied) {
            if (table[idx].key == key && !table[idx].deleted) {
                return table[idx].value;
            }
            idx = (idx + 1) % capacity;
            if (idx == startIdx) break; // Full loop around
        }
        throw std::runtime_error("Key not found");
    }

    bool exists(int key) const {
        try {
            get(key);
            return true;
        } catch (...) {
            return false;
        }
    }

    // Remove the key from the map
    void remove(int key) {
        int idx = hash(key);
        int startIdx = idx;
        while (table[idx].occupied) {
            if (table[idx].key == key && !table[idx].deleted) {
                table[idx].deleted = true;
                size--;
                return;
            }
            idx = (idx + 1) % capacity;
            if (idx == startIdx) break; // Full loop around
        }
        throw std::runtime_error("Key not found");
    }

    bool find(int key) const {
        int idx = hash(key);
        int startIdx = idx;
        while (table[idx].occupied) {
            if (table[idx].key == key && !table[idx].deleted) {
                return true;
            }
            idx = (idx + 1) % capacity;
            if (idx == startIdx) break;
        }
        return false;
    }

    void erase(int key) {
        int idx = hash(key);
        int startIdx = idx;
        while (table[idx].occupied) {
            if (table[idx].key == key && !table[idx].deleted) {
                table[idx].deleted = true;
                size--;
                return;
            }
            idx = (idx + 1) % capacity;
            if (idx == startIdx) break;
        }
    }

    // Overload the [] operator to allow for easy access to values
    std::string& operator[](int key) {
    int idx = hash(key);
    int startIdx = idx;
    while (table[idx].occupied && !table[idx].deleted) {
        if (table[idx].key == key) {
            return table[idx].value;
        }
        idx = (idx + 1) % capacity;
        if (idx == startIdx) {
            throw std::runtime_error("Key not found");
        }
    }
    
    // If the key is not found, insert a default value and return a reference to it
    insert(key, "");  // Insert a default empty string if key is not present
    return table[idx].value;
    }

        // Define a nested iterator class
    class Iterator {
    private:
        typename std::vector<Entry>::iterator current;
        typename std::vector<Entry>::iterator end;

        void moveToValid() {
            while (current != end && (!current->occupied || current->deleted)) {
                ++current;
            }
        }

    public:
        Iterator(typename std::vector<Entry>::iterator start, typename std::vector<Entry>::iterator end) : current(start), end(end) {
            moveToValid();
        }

        bool operator!=(const Iterator& other) const {
            return current != other.current;
        }

        const std::pair<int, std::string> operator*() const {
            return {current->key, current->value};
        }

        Iterator& operator++() {
            ++current;
            moveToValid();
            return *this;
        }
    };

    // Begin and end methods to support range-based loops
    Iterator begin() {
        return Iterator(table.begin(), table.end());
    }

    Iterator end() {
        return Iterator(table.end(), table.end());
    }

};

#endif
