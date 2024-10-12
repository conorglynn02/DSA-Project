#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <sstream> 

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

    // Deserialize the trie from the file
    std::ifstream inFile("trie_data.txt");
    if (!inFile) {
        std::cerr << "Failed to open trie data file.\n";
        return 1;
    }
    trie.deserialize(inFile);
    inFile.close();

    // Perform autocompletion search
    autoCompleteSearch(trie);

    return 0;
}