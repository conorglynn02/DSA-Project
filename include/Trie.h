#ifndef TRIE_H
#define TRIE_H

#include <unordered_map>
#include <vector>
#include <fstream>
#include <sstream>
#include "VictorVector.h"


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
    void autoCompleteHelper(TrieNode* node, VictorVector<std::string>& results) {
        if (node->isEndOfWord) {
            results.push_back(node->word);
        }
        for (auto& child : node->children) {
            autoCompleteHelper(child.second, results);
        }
    }

    // Function to find all words with the given prefix
    VictorVector<std::string> autoComplete(const std::string& prefix) {
        TrieNode* node = root;
        VictorVector<std::string> results;

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

#endif