#pragma once

#include <tuple>


char **random_prim_algorithm(int n) {
    srand(time(NULL));
    char** maze = new char*[n];

    // Allocate memory for each string and copy the content
    for (int i = 0; i < n; ++i) {
        maze[i] = new char[n];
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            maze[i][j] = 'x';

            if (i == 0 || j == 0 || j == n-1 || i == n-1)
                maze[i][j] = 'b'; // mark boundaries
        }
    }

    std::vector<std::tuple<int, int>> walls_list;

    // do not get borders
    int cell_i = rand() % (n - 1) + 1;
    int cell_j = rand() % (n - 1) + 1;

    maze[cell_i][cell_j] = '.';

    if (maze[cell_i + 1][cell_j] == 'x')
        walls_list.push_back(std::make_tuple(cell_i + 1, cell_j));
    if (maze[cell_i - 1][cell_j] == 'x')
        walls_list.push_back(std::make_tuple(cell_i - 1, cell_j));
    if (maze[cell_i][cell_j + 1] == 'x')
        walls_list.push_back(std::make_tuple(cell_i, cell_j + 1));
    if (maze[cell_i][cell_j - 1] == 'x')
        walls_list.push_back(std::make_tuple(cell_i, cell_j - 1));

    while (walls_list.size()) {

        int random_wall = rand() % walls_list.size();
        std::tuple aux = walls_list[walls_list.size() - 1];
        walls_list[walls_list.size() - 1] = walls_list[random_wall];
        walls_list[random_wall] = aux;

        int cell_i, cell_j;
        std::tie(cell_i, cell_j) = walls_list[walls_list.size() - 1];
        walls_list.pop_back();

        int num_visited_neighboors = 0;
        if (maze[cell_i + 1][cell_j] == '.')
            num_visited_neighboors++;
        if (maze[cell_i - 1][cell_j] == '.')
            num_visited_neighboors++;
        if (maze[cell_i][cell_j + 1] == '.')
            num_visited_neighboors++;
        if (maze[cell_i][cell_j - 1] == '.')
            num_visited_neighboors++;

        if (num_visited_neighboors == 1) {

            maze[cell_i][cell_j] = '.';

            if (maze[cell_i + 1][cell_j] == 'x')
                walls_list.push_back(std::make_tuple(cell_i + 1, cell_j));
            if (maze[cell_i - 1][cell_j] == 'x')
                walls_list.push_back(std::make_tuple(cell_i - 1, cell_j));
            if (maze[cell_i][cell_j + 1] == 'x')
                walls_list.push_back(std::make_tuple(cell_i, cell_j + 1));
            if (maze[cell_i][cell_j - 1] == 'x')
                walls_list.push_back(std::make_tuple(cell_i, cell_j - 1));
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (maze[i][j] == 'b') {
                maze[i][j] = 'x';
            }
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            std::cout << maze[i][j];
        }
        std::cout << std::endl;
    }


    return maze;
}

std::tuple<char **, int, int> get_maze() {

    int n = 21;

    if (n % 2 == 0)
        n++;

    char** mazeOnHeap = random_prim_algorithm(n);

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (mazeOnHeap[i][j] == '.') {
                mazeOnHeap[i][j] = 'c';
                break;
            }


    return std::make_tuple(mazeOnHeap, n, n);
}