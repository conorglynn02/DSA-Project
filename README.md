# DSA-Project

## How to run: 
1. `mkdir build`
2. `cd ./build`
3. `cmake ..`
4. `make`
5. `./indexer`
6. `./searcher`

## How to use:
1. After running `./searcher`, the user is prompted to choose option 1(search), 2(autocomplete) or 3(exit).
2. If user chooses option 1, user can input a single word, and a list of book names and their corresponding file path is returned. The results are ranked in order of frequency.
3. The user also has the option of using `AND`, `OR`, or `NOT` operators in their search. For example, "bean AND gone", "here OR there", "green NOT red".
4. If user chooses option 2, they can input a prefix, and a list of all words beginning with that prefix are returned.
5. If user chooses option 3, the program exits.
6. If user inputs invalid input, they are prompted to input 1,2 or 3 again.