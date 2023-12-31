#pragma once

#include <tuple>

const char *get_maze() {
    return "xxxxx\n"
           "x...x\n"
           "x...x\n"
           "x...x\n"
           "xxxxx\n\0";
}

std::tuple<unsigned int, unsigned int> get_maze_dimensions(const char *maze) {
    unsigned int width, depth = 0;

    int i = 0;
    while (maze[i] != '\n')    
        i++;
    width = i;

    i = 0;
    while (maze[i] != '\0') {
        if (maze[i] == '\n')
            depth++;    
        i++;
    }

    return std::make_tuple(width, depth);
}