#include <iostream>
#include <fstream>
#include <unordered_map>
#include <sstream>
#include <vector>
#include <filesystem> // To iterate over files in the folder

namespace fs = std::filesystem;

const int TABLE_SIZE = 100;

// Hash table node for storing word frequencies per book
// Each word in the hash table is represented by a HashNode, which contains the word itself, a map of book IDs to frequencies, and a pointer to the next node in the linked list.
struct HashNode {
    std::string key;
    std::unordered_map<int, int> bookFrequency;  // Book ID -> frequency
    HashNode* next;

    HashNode(const std::string& k) : key(k), next(nullptr) {}
};

// HashTable class
// The HashTable class stores words and their frequencies for each book in a hash table.
// It uses a hash function to convert a word into an index, and handles collisions by chaining.
class HashTable {
private:
    // The hash table is implemented as a vector of HashNode pointers
    std::vector<HashNode*> table;

    // The size of the hash table
    int TABLE_SIZE = 100;

    // Hash function that converts a string (word) into an index
    int hashFunction(const std::string& key) {
        unsigned long hash = 0;
        for (char ch : key) {
            hash = (hash * 31) + ch;
        }
        return hash % TABLE_SIZE;
    }

public:
    // Constructor to initialize the hash table with a specific size
    HashTable() {
        table.resize(TABLE_SIZE, nullptr);
    }

    // Insert a word and update its frequency for a specific book ID
    void insert(const std::string& word, int bookID, Trie& trie) {
        // Calculate the hash index for the word
        int hashIndex = hashFunction(word);
        // Initialize pointers for traversal
        HashNode* prev = nullptr;
        // Get the head of the linked list at the hash index
        HashNode* entry = table[hashIndex];

        // Insert word into the trie
        trie.insert(word);

        // Traverse the linked list to find the word in the hash table
        while (entry != nullptr && entry->key != word) {
            prev = entry;
            entry = entry->next;
        }

        // Word not found, insert new node
        if (entry == nullptr) {
            entry = new HashNode(word);
            entry->bookFrequency[bookID] = 1;  // First occurrence of the word in the book
            if (prev == nullptr) {  // No collision
                table[hashIndex] = entry;
            } else {  // Collision occurred, add new node to the chain
                prev->next = entry;
            }
        } else {  // Word found, update frequency
            entry->bookFrequency[bookID]++;
        }
    }

    // Display the hash table contents (for debugging)
    void display() {
        for (int i = 0; i < TABLE_SIZE; ++i) {
            HashNode* entry = table[i];
            if (entry != nullptr) {
                std::cout << "Bucket " << i << ":\n";
                while (entry != nullptr) {
                    std::cout << "  Word: " << entry->key << "\n";
                    for (const auto& freq : entry->bookFrequency) {
                        std::cout << "    Book ID: " << freq.first << ", Frequency: " << freq.second << "\n";
                    }
                    entry = entry->next;
                }
            }
        }
    }
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
    }

    // Serialize the trie to a file
    std::ofstream outFile("trie_data.txt");
    trie.serialize(outFile);
    outFile.close();

    std::cout << "Indexing complete and trie serialized.\n";

    return 0;
}
