#include <iostream>
#include <fstream>
#include <unordered_map>
#include <sstream>
#include <vector>
#include <filesystem> // To iterate over files in the folder
#include <regex>
#include "Trie.h"
#include "IndexHashTable.h"

namespace fs = std::filesystem;

const int TABLE_SIZE = 100;

// Function to clean the book name by removing underscores and the ".txt" extension
std::string cleanBookName(const std::string& filename) {
    std::string cleanedName = filename;
    
    // Remove underscores
    std::replace(cleanedName.begin(), cleanedName.end(), '_', ' ');

    // Remove the ".txt" extension if present
    size_t lastDot = cleanedName.find_last_of('.');
    if (lastDot != std::string::npos && cleanedName.substr(lastDot) == ".txt") {
        cleanedName = cleanedName.substr(0, lastDot);
    }

    return cleanedName;
}

std::string cleanWord(const std::string& word) {
    std::string cleanedWord;
    
    // Remove all non-alphanumeric characters from the word
    for (char ch : word) {
        if (std::isalnum(ch)) {
            cleanedWord += ch;
        }
    }

    return cleanedWord;
}

// Create a map to store Book ID -> Book Name
std::unordered_map<int, std::string> bookIdToNameMap;

// Function to read words from a text file and index them
void indexWordsFromFile(const std::string& filename, HashTable& hashTable, Trie& trie, int bookID) {
    std::ifstream file(filename);
    // Check if the file is open
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    // Store the mapping of book ID to book name
    std::string bookName = cleanBookName(filename.substr(filename.find_last_of("/\\") + 1));  // Extract file name
    bookIdToNameMap[bookID] = bookName;  // Save the book name with the book ID

    std::string word;

    while (file >> word) {
        // Convert the word to lowercase for consistency
        for (char& ch : word) {
            ch = std::tolower(ch);
        }

        // Clean the word by removing non-alphanumeric characters
        word = cleanWord(word);

        // Insert the cleaned word into the hash table and trie if it's not empty
        if (!word.empty()) {
            hashTable.insert(word, bookID, trie);  // Insert cleaned word into trie and hash table
        }
    }

    file.close();
}

// Serialize the bookIdToNameMap and book paths to a file
void serializeBookIdMap(std::ofstream& outFile, const std::unordered_map<int, std::string>& bookIdToPathMap) {
    for (const auto& pair : bookIdToNameMap) {
        // Serialize book ID, book name, and constructed path
        std::string filePath = bookIdToPathMap.at(pair.first);
        outFile << pair.first << " " << pair.second << " " << filePath << "\n";
    }
}

int main() {
    HashTable hashTable;
    Trie trie;

    // Path to the folder containing books
    std::string booksFolder = "../Books";
    
    // Book ID counter (1 for the first book, 2 for the second, etc.)
    int bookID = 1;

    std::unordered_map<int, std::string> bookIdToPathMap;

    // Iterate through the "books" folder to get each file
    for (const auto& entry : fs::directory_iterator(booksFolder)) {
        std::string filename = entry.path().string();
        
        // Store the file path for the current book ID
        bookIdToPathMap[bookID] = filename;

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

    // Serialize the hash table to a file
    std::ofstream hashTableFile("hash_table_data.txt");
    hashTable.serialize(hashTableFile);
    hashTableFile.close();

    // Serialize the book ID -> book name map
    std::ofstream bookIdToNameFile("book_id_to_name_map.txt");
    serializeBookIdMap(bookIdToNameFile, bookIdToPathMap);
    bookIdToNameFile.close();

    std::cout << "Hash table serialized.\n";

    std::cout << "Indexing complete, index and trie serialized.\n";

    return 0;
}
