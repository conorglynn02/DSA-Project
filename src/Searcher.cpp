#include <limits>
#include "Trie.h"
#include "SearchHashTable.h"
#include "MaxHeap.h"
#include "VictorVector.h"

// Function to perform autocompletion using the pre-built trie
void autoCompleteSearch(Trie& trie) {
    std::string prefix;
    std::cin >> prefix;
    VictorVector<std::string> results = trie.autoComplete(prefix);
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

// Function to merge two linked lists for AND and find top combined frequencies using MaxHeap
void findTopAndBooks(BookFrequencyList* list1, BookFrequencyList* list2) {
    std::string type = "and";
    
    MaxHeap maxHeap;

    BookFrequencyNode* ptr1 = list1->head;
    BookFrequencyNode* ptr2 = list2->head;

    // Two-pointer to insert into heap
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

    // Display the top books
    std::cout << "Top books with the highest combined frequency:\n";
    maxHeap.displayTop(type);
}

// Function to perform AND search for two words
void topAndSearch(HashTable hashtable, const std::string& word1, const std::string& word2) {
    BookFrequencyList* list1 = hashtable.getBookFrequencyList(word1);
    BookFrequencyList* list2 = hashtable.getBookFrequencyList(word2);

    // Check if both words exist in the hash table
    if (list1 == nullptr || list2 == nullptr) {
        std::cout << "One or both words not found in the hash table.\n";
        return;
    }

    // Find the top books with the highest combined frequency
    findTopAndBooks(list1, list2);
}

// Function to merge two linked lists for AND and find top combined frequencies using MaxHeap
void findTopOrBooks(BookFrequencyList* list1, BookFrequencyList* list2) {
    std::string type = "or";
    
    MaxHeap maxHeap;

    BookFrequencyNode* ptr1 = list1->head;
    BookFrequencyNode* ptr2 = list2->head;

    // Two-pointer to insert into heap
    while (ptr1 && ptr2) {
        if (ptr1->bookID == ptr2->bookID) {
            maxHeap.insertOrUpdate(ptr1->bookID, (ptr1->frequency + ptr2->frequency));
            ptr1 = ptr1->next;
            ptr2 = ptr2->next;
        } else if (ptr1->bookID < ptr2->bookID) {
            maxHeap.insertOrUpdate(ptr2->bookID, ptr2->frequency);
            ptr2 = ptr2->next;
        } else {
            maxHeap.insertOrUpdate(ptr1->bookID, ptr1->frequency);
            ptr1 = ptr1->next;
        }
    }

    // Process remaining nodes in both lists
    while (ptr1) {
        maxHeap.insertOrUpdate(ptr1->bookID, ptr1->frequency);
        ptr1 = ptr1->next;
    }

    while (ptr2) {
        maxHeap.insertOrUpdate(ptr2->bookID, ptr2->frequency);
        ptr2 = ptr2->next;
    }

    // Display the top books
    std::cout << "Top books with the highest combined frequency:\n";
    maxHeap.displayTop(type);
}

// Function to perform OR search for two words
void topOrSearch(HashTable hashtable, const std::string& word1, const std::string& word2) {
    BookFrequencyList* list1 = hashtable.getBookFrequencyList(word1);
    BookFrequencyList* list2 = hashtable.getBookFrequencyList(word2);

    // Check if both words exist in the hash table
    if (list1 == nullptr || list2 == nullptr) {
        std::cout << "One or both words not found in the hash table.\n";
        return;
    }

    // Find the top books with the highest combined frequency
    findTopOrBooks(list1, list2);
}

// Function to merge two linked lists for AND and find top combined frequencies using MaxHeap
void findTopNotBooks(BookFrequencyList* list1, BookFrequencyList* list2, const std::string& word1, const std::string& word2) {
    std::string type = "not";
    
    MaxHeap maxHeap;

    BookFrequencyNode* ptr1 = list1->head;
    BookFrequencyNode* ptr2 = list2->head;

    // Make blacklist of books that have word2
    while (ptr2) {
        maxHeap.bookNotMap[ptr2->bookID] = true;
        ptr2 = ptr2->next;
    }
    
    // Two-pointer to insert into heap
    while (ptr1) {
        if (maxHeap.bookNotMap.find(ptr1->bookID) != maxHeap.bookNotMap.end()){
            ptr1 = ptr1->next;
        } else {
            maxHeap.insertOrUpdate(ptr1->bookID, ptr1->frequency);
            ptr1 = ptr1->next;
        }
    }

    // Display the top books
    std::cout << "Top books with " << word1 << " and without " << word2 << " are:\n";
    maxHeap.displayTop(type);
}

// Function to perform NOT search for two words
void topNotSearch(HashTable hashtable, const std::string& word1, const std::string& word2) {
    BookFrequencyList* list1 = hashtable.getBookFrequencyList(word1);
    BookFrequencyList* list2 = hashtable.getBookFrequencyList(word2);

    // Check if first word exists
    if (list1 == nullptr) {
        std::cout << word1 << " not found in the hash table.\n";
        return;
    }

    // Check if second word exists
    if (list2 == nullptr) {
        hashtable.search(word1);
        return;
    }

    // Find the top books with the highest combined frequency
    findTopNotBooks(list1, list2, word1, word2);
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

                // if the input has "AND"
                if (input.find("AND") != std::string::npos) {
                    std::istringstream iss(input);
                    std::string word1, andKeyword, word2;
                    iss >> word1 >> andKeyword >> word2;

                    // Perform search for both words
                    std::cout << "Searching for: " << word1 << " and " << word2 << std::endl;
                    topAndSearch(hashtable, word1, word2);
                }
                // if the input has "NOT"
                else if (input.find("NOT") != std::string::npos) {
                    std::istringstream iss(input);
                    std::string word1, notKeyword, word2;
                    iss >> word1 >> notKeyword >> word2;

                    // Perform search for both words
                    std::cout << "Searching for: " << word1 << " not " << word2 << std::endl;
                    topNotSearch(hashtable, word1, word2);
                }
                // if the input has "OR"
                else if (input.find("OR") != std::string::npos) {
                    std::istringstream iss(input);
                    std::string word1, orKeyword, word2;
                    iss >> word1 >> orKeyword >> word2;

                    // Perform search for both words
                    std::cout << "Searching for: " << word1 << " or " << word2 << std::endl;
                    topOrSearch(hashtable, word1, word2);
                }
                else {
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