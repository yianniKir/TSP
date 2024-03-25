#ifndef SOLUTION_H
#define SOLUTION_H

#include "TSP.h"
#include "point.h"


float distBtwPoints(Point &point1, Point &point2);

class Solution{
public:

    int lineNum;
    Solution() {}
    Solution(std::string perm) : perm(perm){
        //Solution only needs a string, which is a permutation of the solution
        permLen = perm.length();
        lineNum = permLen - 1;
    }
    
    float calcFitness(Point points[]){
        //The fitness is the total distance traveled
        float fitness = 0.0f;
        for(int i = 0; i < permLen - 1; i++){
            Point p1 = findPoint(perm[i], points, permLen);
            Point p2 = findPoint(perm[i+1], points, permLen);
        
            fitness += distBtwPoints(p1, p2);            
        }

        return fitness;
    }

    bool isValid(){
        //Simple checksum,solution should have same ascii count as solution A-N where N is the last point name (PERMNUM-1)
        int idealSum = 0;
        int realSum = 0;
        char currentChar = 'A';
        while (currentChar <= 'A' + lineNum) {
            idealSum += currentChar;
            currentChar++; 
        }
        for(int i = 0; i < permLen; i++){
            realSum += perm[i];
        }

        return realSum == idealSum;
    }
    void lineOrder(std::string order[], std::string lineNames[], int numLineNames){
        //Writes to order[] by reference, used to fill order with the lines that should be colored using setLineColor() (irrespective of case)
        for(int i = 0; i < lineNum; i++){
            std::string name = std::string(1, perm[i]) + std::string(1, perm[i+1]);
            
            for(int j = 0; j < numLineNames; j++){
                if (compareStrings(name, lineNames[j])){
                    std::sort(name.begin(), name.end());
                    order[i] = name;
                }
            }
        }
    }

    void genRandomPerm(){
        std::random_device rd;
        std::mt19937 gen(rd());

        std::shuffle(perm.begin(), perm.end(), gen);
    }

    //For debugging
    std::string getPerm(){
        return perm;
    }

private:
    std::string perm;
    uint fitness;
    int permLen;

    //finds and returns a point given its name, specific for permutation
    Point findPoint(char name, Point points[], int permLen){
        for(int i = 0; i < permLen; i++){
            if(points[i].getName() == name){
                return points[i];
            }
        }
        return Point('Z', glm::vec2(100000.0f, 100000.0f));
    }
};

float distBtwPoints(Point &point1, Point &point2){
    //get distance between points in a more understandable way
    return glm::distance(point1.getOrigPosition(), point2.getOrigPosition());
}

#endif