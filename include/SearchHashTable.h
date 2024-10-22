#ifndef SEARCHHASHTABLE_H
#define SEARCHHASHTABLE_H

#include "Trie.h"
#include "MaxHeap.h"

// Map to store Book ID -> Book Name
std::unordered_map<int, std::string> bookIdToNameMap;

void displayBookWithId(int count, int bookID, int frequency) {
    if (bookIdToNameMap.find(bookID) != bookIdToNameMap.end()) {
        std::cout << count + 1 << " -> " << "Book ID: " << bookID << ", Book Name: " << bookIdToNameMap[bookID] << ", Frequency: " << frequency << std::endl;
    } else {
        std::cout << "Book ID: " << bookID << " (Book name not found), Frequency: " << frequency << std::endl;
    }
}

struct BookFrequencyNode {
    int bookID;
    int frequency;
    BookFrequencyNode* next;
    BookFrequencyNode* prev;

    BookFrequencyNode(int id, int freq) : bookID(id), frequency(freq), next(nullptr), prev(nullptr) {}
};

class BookFrequencyList {
public:
    BookFrequencyNode* head;

    BookFrequencyList() : head(nullptr) {}

    void insert(int bookID, int frequency) {
        BookFrequencyNode* newNode = new BookFrequencyNode(bookID, frequency);

        // If the list is empty, set the new node as the head
        if (head == nullptr) {
            head = newNode;
            return;
        }

        // Traverse to the tail of the list
        BookFrequencyNode* current = head;
        while (current->next != nullptr) {
            current = current->next;
        }

        // Insert the new node at the end (tail)
        current->next = newNode;
        newNode->prev = current;       
    }

    // Sort the list by frequency in descending order
    void sort(BookFrequencyNode* current) {
        while (current->prev != nullptr && current->frequency > current->prev->frequency) {
            BookFrequencyNode* prevNode = current->prev;

            if (prevNode->prev) prevNode->prev->next = current;
            if (current->next) current->next->prev = prevNode;

            current->prev = prevNode->prev;
            prevNode->next = current->next;
            current->next = prevNode;
            prevNode->prev = current;

            if (prevNode == head) {
                head = current;
            }
        }
    }

    // Function to display the list of X to Y books
    bool displayXToYBooks(int start, int end) {
        BookFrequencyNode* current = head;
        int count = 0;
        while (current != nullptr && count < end) {
            while (current != nullptr && count < start) {
                current = current->next;
                count++;
            }
            displayBookWithId(count, current->bookID, current->frequency); // Pass both book ID and frequency
            count++;
            current = current->next;
        }

        return (current == nullptr);
    }
};

struct HashNode {
    std::string key;
    BookFrequencyList bookFrequency;  // Linked list of Book ID -> frequency
    HashNode* next;

    HashNode(const std::string& k) : key(k), next(nullptr) {}
};

// HashTable class
class HashTable {
private:
    std::vector<HashNode*> table;
    int TABLE_SIZE;

    // Hash function that converts a string (word) into an index
    int hashFunction1(const std::string& key) {
        unsigned long hash = 0;
        for (char ch : key) {
            hash = (hash * 31) + ch;
        }
        return hash % TABLE_SIZE;
    }

    int hashFunction2(const std::string& key) {
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

public:
    HashTable() : TABLE_SIZE(100) {
        table.resize(TABLE_SIZE, nullptr);
    }

    BookFrequencyList* getBookFrequencyList(const std::string& word) {
        int hashIndex = hashFunction1(word);
        int step = hashFunction2(word);
        HashNode* entry = table[hashIndex];

        while (entry != nullptr && entry->key != word) {
            hashIndex = (hashIndex + step) % TABLE_SIZE;
            entry = table[hashIndex];  // Move to the next probed index
        }

        if (entry == nullptr) {
            return nullptr;
        } else {
            return &entry->bookFrequency;
        }
    }

    void search(const std::string& word) {
        int hashIndex = hashFunction1(word);
        int step = hashFunction2(word);
        HashNode* entry = table[hashIndex];

        while (entry != nullptr && entry->key != word) {
            hashIndex = (hashIndex + step) % TABLE_SIZE;
            entry = table[hashIndex];  // Move to the next probed index
        }

        if (entry == nullptr) {
            printWithColor("Word not Found: ", "1;31");
            std::cout << std::endl;
            return;
        } else {
            printWithColor("Word Found: ", "1;33");
            std::cout << entry->key << std::endl;
            int start = 0;
            int end = 10;
            bool isEnd = entry->bookFrequency.displayXToYBooks(start, end);
            if (isEnd) {
                std::cout << "No more books match this search." << std::endl;
                return;
            }
            std::string message = "To see next 10, type \"next\", else press any key and enter: ";
            printWithColor(message, "1;32");
            std::string option;
            std::cin >> option;
            while (option == "next" && !isEnd) {
                start += 10;
                end += 10;
                isEnd = entry->bookFrequency.displayXToYBooks(start, end);
                if (isEnd) {
                    break;
                }
                printWithColor(message, "1;32");
                std::cin >> option;
            }
            if (isEnd) {
                printWithColor("No more books match this search." , "1;31");
                std::cout << std::endl;
                return;
            }
            std::cout << "Search exited successfully." << std::endl;
            return;
        }
    }

    // Helper function for deserializing the hash table
    void deserializeHelper(std::ifstream& inFile) {
        std::string line;
        std::getline(inFile, line);  // Read the first line containing the table size
        TABLE_SIZE = std::stoi(line);
        // Resize the table to the new size
        table.resize(TABLE_SIZE, nullptr);

        while (std::getline(inFile, line)) {
            std::istringstream iss(line);
            std::string word;
            iss >> word;  // Read the word

            int bookID, frequency;
            while (iss >> bookID) {
                char colon;  // Read the colon separating book ID and frequency
                iss >> colon; // Skip the colon
                iss >> frequency;  // Read the frequency

                // Insert into the HashNode's BookFrequencyList
                insertToList(word, bookID, frequency);
            }
        }
    }

    // Insert word into the HashTable with frequency as linked list
    void insertToList(const std::string& word, int bookID, int frequency) {
        int hashIndex = hashFunction1(word);
        int step = hashFunction2(word);
        HashNode* entry = table[hashIndex];

        // Check if the word already exists in the table
        while (entry != nullptr) {
            if (entry->key == word) {
                entry->bookFrequency.insert(bookID, frequency);
                return;
            }
            // Collision detected, apply double hashing
            hashIndex = (hashIndex + step) % TABLE_SIZE;
            entry = table[hashIndex];  // Move to the next probed index
        }

        entry = new HashNode(word);  // Create a new HashNode for this word
        table[hashIndex] = entry;    // Assign it to the hash table

        entry->bookFrequency.insert(bookID, frequency);
    }

    // Deserialize the hash table from a file
    void deserialize(std::ifstream& inFile) {
        deserializeHelper(inFile);
    }
};

#endif