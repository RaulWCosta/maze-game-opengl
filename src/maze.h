#pragma once

#include <tuple>

std::tuple<const char **, int, int> get_maze() {
    const char* maze[] = {
        "xxxxx",
        "x...x",
        "x.c.x",
        "x...x",
        "xxxxx",
    };

    // Determine the number of strings in the array
    const int numStrings = sizeof(maze) / sizeof(maze[0]);

    // Allocate memory for an array of char pointers on the heap
    const char** mazeOnHeap = new const char*[numStrings];

    // Allocate memory for each string and copy the content
    for (int i = 0; i < numStrings; ++i) {
        mazeOnHeap[i] = new char[strlen(maze[i]) + 1]; // +1 for the null terminator
        strcpy(const_cast<char*>(mazeOnHeap[i]), maze[i]);
    }

    return std::make_tuple(mazeOnHeap, numStrings, numStrings);
}
