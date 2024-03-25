#ifndef POINT_H
#define POINT_H

#include "TSP.h"




class Point {
public:
    Point() {}
    //Point has a letter name, like 'A', goes all the way up to Z hypothetically
    Point(char name, const glm::vec2& origPosition) : name(name), origPosition(origPosition) {
        position = origPosition;
    }

    char getName() const { return name; }
    glm::vec2 getPosition() const { return position; }
    glm::vec2 getOrigPosition() const { return origPosition; }

    void normalizePosition() {
        position.x /= SCRWIDTH;
        position.y /= SCRHEIGHT;
    }


private:
    char name;
    glm::vec2 position;
    glm::vec2 origPosition;
 
};

Point FindInPointArr(char name, const Point* points, int numPoints) {
    //finds and returns a point given its name
    for (int i = 0; i < numPoints; ++i) {
        if (points[i].getName() == name) {
            return points[i];
        }
    }
    return Point('\0', glm::vec2(0.0f, 0.0f));
}

int loadPointNames(Point points[], std::string path){
    //Loads the points as Point classes
    std::ifstream fin;
    fin.open(path);
    if(!fin){
        std::cerr << "ERROR: INVALID PATH" << std::endl;
        return -1;
    }
    int index = 0;
    std::string line;

    while(getline(fin, line)){
        std::stringstream ss(line);
        std::string tmp;

        char name;
        float xpos;
        float ypos;

        getline(ss, tmp, ',');
        name = tmp[0];
        getline(ss, tmp, ',');
        xpos =  std::stof(tmp);
        getline(ss, tmp, ',');
        ypos =  std::stof(tmp);

        points[index] = Point(name, glm::vec2(xpos, ypos));
        index++;
    
    }
    return 0;
    fin.close();
}

#endif