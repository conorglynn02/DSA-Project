#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <sstream> 

const int TABLE_SIZE = 100;
const int numberOfBooks = 10;

// Trie node definition
struct TrieNode {
    std::unordered_map<char, TrieNode*> children;
    bool isEndOfWord = false;
    std::string word;
};

// Manually implemented Trie class
class Trie {
private:
    TrieNode* root;

public:
    Trie() {
        root = new TrieNode();
    }

    // Helper function for deserializing the trie
    void deserializeHelper(TrieNode* node, std::ifstream& inFile) {
        std::string line;
        while (getline(inFile, line)) {
            if (line == "END") {
                return;
            }

            // Parse the character, isEndOfWord, and the word (if end of word)
            char ch;
            int isEndOfWord;
            std::string word = "";

            std::istringstream iss(line);
            iss >> ch >> isEndOfWord;

            TrieNode* child = new TrieNode();
            node->children[ch] = child;

            if (isEndOfWord) {
                iss >> word;
                child->isEndOfWord = true;
                child->word = word;
            }

            // Recursively deserialize the child nodes
            deserializeHelper(child, inFile);
        }
    }

    // Deserialize the trie from a file
    void deserialize(std::ifstream& inFile) {
        deserializeHelper(root, inFile);
    }

    // Helper function for DFS to find all words that match the prefix
    void autoCompleteHelper(TrieNode* node, std::vector<std::string>& results) {
        if (node->isEndOfWord) {
            results.push_back(node->word);
        }
        for (auto& child : node->children) {
            autoCompleteHelper(child.second, results);
        }
    }

    // Function to find all words with the given prefix
    std::vector<std::string> autoComplete(const std::string& prefix) {
        TrieNode* node = root;
        std::vector<std::string> results;

        for (char ch : prefix) {
            if (node->children.find(ch) == node->children.end()) {
                return results;  // No words match this prefix
            }
            node = node->children[ch];
        }

        // Perform DFS from this node to find all words that match the prefix
        autoCompleteHelper(node, results);
        return results;
    }
};

struct HashNode {
    std::string key;
    std::unordered_map<int, int> bookFrequency;  // Book ID -> frequency
    HashNode* next;

    HashNode(const std::string& k) : key(k), next(nullptr) {}
};

// HashTable class
class HashTable {
private:
    std::vector<HashNode*> table;
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
    HashTable() {
        table.resize(TABLE_SIZE, nullptr);
    }

    // Insert a word and update its frequency for a specific book ID
    void insert(const std::string& word, int bookID) {
        int hashIndex = hashFunction(word);
        HashNode* prev = nullptr;
        HashNode* entry = table[hashIndex];

        // Traverse the linked list to find the word in the hash table
        while (entry != nullptr && entry->key != word) {
            prev = entry;
            entry = entry->next;
        }

        if (entry == nullptr) {  // Word not found, insert new node
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

    void search(std::string word) {
        int hashIndex = hashFunction(word);
        HashNode* prev = nullptr;
        HashNode* entry = table[hashIndex];

        while (entry != nullptr && entry->key != word) {
            prev = entry;
            entry = entry->next;
        }

        if (entry == nullptr) { // Word is not found
            std::cout << "Word not found" << std::endl;
            return;
        } else { // Word is found
            std::cout << "Word found: " << std::endl;
            for (int i = 1; i <= numberOfBooks; i++) {
                if (entry->bookFrequency[i]) {
                    std::cout << "Book id: " << i << " Count: " << entry->bookFrequency[i] << std::endl;
                }
            }
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

    // Helper function for deserializing the hash table
    void deserializeHelper(std::ifstream& inFile) {
        std::string line;
        while (std::getline(inFile, line)) {
            std::istringstream iss(line);
            std::string word;
            iss >> word;  // Read the word

            int bookID, frequency;
            while (iss >> bookID) {
                char colon;  // Read the colon separating book ID and frequency
                iss >> colon; // Skip the colon
                iss >> frequency;  // Read the frequency
                insert(word, bookID);  // Insert into the hash table
            }
        }
    }

    // Deserialize the hash table from a file
    void deserialize(std::ifstream& inFile) {
        deserializeHelper(inFile);
    }
};

// Function to perform autocompletion using the pre-built trie
void autoCompleteSearch(Trie& trie) {
    std::string prefix;
    std::cout << "Enter search prefix: ";
    std::cin >> prefix;

    std::vector<std::string> results = trie.autoComplete(prefix);
    std::cout << "Autocomplete results for prefix \"" << prefix << "\":\n";
    for (const std::string& result : results) {
        std::cout << result << "\n";
    }
}

int main() {
    Trie trie;
    HashTable hashtable;

    // Deserialize the trie from the file
    std::ifstream inFile("trie_data.txt");
    if (!inFile) {
        std::cerr << "Failed to open trie data file.\n";
        return 1;
    }
    trie.deserialize(inFile);
    inFile.close();

    // Deserialize the trie from the file
    std::ifstream inFileHashTable("hash_table_data.txt");
    if (!inFileHashTable) {
        std::cerr << "Failed to open hash table data file.\n";
        return 1;
    }
    hashtable.deserialize(inFileHashTable);
    inFileHashTable.close();

    hashtable.search("my");

    // hashtable.display();

    // Perform autocompletion search
    autoCompleteSearch(trie);

    return 0;
}