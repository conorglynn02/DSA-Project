#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <regex>
#include "include/nlohmann/json.hpp"

#include <vector>

using json = nlohmann::json;
using namespace std;

// Trie Node definition
class TrieNode {
public:
    unordered_map<char, TrieNode*> children;
    bool isEndOfWord;

    TrieNode() : isEndOfWord(false) {}
};

// Trie definition
class Trie {
private:
    TrieNode* root;

public:
    Trie() {
        root = new TrieNode();
    }

    // Insert a word into the Trie
    void insert(const string& word) {
        TrieNode* node = root;
        for (char ch : word) {
            if (!node->children.count(ch)) {
                node->children[ch] = new TrieNode();
            }
            node = node->children[ch];
        }
        node->isEndOfWord = true;
    }

    // Get all words that start with a given prefix
    void searchWithPrefix(TrieNode* node, const string& prefix, vector<string>& results) {
        if (node->isEndOfWord) {
            results.push_back(prefix);
        }
        for (const auto& child : node->children) {
            searchWithPrefix(child.second, prefix + child.first, results);
        }
    }

    vector<string> autocomplete(const string& prefix) {
        TrieNode* node = root;
        vector<string> results;

        // Traverse the Trie to find the node corresponding to the prefix
        for (char ch : prefix) {
            // If the prefix does not exist in the Trie, return an empty vector
            if (!node->children.count(ch)) {
                return results;
            }
            // Move to the next node
            node = node->children[ch];
        }

        // Collect all words starting from this node
        searchWithPrefix(node, prefix, results);
        return results;
    }
};

// Function to tokenize text using regex
unordered_set<string> tokenize(const string& text) {
    unordered_set<string> tokens;
    regex word_regex("\\b\\w+\\b");
    auto words_begin = sregex_iterator(text.begin(), text.end(), word_regex);
    auto words_end = sregex_iterator();
    
    for (auto it = words_begin; it != words_end; ++it) {
        tokens.insert(it->str());
    }
    return tokens;
}

// Function to build the index
unordered_map<string, unordered_set<string>> buildIndex(const unordered_map<string, string>& documents, Trie& trie) {
    unordered_map<string, unordered_set<string>> index;
    
    for (const auto& doc : documents) {
        const string& doc_id = doc.first;
        const string& doc_path = doc.second;
        
        ifstream file(doc_path);
        if (file.is_open()) {
            stringstream buffer;
            buffer << file.rdbuf();
            string content = buffer.str();
            unordered_set<string> tokens = tokenize(content);
            
            for (const string& token : tokens) {
                index[token].insert(doc_id);
                trie.insert(token);
            }
            file.close();
        } else {
            cerr << "Failed to open file: " << doc_path << endl;
        }
    }
    return index;
}

// Function to save the index to a JSON file
void saveIndex(const unordered_map<string, unordered_set<string>>& index, const string& filePath) {
    json jsonIndex;
    for (const auto& entry : index) {
        jsonIndex[entry.first] = entry.second;
    }
    
    ofstream outFile(filePath);
    if (outFile.is_open()) {
        outFile << jsonIndex.dump(4);
        outFile.close();
    } else {
        cerr << "Failed to write to file: " << filePath << endl;
    }
}

int main() {
    // Map of document IDs to file paths
    unordered_map<string, string> documents = {
        {"doc1", "./document1.txt"},
        {"doc2", "./document2.txt"},
    };

    // Create a Trie for autocomplete
    Trie trie;

    // Build the index
    unordered_map<string, unordered_set<string>> index = buildIndex(documents, trie);
    
    // Save the index to a JSON file
    saveIndex(index, "index.json");
    
    cout << "Indexing complete. Saved to index.json." << endl;

    // Autocomplete
    string prefix;
    cout << "Enter prefix for autocomplete: ";
    cin >> prefix;
    auto suggestions = trie.autocomplete(prefix);
    cout << "Suggestions: ";
    for (const auto& suggestion : suggestions) {
        cout << suggestion << " ";
    }
    cout << endl;

    return 0;
}
