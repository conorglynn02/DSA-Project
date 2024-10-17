#include <iostream>
#include <fstream>
#include <unordered_map>
#include <sstream>
#include <vector>
#include <filesystem> // To iterate over files in the folder

namespace fs = std::filesystem;

const int TABLE_SIZE = 10000000;
int numberOfBooks = 0;
int mapSize = 0;

// template <typename T>
// class victorTheVector {
// private:
//     T* currentVector;   // Pointer to the underlying array
//     int capacity;
//     int currentSize;

// public:
//     victorTheVector(int size = 10) {
//         capacity = size;
//         currentSize = 0;
//         currentVector = new T[capacity];
//     }

//     // // Destructor to free the dynamically allocated memory
//     // ~victorTheVector() {
//     //     delete[] currentVector;
//     // }

//     // Resize array when it's full
//     void resize() {
//         capacity *= 2; // Double the capacity
//         T* newVector = new T[capacity]; // Allocate a new array with the new capacity
        
//         // Copy elements from old array to new array
//         for (int i = 0; i < currentSize; i++) {
//             newVector[i] = currentVector[i];
//         }
        
//         delete[] currentVector;  // Delete the old array
//         currentVector = newVector;  // Point to the new array
//     }

//     // Method to add an element to the vector
//     void push_back(T element) {
//         // If array is full, resize it
//         if (currentSize == capacity) {
//             resize();
//         }
        
//         currentVector[currentSize] = element;   // Add the new element
//         currentSize++;  // Increment the size
//     }

//     T& operator[](int index) {
//         if (index >= currentSize || index < 0) {
//             throw std::out_of_range("Index out of range");
//         }
//         return currentVector[index];
//     }

//     int size() {
//         return currentSize;
//     }
// };

struct BookFrequencyNode {
    int bookID;
    int frequency;
    BookFrequencyNode* next;
    BookFrequencyNode* prev;

    BookFrequencyNode(int id, int freq) : bookID(id), frequency(freq), next(nullptr) {}
};

// Book Frequency List class
class BookFrequencyList {
public:
    BookFrequencyNode* head;

public:
    BookFrequencyList() : head(nullptr) {}
    void insert(int bookID, int frequency) {
        // Case 1: checking if the list is empty and inserting the first node
        if (head == nullptr) {
            BookFrequencyNode* newNode = new BookFrequencyNode(bookID, frequency);
            head = newNode;
            head->prev = nullptr;
            mapSize++;
            return;
        }

        // Case 2: checking if book ID is already in the list
        BookFrequencyNode* current = head;
        while (current != nullptr) {
            if (current->bookID == bookID) {
                current->frequency += frequency;
                sort(current); // Sort the node based on the frequency
                return;
            }
            current = current->next;
        }

        // Case 3: inserting a new node if bookID is not in the list
        BookFrequencyNode* newNode = new BookFrequencyNode(bookID, frequency);
        mapSize++;

        insertSorted(newNode);
        
        // newNode->prev = nullptr;
        // newNode->next = head;
        // head->prev = newNode;
        // head = newNode;  // Insert at the beginning for simplicity

        // Case 3: inserting a new node at the end if bookID is not in the list
        // insert it at the end of the list


        // // Case 1: checking to see if the list is empty and inserting the first node
        // if (head == nullptr) {
        //     BookFrequencyNode* newNode = new BookFrequencyNode(bookID, 1);
        //     head = newNode;
        //     head->prev = nullptr;
        //     return;
        // }

        // // Case 2: checking to see if book ID is already in the list
        // BookFrequencyNode* current = head;
        // while (current->next != nullptr) {
        //     if (current->bookID == bookID) {
        //         current->frequency++;
        //         sort(current);
        //         return;
        //     }
        //     current = current->next;
        // }
        // if (current->bookID == bookID) {
        //     current->frequency++;
        //     sort(current);
        //     return;
        // }


        // // Case 3: inserting a new node if bookid is not in the list
        // BookFrequencyNode* newNode = new BookFrequencyNode(bookID, 1);

        // //inserting new node at the end of the list
        // current->next = newNode;
        // newNode->prev = current;
        // newNode->next = nullptr;
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


        // // Already at the head, no need to sort
        // if (current->prev == nullptr) {
        //     return;
        // }

        // // Check if already sorted
        // if (current->frequency <= current->prev->frequency) {
        //     return;
        // }

        // BookFrequencyNode* sortedNode = current;


        // while (current->prev != nullptr) {
        //     if (current->frequency > sortedNode->frequency) {
        //         // Traverse list right to left
        //         sortedNode->next->prev = current->next;
        //         current->next->next = sortedNode->next;
        //         sortedNode->next = current->next;
        //         sortedNode->prev = current;
        //         current->next->prev = sortedNode;
        //         current->next = sortedNode;
        //         return;
        //     }
        //     current = current->prev;
        // }

        // // If the sorted node is the new head
        // if (current->prev == nullptr) {
        //     sortedNode->prev->next = sortedNode->next;
        //     current->prev = sortedNode;
        //     sortedNode->next = current;
        //     sortedNode->prev = nullptr;
        //     head = sortedNode;
        // }

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
    // HashNode* next;

    HashNode(const std::string& k) : key(k), bookFrequency() {}
    //, next(nullptr) {}
};


// Trie node definition
// Each node in the trie contains a map of characters to child nodes, a flag to indicate the end of a word, and the word itself (if it is the end of a word).
struct TrieNode {
    std::unordered_map<char, TrieNode*> children;
    bool isEndOfWord = false;
    std::string word;
};

// Manually implemented Trie class
// The Trie class provides methods to insert words into the trie and serialize the trie into a file.
class Trie {
private:
    TrieNode* root;

public:
    // Constructor to initialize the root node
    Trie() {
        root = new TrieNode();
    }

