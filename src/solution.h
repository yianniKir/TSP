#ifndef SOLUTION_H
#define SOLUTION_H

#include "TSP.h"
#include "point.h"


float distBtwPoints(Point &point1, Point &point2);

class Solution{
public:
    float fitness;
    std::string perm;
    int lineNum;
    Solution() {}
    Solution(std::string perm) : perm(perm){
        //Solution only needs a string, which is a permutation of the solution
        permLen = perm.length();
        lineNum = permLen - 1;
    }
    
    void calcFitness(Point points[]){
        //The fitness is the total distance traveled
        fitness = 0.0f;
        for(int i = 0; i < permLen - 1; i++){
            Point p1 = findPoint(perm[i], points, permLen);
            Point p2 = findPoint(perm[i+1], points, permLen);
        
            fitness += distBtwPoints(p1, p2);            
        }
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
        //debug
        //std::cout << perm << std::endl;
        for(int i = 0; i < lineNum; i++){
            std::string name = std::string(1, perm[i]) + std::string(1, perm[i+1]);
            
            for(int j = 0; j < numLineNames; j++){
                if (compareStrings(name, lineNames[j])){
                    std::sort(name.begin(), name.end());
                    order[i] = name;

                    //debug
                    //std::cout << order[i] << ", ";
                }
            }
            //debug
            //std::cout << std::endl;
        }

    }
    void mutate() {
    // Generate a random number between 0 and 99
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 99);
    int chance = dis(gen);

    // If the chance is less than 10, perform mutation
    if (chance < 40) {
        // Generate two random indices to swap
        std::uniform_int_distribution<> indexDis(0, permLen - 1);
        int index1 = indexDis(gen);
        int index2 = indexDis(gen);

        // Swap the characters at the generated indices
        std::swap(perm[index1], perm[index2]);
    }
}

    void setColor(glm::vec3 inColor){
        color = inColor;
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
    int permLen;
    
    glm::vec3 color;

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

bool compareSolutionsByFitness(Solution &sol1, Solution &sol2) {
    return sol1.fitness < sol2.fitness;
}

Solution cycleCrossover(Solution sol1, Solution sol2){
    Solution child("ABCDEFGHIJK");
    //start with first element in sol1, let this be char1
    //Find the char1 in sol2, let this be idx1
    //sol1[idx1] is the new element
    int sol2index = 0;
    for(int i = 0; i < sol1.lineNum + 1; i++){
        char c1 = sol1.perm[i];

        for(int j = 0; j < sol2.lineNum + 1; j++){
            if (c1 == sol2.perm[j]){
                sol2index = j;
            }
        }

        child.perm[i] = sol1.perm[sol2index];
    }
    return child;
}

//solution breed(sol1, sol2);

#endif