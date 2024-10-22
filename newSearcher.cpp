#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <sstream> 
#include <unistd.h>
#include <limits>

// Map to store Book ID -> Book Name
std::unordered_map<int, std::string> bookIdToNameMap;

// Function to print text with color
void printWithColor(const std::string& text, const std::string& colorCode) {
    std::cout << "\033[" << colorCode << "m" << text << "\033[0m";
}

void displayBookWithId(int bookID, int frequency) {
    if (bookIdToNameMap.find(bookID) != bookIdToNameMap.end()) {
        std::cout << "Book ID: " << bookID << ", Book Name: " << bookIdToNameMap[bookID] << ", Frequency: " << frequency << std::endl;
    } else {
        std::cout << "Book ID: " << bookID << " (Book name not found), Frequency: " << frequency << std::endl;
    }
}

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
    std::unordered_map<int, int> bookIDToIndex; // Map from bookID to index in the heap

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
            // Update index map after swap
            bookIDToIndex[heap[idx].first] = idx;
            bookIDToIndex[heap[largest].first] = largest;
            heapifyDown(largest);
        }
    }

    void heapifyUp(int idx) {
        while (idx > 0) {
            int parent = (idx - 1) / 2;
            if (heap[idx].second > heap[parent].second) {
                // remove the old index from the map
                if (bookIDToIndex.find(heap[idx].first) != bookIDToIndex.end()) {
                    bookIDToIndex.erase(heap[idx].first);
                }
                std::swap(heap[idx], heap[parent]);

                // update the new index in the map
                bookIDToIndex[heap[idx].first] = idx;
                // update the index in the heap
                bookIDToIndex[heap[parent].first] = parent;
                idx = parent;
            } else {
                break;
            }
        }
    }