    // Insert a word into the trie
    void insert(const std::string& word) {
        // Start from the root node
        TrieNode* node = root;
        // Traverse the trie, creating new nodes as needed
        for (char ch : word) {
            // If the character is not present in the children map, create a new node
            if (node->children.find(ch) == node->children.end()) {
                // Create a new node for the character
                node->children[ch] = new TrieNode();
            }
            // Move to the child node corresponding to the character
            node = node->children[ch];
        }
        // Mark the end of the word and store the word in the node
        node->isEndOfWord = true;
        // Store the word in the node
        node->word = word;
    }

    // Helper function for serializing the trie
    // This function performs a depth-first traversal of the trie and writes the nodes to a file
    void serializeHelper(TrieNode* node, std::ofstream& outFile) {
        // Base case: If the node is null, return
        if (!node) return;
        // Traverse the children of the node
        for (const auto& pair : node->children) {
            // Write the character, isEndOfWord flag, and the word to the file
            outFile << pair.first << " " << pair.second->isEndOfWord;
            // If it is the end of a word, write the word itself
            if (pair.second->isEndOfWord) {
                outFile << " " << pair.second->word;
            }
            // Write a newline character to separate entries
            outFile << "\n";

            // Recursive call for child nodes
            serializeHelper(pair.second, outFile);
        }
        
        // Mark the end of children for this node
        outFile << "END\n";
    }

    // Serialize the trie into a file
    void serialize(std::ofstream& outFile) {
        serializeHelper(root, outFile);
    }
};

// HashTable class
// The HashTable class stores words and their frequencies for each book in a hash table.
// It uses a hash function to convert a word into an index, and handles collisions by chaining.
class HashTable {
private:
    // The hash table is implemented as a vector of HashNode pointers
    // victorTheVector<HashNode*> table;
    // std::vector<HashNode*> table;
    HashNode** table;

    // The size of the hash table
    int TABLE_SIZE = 100;
    float loadFactor = 0.75;

    // Hash function that converts a string (word) into an index
    // int hashFunction1(const std::string& key) {
    //     unsigned long hash = 0;
    //     for (char ch : key) {
    //         hash = (hash * 31) + ch;
    //     }
    //     return hash % TABLE_SIZE;
    // }

    // int hashFunction2(const std::string& key) {
    //     int hash =hashFunction1(key);
    //     int magicNumber = 13 - (hash % 13);
    //     if (magicNumber == 0) {
    //         magicNumber = 1;
    //     }
    //     return magicNumber;
    // }

    int firstHash(const std::string& word) {
        size_t hash = std::hash<std::string>()(word); 
        return hash % TABLE_SIZE;
    }

    int secondHash(const std::string& word) {
        size_t hash = std::hash<std::string>()(word); 
        return (7 - (hash % 7));
    }

public:
    // Constructor to initialize the hash table with a specific size
    HashTable() {
        // table.resize(TABLE_SIZE, nullptr);
        // table = victorTheVector<HashNode*>(TABLE_SIZE);
        // table = std::vector<HashNode*>(TABLE_SIZE, nullptr);
        table = new HashNode*[TABLE_SIZE];
    }
    // Insert a word and update its frequency for a specific book ID
    void insert(const std::string& word, int bookID, Trie& trie) {
        // if table has too high load factor, then rehash
        if ((float(mapSize) / TABLE_SIZE) >= loadFactor) {
            rehash(trie);
        }
        // Calculate the hash index for the word
        // int hashIndex = hashFunction1(word);
        // int hash2 = hashFunction2(word);
        int hashIndex = firstHash(word);
        int hash2 = secondHash(word);

        while(table[hashIndex] != nullptr && table[hashIndex]->key != word) {
            hashIndex = (hashIndex + hash2) % TABLE_SIZE;
        }
        // std::cout << "Inserting word: " << word << " with book ID: " << bookID << " at hash index: " << hashIndex << "\n";

        // Initialize pointers for traversal
        HashNode* prev = nullptr;
        // Get the head of the linked list at the hash index
        HashNode* entry = table[hashIndex];

        // If the entry is null, create a new HashNode
        if (entry == nullptr) {
            entry = new HashNode(word);  // Create a new HashNode for this word
            table[hashIndex] = entry;    // Assign it to the hash table
        }

        entry->bookFrequency.insert(bookID, 1);

        // Insert word into the trie
        trie.insert(word);
    }

