#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "include/nlohmann/json.hpp"
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

unordered_map<string, unordered_set<string>> loadIndex(const string& filePath) {
    unordered_map<string, unordered_set<string>> index;
    ifstream inFile(filePath);
    if (inFile.is_open()) {
        json jsonIndex;
        inFile >> jsonIndex;

        for (auto& element : jsonIndex.items()) {
            string key = element.key();
            unordered_set<string> values = element.value().get<unordered_set<string>>();
            index[key] = values;
        }
        inFile.close();
    } else {
        cerr << "Failed to open file: " << filePath << endl;
    }
    return index;
}

int main(int argc, char const *argv[])
{
    // Create a Trie for autocomplete
    Trie trie;

    // Load the index from a JSON file
    unordered_map<string, unordered_set<string>> index = loadIndex("index.json");

    // cout << index["101"] << endl;

    // Insert words into the Trie
    for (const auto& entry : index) {
        trie.insert(entry.first);
    }

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
