#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <sstream> 
#include <unistd.h>

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

class MaxHeap {
private:
    std::vector<std::pair<int, int>> heap;  // pair of bookID and combinedFrequency

    void heapifyDown(int idx) {
        int largest = idx;
        int left = 2 * idx + 1;
        int right = 2 * idx + 2;

        // Compare with left child
        if (left < heap.size() && heap[left].second > heap[largest].second) {
            largest = left;
        }

        // Compare with right child
        if (right < heap.size() && heap[right].second > heap[largest].second) {
            largest = right;
        }

        if (largest != idx) {
            std::swap(heap[idx], heap[largest]);
            heapifyDown(largest);
        }
    }

    void heapifyUp(int idx) {
        while (idx > 0) {
            int parent = (idx - 1) / 2;
            if (heap[idx].second > heap[parent].second) {
                std::swap(heap[idx], heap[parent]);
                idx = parent;
            } else {
                break;
            }
        }
    }

public:
    void insert(int bookID, int combinedFrequency) {
        heap.push_back({bookID, combinedFrequency});
        heapifyUp(heap.size() - 1);
        if (heap.size() > 10) {
            extractMax();
        }
    }

    std::pair<int, int> extractMax() {
        if (heap.empty()) return {-1, -1};  // Edge case for empty heap
        std::pair<int, int> maxValue = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        heapifyDown(0);
        return maxValue;
    }

    void displayTop() {
        for (auto& entry : heap) {
            std::cout << "Book ID: " << entry.first << ", Combined Frequency: " << entry.second << std::endl;
        }
    }
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

