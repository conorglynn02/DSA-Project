#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <regex>

#include "include/nlohmann/json.hpp"
using json = nlohmann::json;

using namespace std;

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
unordered_map<string, unordered_set<string>> buildIndex(const unordered_map<string, string>& documents) {
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
        {"AlicesAdventuresInWonderland", "./Books/AlicesAdventuresInWonderland.txt"},
        {"ARoomWithAView", "./Books/ARoomWithAView.txt"},
        {"Frankenstein", "./Books/Frankenstein.txt"},
        {"LittleWomen", "./Books/LittleWomen.txt"},
        {"Middlemarch", "./Books/Middlemarch.txt"},
        {"MobyDick", "./Books/MobyDick.txt"},
        {"PrideAndPrejudice", "./Books/PrideAndPrejudice.txt"},
        {"RomeoAndJuliet", "./Books/RomeoAndJuliet.txt"},
        {"TheBlueCastle", "./Books/TheBlueCastle.txt"},
        {"TheScarletLetter", "./Books/TheScarletLetter.txt"},
    };

    // Build the index
    unordered_map<string, unordered_set<string>> index = buildIndex(documents);
    
    // Save the index to a JSON file
    saveIndex(index, "index.json");
    
    cout << "Indexing complete. Saved to index.json." << endl;

    return 0;
}
