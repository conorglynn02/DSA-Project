#ifndef INDEXHASHTABLE_H
#define INDEXHASHTABLE_H

#include <iostream>
#include "Trie.h"

struct BookFrequencyNode {
    int bookID;
    int frequency;
    BookFrequencyNode* next;
    BookFrequencyNode* prev;

    BookFrequencyNode(int id, int freq) : bookID(id), frequency(freq), next(nullptr), prev(nullptr) {}
};

// Book Frequency List class
class BookFrequencyList {
public:
    BookFrequencyNode* head;

public:
    BookFrequencyList() : head(nullptr) {}
    void insert(int bookID) {
    // Case 1: checking if the list is empty and inserting the first node
        if (head == nullptr) {
            BookFrequencyNode* newNode = new BookFrequencyNode(bookID, 1);
            head = newNode;
            head->prev = nullptr;
            return;
        }

        // Case 2: checking if book ID is already in the list
        BookFrequencyNode* current = head;
        while (current != nullptr) {
            if (current->bookID == bookID) {
                current->frequency++;
                sort(current); // Sort the node based on the frequency
                return;
            }
            current = current->next;
        }

        // Case 3: inserting a new node if bookID is not in the list
        BookFrequencyNode* newNode = new BookFrequencyNode(bookID, 1);

        insertSorted(newNode);
    }

    // Insert the new node based on frequency
    void insertSorted(BookFrequencyNode* newNode) {
        // If the list is empty, insert at the head
        if (head == nullptr) {
            head = newNode;
            return;
        }

        // If the new node has a higher frequency than the head, insert it at the head
        if (newNode->frequency > head->frequency) {
            newNode->next = head;
            head->prev = newNode;
            head = newNode;
            return;
        }

        // Otherwise, find the correct spot in the list
        BookFrequencyNode* current = head;
        while (current->next != nullptr && current->next->frequency >= newNode->frequency) {
            current = current->next;
        }

        // Insert the new node after the current node
        newNode->next = current->next;
        if (current->next != nullptr) {
            current->next->prev = newNode;
        }
        current->next = newNode;
        newNode->prev = current;
    }

    // Resort the updated frequency node in the list
    void sort(BookFrequencyNode* current) {
        // Sort the current node by frequency by moving it up the list
        while (current->prev != nullptr && current->frequency > current->prev->frequency) {
            // Swap current with previous node
            BookFrequencyNode* prevNode = current->prev;

            // Update the previous and next pointers
            if (prevNode->prev) prevNode->prev->next = current;
            if (current->next) current->next->prev = prevNode;

            current->prev = prevNode->prev;
            prevNode->next = current->next;
            current->next = prevNode;
            prevNode->prev = current;

            // If we swapped with the head, update the head pointer
            if (prevNode == head) {
                head = current;
            }
        }
    }
};

// Hash table node for storing word frequencies per book
// Each word in the hash table is represented by a HashNode, which contains the word itself, a map of book IDs to frequencies, and a pointer to the next node in the linked list.
struct HashNode {
    // std::string key;
    // std::unordered_map<int, int> bookFrequency;  // Book ID -> frequency
    HashNode* next;
    std::string key;
    BookFrequencyList bookFrequency;  // Book ID -> frequency

    HashNode(const std::string& k) : key(k), next(nullptr) {}
};

// HashTable class
// The HashTable class stores words and their frequencies for each book in a hash table.
// It uses a hash function to convert a word into an index, and handles collisions by chaining.
class HashTable {
private:
    // The hash table is implemented as a vector of HashNode pointers
    VictorVector<HashNode*> table;
    int TABLE_SIZE;
    int numElements;
    const double LOAD_FACTOR_THRESHOLD = 0.75;

    // Hash function that converts a string (word) into an index
    int hashFunction1(const std::string& key) {
        if (key.empty()) {
            std::cerr << "Error: Empty or invalid key!" << std::endl;
            return 0;
        }

        unsigned long hash = 0;
        for (char ch : key) {
            hash = (hash * 31) + ch;
        }
        return hash % TABLE_SIZE;
    }

