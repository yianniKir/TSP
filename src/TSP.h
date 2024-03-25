#ifndef TSP_H
#define TSP_H

#include "vendors/glad/glad.h"
#include "vendors/GLFW/glfw3.h"
#include "vendors/glm/glm.hpp"
#include "vendors/glm/gtc/matrix_transform.hpp"
#include "vendors/glm/gtc/type_ptr.hpp"

#include <string>

#include <unistd.h>
#include <cmath>

#include <algorithm>
#include <random>
#include <sstream>
#include <iostream>
#include <fstream>

const float PI = 3.14159265359f;
const int SCRWIDTH = 800;
const int SCRHEIGHT = 800;

const int numOfSolutions = 6;

bool compareStrings(const std::string& str1, const std::string& str2) {
    //Compare two strings irrespective of order AE = EA (true)
    std::string sortedStr1 = str1;
    std::string sortedStr2 = str2;
    // Sort characters of both strings
    std::sort(sortedStr1.begin(), sortedStr1.end());
    std::sort(sortedStr2.begin(), sortedStr2.end());

    return sortedStr1 == sortedStr2;
}

const int countLines(const std::string path) {
    //Counts the lines in a file for constants that define array sizes
    std::ifstream fin;
    fin.open(path);
    if (!fin) {
        std::cerr << "ERROR: INVALID PATH" << path << std::endl;
        return -1;
    }

    int count = 0;
    std::string line;
    while (std::getline(fin, line)) {
        ++count;
    }

    fin.close();
    return count;
}

#endif