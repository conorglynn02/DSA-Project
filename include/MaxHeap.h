#ifndef MAXHEAP_H
#define MAXHEAP_H
#include "SearchHashTable.h"
#include <vector>
#include <unordered_map>
#include <iostream>

// Map to store Book ID -> Book Name, declared as extern
extern std::unordered_map<int, std::string> bookIdToNameMap;

// Function to print text with color
extern void printWithColor(const std::string& text, const std::string& colorCode) {
    std::cout << "\033[" << colorCode << "m" << text << "\033[0m";
}

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
    std::unordered_map<int, std::pair<std::pair<bool, bool>, int>> seen; // Map to keep track of the seen books and their frequency
    std::unordered_map<int, bool> bookNotMap; // Map to keep track of the books that shouldn't appear, not search
    
    void insertOrUpdate(int bookID, int frequency) {
        if (bookIDToIndex.find(bookID) != bookIDToIndex.end()) {
            // BookID already in the heap, update its frequency
            int idx = bookIDToIndex[bookID];
            heap[idx].second += frequency;

            // move this book up the heap if needed
            heapifyUp(idx);
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
    bool displayXToYBooks(int start, int end, const std::string& type) {
        while (heap.size() != 0 && start < end) {
            std::pair<int, int> current = extractMax();
            if (current.first == -1) {
                return true;
            }
            //check if book ID exists in map
            if (bookIdToNameMap.find(current.first) !=bookIdToNameMap.end()) {
                if (type == "and" || type == "or") {
                std::cout << start+1 << " -> " << "Book ID: " << current.first << ", Book Name: " << bookIdToNameMap[current.first] << ", Combined Frequency: " << current.second << std::endl;
                }
                else if (type == "not") {
                std::cout << start+1 << " -> " << "Book ID: " << current.first << ", Book Name: " << bookIdToNameMap[current.first] << ", Frequency: " << current.second << std::endl;
                }
            }
            start++;
        }

        return heap.empty();
    }

    void displayTop(const std::string& type) {
        if (heap.size() == 0) {
            printWithColor("No book matches found. ", "1;31");
            std::cout << std::endl;
            return;
        }

        printWithColor("Book matches found: ", "1;33");
        std::cout << std::endl;
        int start = 0;
        int end = 10;
        bool isEnd = displayXToYBooks(start, end, type);
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
            isEnd = displayXToYBooks(start, end, type);
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

#endif