    // Function to display the list (for debugging)
    void display() {
        BookFrequencyNode* current = head;
        int count = 0;
        while (current != nullptr && count < 10) {
            std::cout << "Book ID: " << current->bookID << ", Frequency: " << current->frequency << std::endl;
            current = current->next;
            count++;
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

    BookFrequencyList* getBookFrequencyList(const std::string& word) {
        int hashIndex = hashFunction(word);
        HashNode* entry = table[hashIndex];

        while (entry != nullptr && entry->key != word) {
            entry = entry->next;
        }

        if (entry == nullptr) {
            return nullptr;
        } else {
            return &entry->bookFrequency;
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
    if (results.empty()) {
        std::cout << "No words found with the prefix \"" << prefix << "\".\n";
    } else {
        std::cout << "Autocomplete results for prefix \"" << prefix << "\":\n";
        for (const std::string& result : results) {
            std::cout << result << "\n";
        }
    }
}

void displayMenu() {
    std::cout << "Choose an option: \n";
    std::cout << "1. Search for a word\n";
    std::cout << "2. Autocomplete\n";
    std::cout << "3. Exit\n";
}

// Function to merge two linked lists and find top 10 combined frequencies using MaxHeap
void findTop10Books(BookFrequencyList* list1, BookFrequencyList* list2) {
    MaxHeap maxHeap;

    BookFrequencyNode* ptr1 = list1->head;
    BookFrequencyNode* ptr2 = list2->head;

    // Use two-pointer technique to merge the two lists
    while (ptr1 && ptr2) {
        if (ptr1->bookID == ptr2->bookID) {
            maxHeap.insert(ptr1->bookID, ptr1->frequency + ptr2->frequency);
            ptr1 = ptr1->next;
            ptr2 = ptr2->next;
        } else if (ptr1->bookID < ptr2->bookID) {
            maxHeap.insert(ptr2->bookID, ptr2->frequency);
            ptr2 = ptr2->next;
        } else {
            maxHeap.insert(ptr1->bookID, ptr1->frequency);
            ptr1 = ptr1->next;
        }
    }

    // Process remaining nodes in both lists
    while (ptr1) {
        maxHeap.insert(ptr1->bookID, ptr1->frequency);
        ptr1 = ptr1->next;
    }

    while (ptr2) {
        maxHeap.insert(ptr2->bookID, ptr2->frequency);
        ptr2 = ptr2->next;
    }

    // Display the top 10 books
    std::cout << "Top 10 books with the highest combined frequency:\n";
    maxHeap.displayTop();
}

// Function to perform AND search for two words
void top10AndSearch(HashTable hashtable, const std::string& word1, const std::string& word2) {
    BookFrequencyList* list1 = hashtable.getBookFrequencyList(word1);
    BookFrequencyList* list2 = hashtable.getBookFrequencyList(word2);

    // Check if both words exist in the hash table
    if (list1->head == nullptr || list2->head == nullptr) {
        std::cout << "One or both words not found in the hash table.\n";
        return;
    }

    // Find the top 10 books with the highest combined frequency
    findTop10Books(list1, list2);
}

void printWithColor(const std::string& text, const std::string& colorCode) {
    std::cout << "\033[" << colorCode << "m" << text << "\033[0m";
}

// Function to display cool ASCII art
void displayCoolIntro() {
    // Adding some ASCII art for the search engine title
    std::string title = R"(
          _    _____       _ _       _       ____                                       _____                     _       ______             _            
         ( )  / ____|     | | |     ( )     |  _ \                                     / ____|                   | |     |  ____|           (_)           
   ___   |/  | (___  _   _| | |_   _|/ ___  | |_) | __ _ _ __   __ _ _ __   __ _ ___  | (___   ___  __ _ _ __ ___| |__   | |__   _ __   __ _ _ _ __   ___ 
  / _ \       \___ \| | | | | | | | | / __| |  _ < / _` | '_ \ / _` | '_ \ / _` / __|  \___ \ / _ \/ _` | '__/ __| '_ \  |  __| | '_ \ / _` | | '_ \ / _ \
 | (_) |      ____) | |_| | | | |_| | \__ \ | |_) | (_| | | | | (_| | | | | (_| \__ \  ____) |  __/ (_| | | | (__| | | | | |____| | | | (_| | | | | |  __/
  \___/      |_____/ \__,_|_|_|\__, | |___/ |____/ \__,_|_| |_|\__,_|_| |_|\__,_|___/ |_____/ \___|\__,_|_|  \___|_| |_| |______|_| |_|\__, |_|_| |_|\___|
                                __/ |                                                                                                   __/ |             
                               |___/                                                                                                   |___/                 
)";
    
    // Display ASCII art with a color
    printWithColor(title, "1;36");  // Cyan color

    // Add a tagline for the search engine
    std::string tagline = "\n\nWelcome to the Ultimate Search Engine!\n";
    printWithColor(tagline, "1;32");  // Green color

    std::string instructions = "Find words, autocomplete your queries, and explore the world of books.\n\n";
    printWithColor(instructions, "1;37");  // White color

    sleep(2);  // Pause for 2 seconds for a dramatic effect

    std::string startMessage = "Let's get started...\n\n";
    printWithColor(startMessage, "1;33");  // Yellow color

    sleep(1);  // Pause for 1 second
}

int main() {
    Trie trie;
    HashTable hashtable;

    // Display the cool intro with ASCII art
    displayCoolIntro();

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
    std:: string input;

    while (true) {
        displayMenu();
        std::cin >> choice;

        switch (choice) {
            case 1:
                std::cout << "Enter a word to search: ";
                std::cin.ignore(); // To discard any leftover newline characters
                std::getline(std::cin, input);

                // Check if the input contains "AND"
                if (input.find("AND") != std::string::npos) {
                    std::istringstream iss(input);
                    std::string word1, andKeyword, word2;
                    iss >> word1 >> andKeyword >> word2;

                    // Perform search for both words
                    std::cout << "Searching for: " << word1 << " and " << word2 << std::endl;
                    top10AndSearch(hashtable, word1, word2);
                } else {
                    // Perform a normal single-word search
                    hashtable.search(input);
                }
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

    return 0;
}