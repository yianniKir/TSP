#include "TSP.h"
#include "shaderHeader.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

const uint SCRWIDTH = 800;
const uint SCRHEIGHT = 800;
const float PI = 3.14159265359f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

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

float distBtwPoints(Point &point1, Point &point2){
    //get distance between points in a more understandable way
    return glm::distance(point1.getOrigPosition(), point2.getOrigPosition());
}

bool compareStrings(const std::string& str1, const std::string& str2) {
    //Compare two strings irrespective of order AE = EA (true)
    std::string sortedStr1 = str1;
    std::string sortedStr2 = str2;
    // Sort characters of both strings
    std::sort(sortedStr1.begin(), sortedStr1.end());
    std::sort(sortedStr2.begin(), sortedStr2.end());

    return sortedStr1 == sortedStr2;
}

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



Point FindInPointArr(char name, const Point* points, int numPoints) {
    //finds and returns a point given its name
    for (int i = 0; i < numPoints; ++i) {
        if (points[i].getName() == name) {
            return points[i];
        }
    }
    return Point('\0', glm::vec2(0.0f, 0.0f));
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


int main()
{   
    //GLFW initialization stuff
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
    
    //Very basic shader program, I am useng Joey De'Vries shader class
    //https://github.com/JoeyDeVries/LearnOpenGL/blob/master/includes/learnopengl/shader.h
    //I used pretty basic shaders, only changing things through glsl uniforms, this was likely not the best approach but it works
    Shader shader("src/vert.glsl", "src/frag.glsl");
    shader.use();

    //Load points
    const int numPoints = countLines("src/codeGens/points.txt");
    Point points[numPoints];
    if(loadPointNames(points, "src/codeGens/points.txt") == -1){
        return -1;
    }
    for(int i = 0; i < numPoints; i++){
        points[i].normalizePosition();
    }

    //Load lines
    const int numOfLines = countLines("src/codeGens/lines.txt");
    std::string lineNames[numOfLines];
    if(loadLineNames(lineNames, "src/codeGens/lines.txt") == -1){
        return -1;
    }

     //Fill lines array (This is used for math)
    Line lines[numOfLines];
    for(int i = 0; i < numOfLines; i++){
        Point startPoint = FindInPointArr(lineNames[i].c_str()[0], points, numPoints);
        Point endPoint = FindInPointArr(lineNames[i].c_str()[1], points, numPoints);
  
        lines[i] = Line(lineNames[i], startPoint, endPoint, glm::vec3(1.0f, 0.0f, 0.0f));
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

    //Store lines in buffers and make an array object
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
    const int numPerVertex = 12;
    const int numSquareVertices = numPerVertex* numPoints;
    float squareVertices[numSquareVertices];

    for(int i = 0; i < numSquareVertices; i = i + numPerVertex){
        int idx = std::floor(i/(numPerVertex));
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

    //Make squares array object
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




    //Solution initializations

    //write load solution function, specify colors for each line
    //solutionDraw strict with lineorder and linecolor
    Solution sol("CBADEFGHKJI");
    std::cout<< sol.calcFitness(points) << std::endl;
    sol.isValid();
    //This will need to be the first lineorder in solutiondraw (same number of lines for every solution)
    const int solutionLineNum = sol.lineNum;

    std::string lineOrder[solutionLineNum];

    sol.lineOrder(lineOrder, lineNames, numOfLines);

    int ticker = 0;

    //Render loop
    while(!glfwWindowShouldClose(window))
    {
        //Get delta time for smoother performance across devices
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //Input
        processInput(window);
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        


        shader.use();
        
        //USED FOR TESTING, CHANGING COLOR OF LINES
        setLineColor(lineOrder[ticker], glm::vec3(0.0f,1.0f,0.0f), numOfLines, lines);
        std::cout << lineOrder[ticker] << std::endl;


        // Draw the lines
        //This draws every line individually, it is a miracle it works the way it does honestly.
        //Since it draws every line individually, merely changing an element in lines[] color beforehand is all that is needed to change the color of a line
        glBindVertexArray(linesArray);
        int linesIndex = 0;
        for(int i = 0; i < numOfLines*2; i = i + 2){  
            shader.setVec3("color", lines[linesIndex].color);
            glDrawArrays(GL_LINES, i, i + 2);
            linesIndex++;
        }
        
        //Draw the points
        shader.setVec3("color",0.0f,0.0f,1.0f);
        glBindVertexArray(squaresArray);
        glDrawArrays(GL_TRIANGLES, 0, numSquareVertices/2);

        //Reset vertex array i guess
        glBindVertexArray(0);

        //Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
        ticker++;
        if(ticker >= solutionLineNum){
            std::cout << "NEW RUN" << std::endl;
            ticker = 0;
            for(int i = 0; i < solutionLineNum; i++){
                setLineColor(lineOrder[i], glm::vec3(1.0f,0.0f,0.0f), numOfLines, lines);
            }
        }
        //Delay(I NEED TO CHANGE THIS THIS IS NOT A GOOD WAY LMAOOOO)
        usleep(600000);
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
