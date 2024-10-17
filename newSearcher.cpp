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
        // Case 1: Insert the first node
        if (head == nullptr) {
            BookFrequencyNode* newNode = new BookFrequencyNode(bookID, frequency);
            head = newNode;
            return;
        }

        // Case 2: Check if bookID already exists in the list
        BookFrequencyNode* current = head;
        while (current != nullptr) {
            if (current->bookID == bookID) {
                current->frequency += frequency;
                sort(current); // Sort the node based on updated frequency
                return;
            }
            current = current->next;
        }

        // Case 3: Insert new node at the beginning
        BookFrequencyNode* newNode = new BookFrequencyNode(bookID, frequency);
        newNode->next = head;
        if (head != nullptr) head->prev = newNode;
        head = newNode;
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

    // Function to display the list (for debugging)
    void display() {
        BookFrequencyNode* current = head;
        while (current != nullptr) {
            std::cout << "Book ID: " << current->bookID << ", Frequency: " << current->frequency << std::endl;
            current = current->next;
        }
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
    void insertX(const std::string& word, int bookID, int frequency) {
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
            entry->bookFrequency.insert(bookID, frequency);  // Insert into the linked list
            if (prev == nullptr) {  // No collision
                table[hashIndex] = entry;
            } else {  // Collision occurred, add new node to the chain
                prev->next = entry;
            }
        } else {  // Word found, update frequency in the linked list
            entry->bookFrequency.insert(bookID, frequency);
        }
    }

    void search(const std::string& word) {
        int hashIndex = hashFunction(word);
        HashNode* entry = table[hashIndex];

        while (entry != nullptr && entry->key != word) {
            entry = entry->next;
        }

        if (entry == nullptr) {
            std::cout << "Word not found" << std::endl;
            return;
        } else {
            std::cout << "Word found: " << entry->key << std::endl;
            entry->bookFrequency.display();  // Display the book frequency linked list
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

                // Insert into the HashNode's BookFrequencyList
                insertToList(word, bookID, frequency);
            }
        }
    }

    // Insert word into the HashTable with frequency as linked list
    void insertToList(const std::string& word, int bookID, int frequency) {
        int hashIndex = hashFunction(word);
        HashNode* entry = table[hashIndex];

        // If word does not exist in the table, create a new node
        if (entry == nullptr) {
            entry = new HashNode(word);
            table[hashIndex] = entry;
        }

        // Insert into the BookFrequencyList
        entry->bookFrequency.insert(bookID, frequency);
    }

    // Deserialize the hash table from a file
    void deserialize(std::ifstream& inFile) {
        deserializeHelper(inFile);
    }
};

// Function to perform autocompletion using the pre-built trie
void autoCompleteSearch(Trie& trie) {
    std::string prefix;
    std::cin >> prefix;
    std::vector<std::string> results = trie.autoComplete(prefix);
    std::cout << "Autocomplete results for prefix \"" << prefix << "\":\n";
    for (const std::string& result : results) {
        std::cout << result << "\n";
    }
}

void displayMenu() {
    std::cout << "Choose an option: \n";
    std::cout << "1. Search for a word\n";
    std::cout << "2. Autocomplete\n";
    std::cout << "3. Exit\n";
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

    int choice;
    std:: string word;

    while (true) {
        displayMenu();
        std::cin >> choice;

        switch (choice) {
            case 1:
                std::cout << "Enter a word to search: ";
                std::cin >> word;
                hashtable.search(word);
                break;
            case 2:
                std::cout << "Enter search prefix: ";
                autoCompleteSearch(trie);
                break;
            case 3:
                std::cout << "Exiting program.\n";
                return 0;
            default:
                std::cout << "Invalid choice. Please try again.\n";
                break;
        }
    }

    // hashtable.search("my");

    // hashtable.display();

    // Perform autocompletion search
    // autoCompleteSearch(trie);

    return 0;
}