    void rehash(Trie& trie) {
        int oldSize = TABLE_SIZE;
        TABLE_SIZE = (TABLE_SIZE * 2) + 1;
        // std::cout << "Rehashing... table size is now "<< TABLE_SIZE << "\n";
        // std::cout << "Map size is " << mapSize << "\n";

        // victorTheVector<HashNode*> oldTable = victorTheVector<HashNode*>(oldSize);
        
        // std::vector<HashNode*> oldTable;
        // for (int i = 0; i < oldSize; i++) {
        //     if (table[i] != nullptr) {
        //         HashNode* newNode = new HashNode(*table[i]);  // Create a new HashNode copy
        //         // oldTable.push_back(newNode);
        //         oldTable[i] = newNode;
        //     }
        //     else {
        //         oldTable.push_back(nullptr);
        //     }
        // }

        // std::vector<HashNode*> oldTable = table;
        HashNode** oldTable = table;
        // table = new HashNode<K, V>*[capacity]; 

        // table = victorTheVector<HashNode*>(TABLE_SIZE);
        // table = std::vector<HashNode*>(TABLE_SIZE, nullptr);
        table = new HashNode*[TABLE_SIZE];

        for (int i = 0; i < TABLE_SIZE; i++) {
            table[i] = nullptr;
        }

        mapSize = 0;

        // need to add this function to the indexer too.

        for(int i = 0; i < oldSize; i++) {
            HashNode* entry = oldTable[i];
            while (entry != nullptr) {
                // Iterate through the bookFrequency list for each HashNode
                BookFrequencyNode* freqNode = entry->bookFrequency.head;
                while (freqNode != nullptr) {
                    // Re-insert the word and book ID/frequency into the new table
                    insert(entry->key, freqNode->bookID, trie);  // Insert each bookID and update frequency inside insert()
                    freqNode = freqNode->next;
                }
                entry = entry->next;
            }
        }
        // delete[] oldTable;

        // table = newTable;
    }

    // Display the hash table contents (for debugging)
    // void display() {
    //     for (int i = 0; i < TABLE_SIZE; ++i) {
    //         HashNode* entry = table[i];
    //         if (entry != nullptr) {
    //             std::cout << "Bucket " << i << ":\n";
    //             while (entry != nullptr) {
    //                 std::cout << "  Word: " << entry->key << "\n";
    //                 for (const auto& freq : entry->bookFrequency) {
    //                     std::cout << "    Book ID: " << freq.first << ", Frequency: " << freq.second << "\n";
    //                 }
    //                 entry = entry->next;
    //             }
    //         }
    //     }
    // }

    // Helper function for serializing the hash table
    void serializeHelper(std::ofstream& outFile) {
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

// Function to read words from a text file and index them
void indexWordsFromFile(const std::string& filename, HashTable& hashTable, Trie& trie, int bookID) {
    std::ifstream file(filename);
    std::string word;
    
    while (file >> word) {
        hashTable.insert(word, bookID, trie);
    }
    
    file.close();
}

int main() {
    HashTable hashTable;
    Trie trie;

    // Path to the folder containing books
    std::string booksFolder = "./Books";

    // Book ID counter (1 for the first book, 2 for the second, etc.)
    int bookID = 1;

    // Iterate through the "books" folder to get each file
    for (const auto& entry : fs::directory_iterator(booksFolder)) {
        std::string filename = entry.path().string();

        // Index words from the current book file
        std::cout << "Indexing book: " << filename << " (Book ID: " << bookID << ")\n";
        indexWordsFromFile(filename, hashTable, trie, bookID);

        // Increment the book ID for the next book
        bookID++;
        break;
    }

    // hashTable.display();

    // Serialize the trie to a file
    // std::ofstream outFile("trie_data.txt");
    // trie.serialize(outFile);
    // outFile.close();

    // Serialize the hash table to a file
    std::ofstream hashTableFile("hash_table_data.txt");
    hashTable.serialize(hashTableFile);
    hashTableFile.close();

    std::cout << "Hash table serialized.\n";

    std::cout << "Indexing complete, index and trie serialized.\n";

    return 0;
}
