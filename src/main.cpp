#include "vendors/glad/glad.h"
#include "vendors/GLFW/glfw3.h"

#include "vendors/glm/glm.hpp"
#include "vendors/glm/gtc/matrix_transform.hpp"
#include "vendors/glm/gtc/type_ptr.hpp"

#include "shaderHeader.h"
#include <iostream>
#include <string>

#include <sstream>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

const uint SCRWIDTH = 800;
const uint SCRHEIGHT = 800;
const float PI = 3.14159265359f;

class Square {
    public: 
        glm::vec2 upright;
        glm::vec2 upleft;
        glm::vec2 botleft;
        glm::vec2 botright;

        Square(float cx, float cy): cx(cx), cy(cy) {
            upright = glm::vec2(cx+0.01f, cy+0.01f);
            upleft = glm::vec2(cx-0.01f, cy+0.01f);
            botleft = glm::vec2(cx-0.01f, cy-0.01f);
            botright = glm::vec2(cx+0.01f, cy-0.01f);
        }

    private:
        float cx;
        float cy;
        
};

class Point {
public:
    Point() {}

    Point(char name, const glm::vec2& position) : name(name), position(position) {}

    char getName() const { return name; }
    glm::vec2 getPosition() const { return position; }

    void normalizePosition() {
        position.x /= SCRWIDTH;
        position.y /= SCRHEIGHT;
    }


private:
    char name;
    glm::vec2 position;
 
};

class Line {
public:
    Line() {}

    Line(std::string name, const Point& start, const Point& end) : name(name), start(start), end(end) {}

    Point getStart() const { return start; }
    Point getEnd() const { return end; }
    std::string getName() const { return name; }
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

Point FindInPointArr(char name, const Point* points, int numPoints) {
    for (int i = 0; i < numPoints; ++i) {
        if (points[i].getName() == name) {
            return points[i];
        }
    }
    return Point('\0', glm::vec2(0.0f, 0.0f));
}

const int countLines(const std::string path) {
    std::ifstream fin;
    fin.open(path);
    if (!fin) {
        std::cerr << "ERROR: INVALID PTH" << path << std::endl;
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

int loadLineNames(std::string lineNames[], std::string path){
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

int loadPointNames(Point points[], std::string path){
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

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    GLFWwindow* window = glfwCreateWindow(SCRWIDTH, SCRHEIGHT, "TSP-John Kiritsis", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1; 
    }
    //set coordinate system
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glViewport(0, 0, SCRWIDTH, SCRHEIGHT);
    
    Shader shader("src/vert.glsl", "src/frag.glsl");
    shader.use();
    
    //Declare some general points
    
    /*Point points[] = {
        Point('A', glm::vec2(78.0f,  -481.0f)),
        Point('B', glm::vec2(744.0f,  -150.0f)),
        Point('C', glm::vec2(-279.0f,  -110.0f)),
        Point('D', glm::vec2(587.0f,  -700.0f)),
        Point('E', glm::vec2(239.0f,  -250.0f)),
        Point('F', glm::vec2(-426.0f,  200.0f)),
        Point('G', glm::vec2(565.0f,  231.0f)),
    };*/

    const int numPoints = countLines("src/codeGens/points.txt");
    Point points[numPoints];
    if(loadPointNames(points, "src/codeGens/points.txt") == -1){
        return -1;
    }

    for(int i = 0; i < numPoints; i++){
        points[i].normalizePosition();
    }

    const int numOfLines = countLines("src/codeGens/lines.txt");
    std::string lineNames[numOfLines];
    if(loadLineNames(lineNames, "src/codeGens/lines.txt") == -1){
        return -1;
    }

    Line lines[numOfLines];

    //Fill lines array (This is used for math)
    for(int i = 0; i < numOfLines; i++){
        Point startPoint = FindInPointArr(lineNames[i].c_str()[0], points, numPoints);
        Point endPoint = FindInPointArr(lineNames[i].c_str()[1], points, numPoints);
  
        lines[i] = Line(lineNames[i], startPoint, endPoint);
        //lines[i].debug();
    }

    //Lines for display
    const int numVerticesElements = numOfLines * 4;
    float vertices[numVerticesElements];

    for (int i = 0; i < numVerticesElements; i= i + 4) {
        int idx = std::floor(i/4);
        vertices[i] = lines[idx].getStart().getPosition().x;
        vertices[i+1] = lines[idx].getStart().getPosition().y;
        
        vertices[i+2] = lines[idx].getEnd().getPosition().x;
        vertices[i+3] = lines[idx].getEnd().getPosition().y;  
    }


    uint linesBuffer, linesArray;
    glGenVertexArrays(1, &linesArray);
    glGenBuffers(1, &linesBuffer);
    glBindVertexArray(linesArray);

    glBindBuffer(GL_ARRAY_BUFFER, linesBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //Points for Display
    const int numSquareVertices = 12* numPoints;
    float squareVertices[numSquareVertices];

    for(int i = 0; i < numSquareVertices; i = i + 12){
        int idx = std::floor(i/(12));
        Square dummy = Square(points[idx].getPosition().x,points[idx].getPosition().y);

        //Arr format upright, upleft, botleft, botright (x,y of course)
        squareVertices[i] = dummy.upright.x;
        squareVertices[i + 1] = dummy.upright.y;

        squareVertices[i + 2] = dummy.upleft.x;
        squareVertices[i + 3] = dummy.upleft.y;

        squareVertices[i + 4] = dummy.botleft.x;
        squareVertices[i + 5] = dummy.botleft.y;

        squareVertices[i + 6] = dummy.botleft.x;
        squareVertices[i + 7] = dummy.botleft.y;

        squareVertices[i + 8] = dummy.botright.x;
        squareVertices[i + 9] = dummy.botright.y;

        squareVertices[i + 10] = dummy.upright.x;
        squareVertices[i + 11] = dummy.upright.y;
    }

    uint squaresBuffer, squaresArray;
    glGenVertexArrays(1, &squaresArray);
    glGenBuffers(1, &squaresBuffer);
    glBindVertexArray(squaresArray);

    glBindBuffer(GL_ARRAY_BUFFER, squaresBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);
    
    //Render loop
    while(!glfwWindowShouldClose(window))
    {
        //Input
        processInput(window);
        // Clear the screen
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Use our shader program
        
        // Draw the line
        shader.setVec3("color",1.0f,0.0f,0.0f);
        glBindVertexArray(linesArray);
        glDrawArrays(GL_LINES, 0, numVerticesElements);

        shader.setVec3("color",0.0f,1.0f,0.0f);
        glBindVertexArray(squaresArray);
        glDrawArrays(GL_TRIANGLES, 0, numSquareVertices);
        //Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();


    }

    glfwTerminate();
    return 0;

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