    int hashFunction2(const std::string& key) {
        if (key.empty()) {
            std::cerr << "Error: Empty or invalid key!" << std::endl;
            return 1;
        }

        unsigned long hash = 0;
        for (char ch : key) {
            hash = (hash * 37) + ch;  // different hash function than the first one
        }
        int secondaryHash = 7 - (hash % 7);  // prime number less than table size
        
        if (secondaryHash == 0) {
            secondaryHash = 1;
        }
        return secondaryHash;
    }

    void resize() {
        int oldTableSize = TABLE_SIZE;
        TABLE_SIZE = (TABLE_SIZE * 2) + 1; // relatively prime number
        std::cout << "Resizing hash table to " << TABLE_SIZE << std::endl;
        VictorVector<HashNode*> newTable(TABLE_SIZE, nullptr);

        for (int i = 0; i < oldTableSize; ++i) {
            HashNode* entry = table[i];
            if (entry == nullptr) {
                continue;
            }
            int newIndex = hashFunction1(entry->key);
            int step = hashFunction2(entry->key);
            while (newTable[newIndex] != nullptr) {
                newIndex = (newIndex + step) % TABLE_SIZE;
            }
            newTable[newIndex] = entry;
        }

        table = std::move(newTable);
    }

public:
    // Constructor to initialize the hash table with a specific size
    HashTable() : TABLE_SIZE(100), numElements(0) {
        table.resize(TABLE_SIZE, nullptr);
    }

    ~HashTable() {
        for (int i = 0; i < TABLE_SIZE; ++i) {
            HashNode* entry = table[i];
            while (entry != nullptr) {
                HashNode* temp = entry;
                entry = entry->next;
                delete temp;
            }
        }
    }

    // Insert a word and update its frequency for a specific book ID
    void insert(const std::string& word, int bookID, Trie& trie) {
        if ((double)numElements / TABLE_SIZE > LOAD_FACTOR_THRESHOLD) {
            resize();
        }

        if (word.empty()) {
            std::cerr << "Skipping invalid or empty word" << std::endl;
            return;
        }

        // Calculate the hash index for the word
        int hashIndex = hashFunction1(word);
        int step = hashFunction2(word);
        // Get the head of the linked list at the hash index
        HashNode* entry = table[hashIndex];

        // Check if the word already exists in the table
        while (entry != nullptr) {
            if (entry->key == word) {
                entry->bookFrequency.insert(bookID);
                trie.insert(word);
                return;
            }
            // Collision detected, apply double hashing
            hashIndex = (hashIndex + step) % TABLE_SIZE;
            entry = table[hashIndex];  // Move to the next probed index
        }

        entry = new HashNode(word);  // Create a new HashNode for this word
        table[hashIndex] = entry;    // Assign it to the hash table
        numElements++;

        entry->bookFrequency.insert(bookID);
        
        // Insert word into the trie
        trie.insert(word);
    }

    // Helper function for serializing the hash table
    void serializeHelper(std::ofstream& outFile) {
        outFile << TABLE_SIZE << "\n";  // Write the table size
        for (int i = 0; i < TABLE_SIZE; ++i) {
            HashNode* entry = table[i];
            while (entry != nullptr) {
                outFile << entry->key;  // Write the word

                BookFrequencyNode* current = entry->bookFrequency.head;
                while (current != nullptr) {
                    outFile << " " << current->bookID << ":" << current->frequency;  // Write Book ID and Frequency
                    current = current->next;
                }
                outFile << "\n";  // End of the current node entry

                entry = entry->next;  // Move to the next HashNode
            }
        }
    }

    // Serialize the hash table into a file
    void serialize(std::ofstream& outFile) {
        serializeHelper(outFile);
    }
};

#endif


