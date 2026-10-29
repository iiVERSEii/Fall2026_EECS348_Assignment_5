/*
 Program: EECS 348 Assignment 5 -  Improved Sudoku Solver
 Description: Read five 9x9 Sudoku puzzles and find every solution using
              an object-oriented recursive depth-first backtracking search,
              writing results to solutionN.txt files
 Author: Kaden Shepherd
 Created: October 29, 2026
 Collaborators: Shimrah Meraj
 Sources: CoPilot and Google Gemini
*/

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class SudokuSolver {
private:
    int grid[9][9]{};    // Zero means the square is empty.
    int solutionCount = 0; // Count every solution found for this puzzle.

    // Check whether a number can be placed at the requested position.
    bool allowed(int row, int col, int value) const {
        for (int i = 0; i < 9; ++i) {
            if (grid[row][i] == value || grid[i][col] == value)
                return false;
        }
        int boxRow = (row / 3) * 3;
        int boxCol = (col / 3) * 3;
        for (int r = boxRow; r < boxRow + 3; ++r) {
            for (int c = boxCol; c < boxCol + 3; ++c) {
                if (grid[r][c] == value)
                    return false;
            }
        }
        return true;
    }

    // Improved search: pick the empty square with the fewest candidates.
    bool findBestEmpty(int& bestRow, int& bestCol) const {
        int fewest = 10;
        bool found = false;
        for (int row = 0; row < 9; ++row) {
            for (int col = 0; col < 9; ++col) {
                if (grid[row][col] != 0)
                    continue;
                found = true;
                int choices = 0;
                for (int value = 1; value <= 9; ++value) {
                    if (allowed(row, col, value))
                        ++choices;
                }
                if (choices < fewest) {
                    fewest = choices;
                    bestRow = row;
                    bestCol = col;
                }
                if (choices == 0)
                    return true;
            }
        }
        return found;
    }

    // Print the Sudoku grid to the specified output stream in the input format.
    void printGrid(ostream& out) const {
        for (int row = 0; row < 9; ++row) {
            for (int col = 0; col < 9; ++col) {
                char shown = grid[row][col] == 0 ? '_' : char('0' + grid[row][col]);
                out << shown << (col == 8 ? '\n' : ' ');
            }
        }
    }

    // Recursive backtracking solver writing directly to the output stream.
    void solveRecursive(ostream& out) {
        int row = -1, col = -1;
        if (!findBestEmpty(row, col)) {
            ++solutionCount;
            printGrid(out); // Print solution grid in standard format
            return;
        }
        for (int value = 1; value <= 9; ++value) {
            if (allowed(row, col, value)) {
                grid[row][col] = value;
                solveRecursive(out);
                grid[row][col] = 0;
            }
        }
    }

public:
    // Load exactly 81 squares; reject invalid characters or extra values.
    bool load(const string& fileName) {
        ifstream file(fileName);
        if (!file)
            return false;
        for (int row = 0; row < 9; ++row) {
            for (int col = 0; col < 9; ++col) {
                char ch;
                if (!(file >> ch) || (ch != '_' && (ch < '1' || ch > '9')))
                    return false;
                grid[row][col] = ch == '_' ? 0 : ch - '0';
            }
        }
        char extra;
        if (file >> extra)
            return false;
        for (int row = 0; row < 9; ++row) {
            for (int col = 0; col < 9; ++col) {
                if (grid[row][col] == 0)
                    continue;
                int value = grid[row][col];
                grid[row][col] = 0;
                bool valid = allowed(row, col, value);
                grid[row][col] = value;
                if (!valid)
                    return false;
            }
        }
        solutionCount = 0;
        return true;
    }

    // Handle one puzzle from file loading through solving and writing to solutionN.txt.
bool run(const string& inputFileName, const string& outputFileName) {
    ofstream outFile(outputFileName);
    if (!outFile) { // Check if the output file could be opened
        cerr << "Could not open output file: " << outputFileName << '\n';
        return false;
    }

    if (!load(inputFileName)) {
        outFile << "Invalid puzzle input.\n";
    } else { // Puzzle loaded successfully, proceed to solve it
        solveRecursive(outFile);

        if (solutionCount == 0) {
            outFile << "No solution found.\n";
        } else {
            outFile << "Solutions found: " << solutionCount << '\n';
        }
    }

    outFile.close();
    if (!outFile) { // Check if the output file could be written
        cerr << "Could not write output file: " << outputFileName << '\n';
        return false;
    }
    return true;
    }
};
// Process the five input files using separate solver objects.
int main() {
    for (int i = 1; i <= 5; ++i) {
        SudokuSolver solver;
        string inputFile = "puzzle" + to_string(i) + ".txt";
        string outputFile = "solution" + to_string(i) + ".txt";
        solver.run(inputFile, outputFile);
    }
    return 0;
}