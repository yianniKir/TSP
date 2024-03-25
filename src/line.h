#ifndef LINE_H
#define LINE_H

#include "TSP.h"
#include "point.h"

class Line {
public:
    //Changes the color in the drawing stage
    glm::vec3 color;

    Line() {}

    //Line has a name, which is the sum of start and end point name, along with the actual points and a color
    Line(std::string name, const Point& start, const Point& end, glm::vec3 color) : name(name), start(start), end(end), color(color){}

    Point getStart() const { return start; }
    Point getEnd() const { return end; }
    std::string getName() const { return name; }
    
    //This debug is super long because it was annoying to write, so I kept it
    void debug(){
        std::cout << getName() << ": (" << getStart().getPosition().x
        << ','
        << getStart().getPosition().y
        << ')'
        << "->"
        << '(' <<getEnd().getPosition().x
        << ','
        << getEnd().getPosition().y
        << ')'
        <<"\n";
    }
private:
    Point start;
    Point end;
    std::string name;
};

void setLineColor(std::string lineName, glm::vec3 color, const int numLines, Line lines[]) {
    //Changes a lines color vector, which changes the output in the drawing stage
    for(int i = 0; i < numLines; i++) {
        std::string linesLineName(lines[i].getName());

        if(compareStrings(lineName, linesLineName)) {
            lines[i].color = color;
            return;
        }
    }
}

int loadLineNames(std::string lineNames[], std::string path){
    //Loads the names of lines
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

        getline(ss, tmp, ',');
        lineNames[index] = tmp;
        index++;
    }
    return 0;
    fin.close();
}

#endif