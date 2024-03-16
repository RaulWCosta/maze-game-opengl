#pragma once

#include <tuple>
#include <cstdlib>
#include <algorithm>
#include <cmath>

bool is_wall_valid(char** maze, int i, int j) {

    int num_visited_neighboors = 0;
    if (maze[i + 1][j] == '.')
        num_visited_neighboors++;
    if (maze[i - 1][j] == '.')
        num_visited_neighboors++;
    if (maze[i][j + 1] == '.')
        num_visited_neighboors++;
    if (maze[i][j - 1] == '.')
        num_visited_neighboors++;

    if (num_visited_neighboors != 1) {
        return false;
    }

    unsigned char curr_pos = 0;

    if (maze[i-1][j-1] == '.')
        curr_pos += 1 << 7;
    if (maze[i-1][j] == '.')
        curr_pos += 1 << 6;
    if (maze[i-1][j+1] == '.')
        curr_pos += 1 << 5;
    if (maze[i][j+1] == '.')
        curr_pos += 1 << 4;
    if (maze[i+1][j+1] == '.')
        curr_pos += 1 << 3;
    if (maze[i+1][j] == '.')
        curr_pos += 1 << 2;
    if (maze[i+1][j-1] == '.')
        curr_pos += 1 << 1;
    if (maze[i][j-1] == '.')
        curr_pos += 1;

    curr_pos = curr_pos ^ ((curr_pos >> 1) | (curr_pos << 7));

    int num_bits = 0;
    for (int i = 0; i < 8; i++) {
        if (curr_pos & (unsigned char)(1 << i))
            num_bits += 1;
    }
    std::cout << "curr_pos = "<< (int)curr_pos << std::endl; 
    std::cout << "bits = "<< num_bits << std::endl; 
    return (bool)(num_bits == 2);

}

char **random_prim_algorithm(int n) {
    // srand(time(NULL));
    srand(42);
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

        // add choosen wall on top of stack
        int random_wall = rand() % walls_list.size();
        std::tuple aux = walls_list[walls_list.size() - 1];
        walls_list[walls_list.size() - 1] = walls_list[random_wall];
        walls_list[random_wall] = aux;

        int cell_i, cell_j;
        std::tie(cell_i, cell_j) = walls_list[walls_list.size() - 1];
        walls_list.pop_back();

        if (is_wall_valid(maze, cell_i, cell_j)) {

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

    return maze;
}


void add_init_location(char **maze, int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (maze[i][j] == '.') {
                maze[i][j] = 'c';
                return;
            }
}

std::tuple<int, int> get_indexes_from_position(float x, float z, int maze_size) {
    int i = (int) (z + (int)(maze_size / 2) + 0.5);
    int j = (int) (x + (int)(maze_size / 2) + 0.5);

    return std::make_tuple(i, j);
}

glm::vec3 get_position_from_indexes(int i, int j, int maze_size) {
    float x = (float) (j - (int) (maze_size / 2));
    float z = (float) (i - (int) (maze_size / 2));
    return glm::vec3(x, 0.0f, z);
}

glm::vec3 get_camera_position(char **maze, int maze_size) {

    for (int i = 0; i < maze_size; i++) {
        for (int j = 0; j < maze_size; j++) {
            // std::cout << maze[i][j] << std::endl; 

            if (maze[i][j] == 'c') {
                return get_position_from_indexes(i, j, maze_size);
            }
        }
    }
}

std::vector<glm::vec3> get_positions(char **maze, int maze_size) {

    std::vector<glm::vec3> cubePositions;

    for (int i = 0; i < maze_size; i++) {
        for (int j = 0; j < maze_size; j++) {
            // std::cout << maze[i][j] << std::endl; 

            if (maze[i][j] == 'x') {
                glm::vec3 pos = get_position_from_indexes(i, j, maze_size);
                cubePositions.push_back(pos);

                // int calc_i, calc_j;
                // std::tie(calc_i, calc_j) = get_indexes_from_position(maze_size, pos.x, pos.z);

                // assert(calc_i == i);
                // assert(calc_j == j);
            }
        }
    }

    return cubePositions;
}

std::tuple<char **, int> get_maze() {

    int n = 8;

    if (n % 2 == 0)
        n++;

    char** mazeOnHeap = random_prim_algorithm(n);

    add_init_location(mazeOnHeap, n);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            std::cout << mazeOnHeap[i][j];
        }
        std::cout << std::endl;
    }

    return std::make_tuple(mazeOnHeap, n);
}