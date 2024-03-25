#ifndef SQUARE_H
#define SQUARE_H

#include "TSP.h"

class Square {
    //Used for drawing squares to represent nodes
    public: 
        glm::vec2 upright;
        glm::vec2 upleft;
        glm::vec2 botleft;
        glm::vec2 botright;

        Square(float cx, float cy): cx(cx), cy(cy) {
            upright = glm::vec2(cx+0.02f, cy+0.02f);
            upleft = glm::vec2(cx-0.02f, cy+0.02f);
            botleft = glm::vec2(cx-0.02f, cy-0.02f);
            botright = glm::vec2(cx+0.02f, cy-0.02f);
        }

    private:
        float cx;
        float cy;
        
};

#endif