public:
    public:
    std::unordered_map<int, std::pair<std::pair<bool, bool>, int>> seen; // Map to keep track the seen books and their frequency
    
    void insertOrUpdate(int bookID, int frequency) {
        if (bookIDToIndex.find(bookID) != bookIDToIndex.end()) {
            // BookID already in the heap, update its frequency
            int idx = bookIDToIndex[bookID];
            heap[idx].second += frequency;

            // Depending on whether the frequency increased or decreased, heapify accordingly
            heapifyUp(idx);
            // heapifyDown(idx);
        } else {
            // BookID not in the heap, insert it as a new entry
            heap.push_back({bookID, frequency});
            int idx = heap.size() - 1;
            bookIDToIndex[bookID] = idx;
            heapifyUp(idx);
        }
    }

    std::pair<int, int> extractMax() {
        if (heap.empty()) return {-1, -1};  // Edge case for empty heap
        std::pair<int, int> maxValue = heap[0];
        bookIDToIndex.erase(maxValue.first); // Remove from map
        heap[0] = heap.back(); // Replace the root with the last element
        heap.pop_back(); // Remove the last element
        if (!heap.empty()) {
            bookIDToIndex[heap[0].first] = 0; // Update root index in map
            heapifyDown(0); // Heapify down the new root
        }
        return maxValue;
    }

    // Function to display the list of X to Y books
    bool displayXToYBooks(int start, int end) {
        while (heap.size() != 0 && start < end) {
            std::pair<int, int> current = extractMax();
            if (current.first == -1) {
                return true;
            }
            std::cout << start+1 << "-> " << "Book ID: " << current.first << ", Combined Frequency: " << current.second << std::endl;
            start++;
        }

        return heap.empty();
    }

    void displayTop() {
        if (heap.size() == 0) {
            printWithColor("No matches found. ", "1;31");
            std::cout << std::endl;
            return;
        }

        printWithColor("Matches found: ", "1;33");
        std::cout << std::endl;
        int start = 0;
        int end = 10;
        bool isEnd = displayXToYBooks(start, end);
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
            isEnd = displayXToYBooks(start, end);
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
            displayBookWithId(current->bookID, current->frequency); // Pass both book ID and frequency
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

// Function to perform autocompletion using the pre-built trie
void autoCompleteSearch(Trie& trie) {
    std::string prefix;
    std::cin >> prefix;
    std::vector<std::string> results = trie.autoComplete(prefix);
    if (results.empty()) {
        printWithColor("No words found with the prefix - ", "1;31");
        std::cout << prefix << "\n";
    } else {
        printWithColor("Autocomplete results for prefix - ", "1;33");
        std::cout << prefix << ":\n";
        for (const std::string& result : results) {
            std::cout << result << "\n";
        }
    }
}

// Function to merge two linked lists and find top 10 combined frequencies using MaxHeap
void findTop10AndBooks(BookFrequencyList* list1, BookFrequencyList* list2) {
    MaxHeap maxHeap;

    BookFrequencyNode* ptr1 = list1->head;
    BookFrequencyNode* ptr2 = list2->head;

    // Use two-pointer technique to merge the two lists
    while (ptr1 && ptr2) {
        if (ptr1->bookID == ptr2->bookID) {
            maxHeap.seen[ptr1->bookID].first.first = true;
            maxHeap.seen[ptr1->bookID].first.second = true;
            maxHeap.insertOrUpdate(ptr1->bookID, ptr1->frequency + ptr2->frequency);
            ptr1 = ptr1->next;
            ptr2 = ptr2->next;
        } else if (ptr1->bookID < ptr2->bookID) {
            maxHeap.seen[ptr2->bookID].first.second = true;
            if (maxHeap.seen[ptr2->bookID].first.first) {
                maxHeap.insertOrUpdate(ptr2->bookID, (ptr2->frequency + maxHeap.seen[ptr2->bookID].second));
            }
            maxHeap.seen[ptr2->bookID].second = ptr2->frequency;
            ptr2 = ptr2->next;
        } else {
            maxHeap.seen[ptr1->bookID].first.first = true;
            if (maxHeap.seen[ptr1->bookID].first.second) {
                maxHeap.insertOrUpdate(ptr1->bookID, (ptr1->frequency + maxHeap.seen[ptr1->bookID].second));
            }
            maxHeap.seen[ptr1->bookID].second = ptr1->frequency;
            ptr1 = ptr1->next;
        }
    }

    // Process remaining nodes in both lists
    while (ptr1) {
        maxHeap.seen[ptr1->bookID].first.first = true;
        if (maxHeap.seen[ptr1->bookID].first.second) {
            maxHeap.insertOrUpdate(ptr1->bookID, (ptr1->frequency + maxHeap.seen[ptr1->bookID].second));
        }
        maxHeap.seen[ptr1->bookID].second = ptr1->frequency;
        ptr1 = ptr1->next;
    }

    while (ptr2) {
        maxHeap.seen[ptr2->bookID].first.second = true;
        if (maxHeap.seen[ptr2->bookID].first.first) {
            maxHeap.insertOrUpdate(ptr2->bookID, (ptr2->frequency + maxHeap.seen[ptr2->bookID].second));
        }
        maxHeap.seen[ptr2->bookID].second = ptr2->frequency;
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
    if (list1 == nullptr || list2 == nullptr) {
        std::cout << "One or both words not found in the hash table.\n";
        return;
    }

    // Find the top 10 books with the highest combined frequency
    findTop10AndBooks(list1, list2);
}

// Deserialize the book ID -> book name map from a file
void deserializeBookIdToNameMap(std::ifstream& inFile) {
    int bookID;
    std::string bookName;
    while (inFile >> bookID) {
        std::getline(inFile, bookName);
        bookIdToNameMap[bookID] = bookName;
    }
}

void displayMenu() {
    std::string title = "\n\nChoose an Option:\n";
    printWithColor(title, "1;32");
    std::string one = "1:";
    printWithColor(one, "1;33");
    std::cout << " Search for a word\n";
    std::string two = "2:";
    printWithColor(two, "1;34");
    std::cout << " Autocomplete\n";
    std::string three = "3:";
    printWithColor(three, "1;31");
    std::cout << " Exit\n";
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

    std::string startMessage = "Let's get started...";
    printWithColor(startMessage, "1;33");  // Yellow color

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

    // Deserialize the book ID -> book name map
    std::ifstream inFileBookIdToName("book_id_to_name_map.txt");
    if (!inFileBookIdToName) {
        std::cerr << "Failed to open book ID to name map file.\n";
        return 1;
    }
    deserializeBookIdToNameMap(inFileBookIdToName);
    inFileBookIdToName.close();

    int choice;
    std:: string input;

    while (true) {
        displayMenu();
        std::cin >> choice;

        // Check if the input is valid

        if (std::cin.fail()) {
            std::cin.clear(); // Clear the error flag on cin
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
            printWithColor("Invalid choice. Please enter a number (1, 2, or 3).\n", "1;33");
            continue; // Re-display the menu and prompt the user for input again
        }

        switch (choice) {
            case 1:
                printWithColor("Enter a word to search: ", "1;32");
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
                printWithColor("Enter search prefix: ", "1;32");
                autoCompleteSearch(trie);
                break;
            case 3:
                printWithColor("Exiting Program...\n", "1;31");
                return 0;
            default:
                printWithColor("Invalid choice. Please enter a number (1, 2, or 3).\n", "1;33");
                break;
        }
    }

    return 0;
}