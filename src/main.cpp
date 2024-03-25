#include "TSP.h"
#include "shaderHeader.h"

#include "point.h"
#include "solution.h"
#include "line.h"
#include "square.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);



float deltaTime = 0.0f;
float lastFrame = 0.0f;


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
    
    //numofsolutions constant from tsp.h, currently 6
    Solution solutions[numOfSolutions];
    
    //Line colors

    //Generate initial solutions
    for (int i = 0; i < numOfSolutions; ++i) {
        solutions[i] = Solution("ABCDEFGHIJK");
        solutions[i].genRandomPerm();
        if(!solutions[i].isValid()){
            std::cerr << "ERROR WHEN GENERATING INITIAL SOLUTIONS" << std::endl;
        }
    }
    const int solutionLineNum = solutions[0].lineNum;
    //Line num[x][y] where x is an int from 0 to numofsolutions, and y is an int that is the index of the line to color
    std::string lineOrder[numOfSolutions][solutionLineNum];
    //assign linecolor orders;
    for(int i = 0; i < numOfSolutions; i++){
        solutions[i].lineOrder(lineOrder[i], lineNames, numOfLines);
    }
    
    //we will only draw the top 3 solutions, so only 3 colors are needed
    glm::vec3 solutionColors[3] = {
        glm::vec3(1.0f,0.749f,0.0f), //orange
        glm::vec3(0.5f,1.0f,0.0f), //green
        glm::vec3(0.0f,0.788f,1.0f) //cyan
    };
    
    int idx = 0; // max numofsolutions
    int idy = 0; //max solutionlinenum
    bool newGeneration = true;

    int lowestFitness = 100000000;
    int count = 0;

    Solution bestSolution;

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
        
        if(newGeneration == true){
            //calculate fitness and rank
            for(int i = 0; i < numOfSolutions; i++){
                solutions[i].calcFitness(points);
            }
            std::sort(std::begin(solutions), std::end(solutions), compareSolutionsByFitness);

            //select top 3 (will need to change later)
            Solution top = solutions[0];
            Solution mid = solutions[1];
            Solution bot = solutions[2];

            Solution c1 = cycleCrossover(top, mid);
            Solution c2 = cycleCrossover(mid, bot);
            Solution c3 = cycleCrossover(bot, top);

            
            //Filling lowest 3
            solutions[3] = c1;
            solutions[4] = c2;
            solutions[5] = c3;

            for(int i = 0; i < numOfSolutions; i++)
                solutions[i].mutate();

            if(solutions[0].fitness < lowestFitness){
                lowestFitness = solutions[0].fitness;
                bestSolution = solutions[0];
                std::cout<< idx << ". " << lowestFitness << std::endl;
                count = 0;
            }else{
                count++;
            }

            if(count > 1000){
                std::cout <<"done" <<std::endl;
                //glfwSetWindowShouldClose(window, true);
                newGeneration = false;
            }
            /*std::cout << top.getPerm() << std::endl;
            std::cout << mid.getPerm() << std::endl;
            std::cout << bot.getPerm() << std::endl;
            std::cout << "\n\n";
            std::cout << c1.getPerm() << std::endl;
            std::cout << c2.getPerm() << std::endl;
            std::cout << c3.getPerm() << std::endl;
            std::cout << std::endl;*/

            //newGeneration = false;


            //Redo drawing stuff at end (lineorder)
        }



        // Draw the lines
        //This draws every line individually, it is a miracle it works the way it does honestly.
        //Since it draws every line individually, merely changing an element in lines[] color beforehand is all that is needed to change the color of a line
        if(!newGeneration){
            bestSolution.lineOrder(lineOrder[0], lineNames, numOfLines);
            setLineColor(lineOrder[0][idy], solutionColors[0], numOfLines, lines);
            if(idy < solutionLineNum - 1){
                std::cout << lineOrder[0][idy] << std::endl;
                idy++;
                std::cout << "Best solution: " << bestSolution.getPerm() << ", " << bestSolution.fitness<<std::endl;
                
            }
            usleep(500000);
            
        }
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

        //when done drawing, ie when idx and idy are good, newGeneration = true;
        
        //
        idx++;


        //Delay(I NEED TO CHANGE THIS THIS IS NOT A GOOD WAY LMAOOOO)
        //usleep(300000);
        
        
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
