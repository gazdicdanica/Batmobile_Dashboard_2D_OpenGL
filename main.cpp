// Autor: Bogdan Davinic

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>   
#include <GLFW/glfw3.h>
//Biblioteke za pokretanje generatora podataka
#include "TestBed.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define CRES 100 // Circle Resolution = Rezolucija kruga

unsigned int compileShader(GLenum type, const char* source); 
unsigned int createShader(const char* vsSource, const char* fsSource);
static unsigned loadImageToTexture(const char* filePath);

int main(void)
{

    //Inicijalizacija objekta tipa Car
    Car car = getCar();

    if (!glfwInit())
    {
        std::cout << "GLFW Biblioteka se nije ucitala! :(\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    unsigned int wWidth = 800;
    unsigned int wHeight = 600;
    const char wTitle[] = "Batmobile dashboard";
    window = glfwCreateWindow(wWidth, wHeight, wTitle, NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Prozor nije napravljen! :(\n";
        glfwTerminate();
        return 2;
    }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW nije mogao da se ucita! :'(\n";
        return 3;
    }

    glfwMaximizeWindow(window);
    unsigned int basicShader = createShader("basic.vert", "basic.frag");
    unsigned int textureShader = createShader("texture.vert", "texture.frag");
    unsigned int progressShader = createShader("progress.vert", "basic.frag");

    float nameVertices[] = {
        //X     Y     S    T
        -1.0f, 0.95f, 0.0, 1.0,
        -1.0f, 0.75f, 0.0, 0.0,
        -0.5f, 0.95f, 1.0, 1.0,
        -0.5f, 0.75f, 1.0, 0.0
    };


    float batVertices[] = {
        -0.15625, 0.64, 0.0, 0.0, 0.0, 0.5, //0
        -0.09375, 0.625, 0.0, 0.0, 0.0, 0.5, //1
        -0.125, 0.5625, 0.0, 0.0, 0.0, 0.5, //2
        -0.1, 0.45, 0.0, 0.0, 0.0, 0.5, //3
        -0.0625, 0.4375, 0.0, 0.0, 0.0, 0.5, //4
        -0.0835, 0.5625, 0.0, 0.0, 0.0, 0.5, //5
        -0.025, 0.525, 0.0, 0.0, 0.0, 0.5, //6
        0.0, 0.35, 0.0, 0.0, 0.0, 0.5, //7
        0.025, 0.525, 0.0, 0.0, 0.0, 0.5, //8
        0.0625, 0.4375, 0.0, 0.0, 0.0, 0.5, //9
        0.0835, 0.5625, 0.0, 0.0, 0.0, 0.5, //10
        0.1, 0.45, 0.0, 0.0, 0.0, 0.5, //11
        0.09375, 0.625, 0.0, 0.0, 0.0, 0.5, //12
        0.125, 0.5625, 0.0, 0.0, 0.0, 0.5, //13
        0.15625, 0.64, 0.0, 0.0, 0.0, 0.5, //14
        -0.025, 0.6, 0.0, 0.0, 0.0, 0.5, //15
        -0.015, 0.5625, 0.0, 0.0, 0.0, 0.5, //16
        0.0, 0.525, 0.0, 0.0, 0.0, 0.5, //17
        0.015, 0.5625, 0.0, 0.0, 0.0, 0.5, //18
        0.025, 0.6, 0.0, 0.0, 0.0, 0.5 //19
    };


    float minX = batVertices[0];
    float maxX = batVertices[0];
    float minY = batVertices[1];
    float maxY = batVertices[1];

    for (int i = 0; i < sizeof(batVertices) / sizeof(float); i += 6) {
        float x = batVertices[i];
        float y = batVertices[i + 1];

        if (x < minX) minX = x;
        if (x > maxX) maxX = x;
        if (y < minY) minY = y;
        if (y > maxY) maxY = y;
    }


    float padding = 0.3;

    float displayVertices[] = {

        minX - padding, maxY + padding, 0.23, 0.28, 0.43, 1.0, // UL
        minX - padding, minY - padding, 0.23, 0.28, 0.43, 1.0, // DL
        maxX + padding, maxY + padding, 0.23, 0.28, 0.43, 1.0, // UR
        maxX + padding, minY - padding, 0.23, 0.28, 0.43, 1.0,  // DR
    };

    float tachometerVertices[] = {
        -0.8f, 0.0f, 0.0, 1.0,
        -0.8f, -0.9f, 0.0, 0.0,
        -0.3f, 0.0f, 1.0, 1.0,
        -0.3f, -0.9f, 1.0, 0.0
    };

    float tachLineVertices[] = {
        -0.55f, -0.45f, 1.0, 0.0, 0.0, 1.0,
        -0.73f, -0.65f, 1.0, 0.0, 0.0, 1.0
    };

    float boxCenterX = 0.55f;
    float boxCenterY = -0.45f;

    float circle[CRES * 6 + 12];
    float r = 0.25;

    circle[0] = boxCenterX;
    circle[1] = boxCenterY;

    circle[2] = 0.0f;
    circle[3] = 0.0f;
    circle[4] = 0.0f;
    circle[5] = 1.0f;

    for (int i = 0; i <= CRES; ++i) {
        circle[6 + 6 * i] = boxCenterX - r * cos((3.141592 / 180) * (i * 360 / CRES));
        circle[6 + 6 * i + 1] = boxCenterY + 1.75f * r * sin((3.141592 / 180) * (i * 360 / CRES));

        circle[6 + 6 * i + 2] = 0.0f;
        circle[6 + 6 * i + 3] = 0.0f;
        circle[6 + 6 * i + 4] = 0.0f;
        circle[6 + 6 * i + 5] = 1.0f;
    }

    float xLeft = displayVertices[12] + 0.1;
    float xRight = displayVertices[12] + 0.3;
    float yMax = displayVertices[1];
    float yMin = displayVertices[7];

    float step = (yMax - yMin) / 10;

    

    float progressBar[8 * 6 + 18 * 6] = {
        xLeft + 0.005 , yMax - 0.005,  0.23, 0.28, 0.43, 1.0, //0
        xRight - 0.005, yMax - 0.005,  0.23, 0.28, 0.43,1.0, //6
        xLeft + 0.005, yMin + 0.005,  0.23, 0.28, 0.43, 1.0, //12
        xRight - 0.005, yMin + 0.005,  0.23, 0.28, 0.43, 1.0, //18

        xLeft , yMax, 0.5, 0.5, 0.5, 1.0, //24
        xRight, yMax, 0.5, 0.5, 0.5, 1.0, //30
        xLeft, yMin, 0.5, 0.5, 0.5, 1.0, //36
        xRight, yMin, 0.5, 0.5, 0.5, 1.0, //42
        //48
    };

    const int totalElements = 8 * 6 + 18 * 6;
    const int initializedElements = 8 * 6;
    const int remainingSpace = totalElements - initializedElements;

    // Determine the starting index for the loop
    const int loopStartIndex = initializedElements / 6;

    for (int i = 4; i < 13; i++) {
        float stepTop = yMax - (i - 3) * step;

        progressBar[i * 12] = xLeft;
        progressBar[i * 12 + 1] = stepTop;
        progressBar[i * 12 + 2] = 0.5;
        progressBar[i * 12 + 3] = 0.5;
        progressBar[i * 12 + 4] = 0.5;
        progressBar[i * 12 + 5] = 1.0;

        progressBar[i * 12 + 6] = xRight;
        progressBar[i * 12 + 7] = stepTop;
        progressBar[i * 12 + 8] = 0.5;
        progressBar[i * 12 + 9] = 0.5;
        progressBar[i * 12 + 10] = 0.5;
        progressBar[i * 12 + 11] = 1.0;
    }

    float lights[] = {
        // check engine
        -0.1, -0.1, 0.0, 1.0,
        -0.1, -0.3, 0.0, 0.0,
        0.1, -0.1, 1.0, 1.0,
        0.1, -0.3, 1.0, 0.0,

        // battery light
        -0.08, -0.5, 0.0, 1.0,
        -0.08, -0.7, 0.0, 0.0,
        0.08, -0.5, 1.0, 1.0,
        0.08, -0.7, 1.0, 0.0
    };

    unsigned int indices[] = // Indeksi tjemena
    {
        0, 1, 2, //Prvi trougao sacinjen od tjemena 0, tjemena 1 i tjemena 2 (Prednja strana mu je naprijed)
        1, 2, 3, //Drugi trougao (Zadnja strana mu je naprijed)
        1, 3, 4,
        4, 5, 6,
        4, 6, 7,
        6, 7, 8,
        7, 8, 9,
        8, 9, 10,
        9, 11, 12,
        11, 12, 13,
        12, 13, 14,
        6, 15, 16,
        6, 16, 17,
        16, 17, 18,
        8, 17, 18,
        8, 18, 19

    };
    unsigned int stride = (2 + 4) * sizeof(float);
    unsigned int textureStride = (2 + 2) * sizeof(float);
    unsigned int VAO[8];
    glGenVertexArrays(8, VAO); 

    unsigned int VBO[8];
    glGenBuffers(8, VBO); 

    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(batVertices), batVertices, GL_STATIC_DRAW); 
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0); 
    glEnableVertexAttribArray(0); 
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float))); 
    glEnableVertexAttribArray(1);

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(displayVertices), displayVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(VAO[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tachometerVertices), tachometerVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, textureStride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, textureStride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(VAO[3]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circle), circle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(VAO[4]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tachLineVertices), tachLineVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(VAO[5]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[5]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(progressBar), progressBar, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(VAO[6]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[6]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(nameVertices), nameVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, textureStride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, textureStride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    glBindVertexArray(VAO[7]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[7]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lights), lights, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, textureStride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, textureStride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    unsigned tachometerTexture = loadImageToTexture("res/tachometer.png"); //Ucitavamo teksturu
    glBindTexture(GL_TEXTURE_2D, tachometerTexture); //Podesavamo teksturu
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);
    
    
    unsigned nameTexture = loadImageToTexture("res/name.png"); //Ucitavamo teksturu
    glBindTexture(GL_TEXTURE_2D, nameTexture); //Podesavamo teksturu
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);


    unsigned engineTexture = loadImageToTexture("res/check-engine.png");
    glBindTexture(GL_TEXTURE_2D, engineTexture); //Podesavamo teksturu
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);
    
    
    unsigned batteryTexture = loadImageToTexture("res/battery-problem.png");
    glBindTexture(GL_TEXTURE_2D, batteryTexture); //Podesavamo teksturu
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);




    unsigned uTexLoc = glGetUniformLocation(textureShader, "uTex");
    glUniform1i(uTexLoc, 0);
    
    //Pokretanje simulacije, kako bi se menjalo stanje automobila kroz vreme
    startSimulation(&car);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    float x = 0;
    float y = 0;
    bool blending = false;

    bool checkEngine = false;
    bool batteryLight = false;

    unsigned int uPosLoc = glGetUniformLocation(basicShader, "uPos");
    unsigned int oneLoc = glGetUniformLocation(basicShader, "one");
    unsigned int rotAnLoc = glGetUniformLocation(basicShader, "rotationAngle");
    int uColLoc = glGetUniformLocation(basicShader, "uCol");
    unsigned int newPosLoc = glGetUniformLocation(progressShader, "newPos");

    
    float maxRPM = 8000.0f;
    float angleRange = 240.0f; 
    
    while (!glfwWindowShouldClose(window)) 
    {
        glfwPollEvents();

        int gear = car.getGear();
        float rpm = car.getTachometer();
        int km = car.getOdometer();
        km = km % 10 + 1;

        float rotationAngle = (rpm / maxRPM) * angleRange * 1.4;

        float progressHeight = km != 0 ? yMin + km * step - 0.005 : yMin + 0.005; 

        //std::cout << std::to_string(km) + "\n";
        //std::cout << std::to_string(progressHeight) + "\t" + std::to_string(km) + "\n";


        //Brisanje ekrana
        glClearColor(0.1, 0.1, 0.1, 1.0); //Podesavanje boje pozadine: RGBA (R - Crvena, G - Zelena, B - Plava, A = neprovidno; Opseg od 0 do 1: sto je vrijednost bliza 0, to je tamnija. Sto je Alfa bliza 0, projekat je vise providan)
        glClear(GL_COLOR_BUFFER_BIT);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glUseProgram(basicShader);
        glUniform1f(oneLoc, 0);
        glUniform2f(uPosLoc, 0, 0);
        glBindVertexArray(VAO[5]);
        glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);

        glUseProgram(progressShader);
        glUniform1f(newPosLoc, progressHeight);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glUseProgram(basicShader);
        glUniform1f(oneLoc, 0);
        glUniform2f(uPosLoc, 0, 0);

        glBindVertexArray(VAO[5]);
        glDrawArrays(GL_LINES, 8, 18);

        glBindVertexArray(VAO[1]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(displayVertices) / stride);

        glBindVertexArray(VAO[3]);
        float R = abs(sin( 2* gear * glfwGetTime()));
        glUniform3f(uColLoc, R, 0.0, 0.0);
        glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(circle) / stride);
            
        glUniform1i(oneLoc, 1);
        glLineWidth(5.0);
        glBindVertexArray(VAO[4]);
        float G = abs(sin(glfwGetTime())); //Absolutna vrijednost sinusa trenutnog vremena
        glUniform3f(uColLoc, 0, G, 0);
        glUniform1f(rotAnLoc, rotationAngle);
        glDrawArrays(GL_LINES, 0, 2);

        glUseProgram(textureShader);
        glBindVertexArray(VAO[2]);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glActiveTexture(GL_TEXTURE0); //tekstura koja se bind-uje nakon ovoga ce se koristiti sa SAMPLER2D uniformom u sejderu koja odgovara njenom indeksu
        glBindTexture(GL_TEXTURE_2D, tachometerTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(tachometerVertices) / textureStride);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindVertexArray(VAO[6]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, nameTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);


        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
            car.setCheckEngine(true);
        }
        if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
            car.setCheckEngine(false);
            car.setBatteryLight(false);
        }
        if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
            car.setBatteryLight(true);
        }

        if (car.getCheckEngineLight()) {
            glBindVertexArray(VAO[7]);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, engineTexture);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        
        if (car.getBatteryProblemLight()) {
            glBindVertexArray(VAO[7]);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, batteryTexture);
            glUniform1f(uTexLoc, 0);
            glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        


        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            if (y <= 0.3)
                y += 0.001;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            if (y > -0.3)
                y -= 0.001;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            if (x <= 0.3)
                x += 0.001;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            if (x >= -0.3)
                x -= 0.001;
        }
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        {
            blending = true;
        }
        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        {
            blending = false;
        }
        if (gear == 1)
        {

            glEnable(GL_PROGRAM_POINT_SIZE);
            glPointSize(6);
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            
        }
        else if (gear == 2)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        if (blending) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        else {
            glDisable(GL_BLEND);

        }
        glUseProgram(basicShader);
        glBindVertexArray(VAO[0]);
        glUniform2f(uPosLoc, x, y);
        glUniform1i(oneLoc, 0);
        glUniform3f(uColLoc, 0, 0, 0);
        glDrawElements(GL_TRIANGLES, 48, GL_UNSIGNED_INT, (void*)(0 * sizeof(unsigned int)));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glUseProgram(0);

        //Zamjena vidljivog bafera sa pozadinskim
        glfwSwapBuffers(window);
    }

    // Zaustavljanje simulacije
    endSimulation(&car);
    glDeleteBuffers(7, VBO);
    glDeleteVertexArrays(7, VAO);
    glDeleteProgram(basicShader);
    glDeleteProgram(textureShader);
    glfwTerminate();
    return 0;
}


unsigned int compileShader(GLenum type, const char* source)
{
    //Uzima kod u fajlu na putanji "source", kompajlira ga i vraca sejder tipa "type"
    //Citanje izvornog koda iz fajla
    std::string content = "";
    std::ifstream file(source);
    std::stringstream ss;
    if (file.is_open())
    {
        ss << file.rdbuf();
        file.close();
        std::cout << "Uspjesno procitao fajl sa putanje \"" << source << "\"!" << std::endl;
    }
    else {
        ss << "";
        std::cout << "Greska pri citanju fajla sa putanje \"" << source << "\"!" << std::endl;
    }
     std::string temp = ss.str();
     const char* sourceCode = temp.c_str(); //Izvorni kod sejdera koji citamo iz fajla na putanji "source"

    int shader = glCreateShader(type); //Napravimo prazan sejder odredjenog tipa (vertex ili fragment)
    
    int success; //Da li je kompajliranje bilo uspjesno (1 - da)
    char infoLog[512]; //Poruka o gresci (Objasnjava sta je puklo unutar sejdera)
    glShaderSource(shader, 1, &sourceCode, NULL); //Postavi izvorni kod sejdera
    glCompileShader(shader); //Kompajliraj sejder

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success); //Provjeri da li je sejder uspjesno kompajliran
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog); //Pribavi poruku o gresci
        if (type == GL_VERTEX_SHADER)
            printf("VERTEX");
        else if (type == GL_FRAGMENT_SHADER)
            printf("FRAGMENT");
        printf(" sejder ima gresku! Greska: \n");
        printf(infoLog);
    }
    return shader;
}
unsigned int createShader(const char* vsSource, const char* fsSource)
{
    //Pravi objedinjeni sejder program koji se sastoji od Vertex sejdera ciji je kod na putanji vsSource

    unsigned int program; //Objedinjeni sejder
    unsigned int vertexShader; //Verteks sejder (za prostorne podatke)
    unsigned int fragmentShader; //Fragment sejder (za boje, teksture itd)

    program = glCreateProgram(); //Napravi prazan objedinjeni sejder program

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource); //Napravi i kompajliraj vertex sejder
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource); //Napravi i kompajliraj fragment sejder

    //Zakaci verteks i fragment sejdere za objedinjeni program
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program); //Povezi ih u jedan objedinjeni sejder program
    glValidateProgram(program); //Izvrsi provjeru novopecenog programa

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success); //Slicno kao za sejdere
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        std::cout << "Objedinjeni sejder ima gresku! Greska: \n";
        std::cout << infoLog << std::endl;
    }

    //Posto su kodovi sejdera u objedinjenom sejderu, oni pojedinacni programi nam ne trebaju, pa ih brisemo zarad ustede na memoriji
    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}

static unsigned loadImageToTexture(const char* filePath) {
    int TextureWidth;
    int TextureHeight;
    int TextureChannels;
    unsigned char* ImageData = stbi_load(filePath, &TextureWidth, &TextureHeight, &TextureChannels, 0);
    if (ImageData != NULL)
    {
        //Slike se osnovno ucitavaju naopako pa se moraju ispraviti da budu uspravne
        stbi__vertical_flip(ImageData, TextureWidth, TextureHeight, TextureChannels);

        // Provjerava koji je format boja ucitane slike
        GLint InternalFormat = -1;
        switch (TextureChannels) {
        case 1: InternalFormat = GL_RED; break;
        case 3: InternalFormat = GL_RGB; break;
        case 4: InternalFormat = GL_RGBA; break;
        default: InternalFormat = GL_RGB; break;
        }

        unsigned int Texture;
        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, TextureWidth, TextureHeight, 0, InternalFormat, GL_UNSIGNED_BYTE, ImageData);
        glBindTexture(GL_TEXTURE_2D, 0);
        // oslobadjanje memorije zauzete sa stbi_load posto vise nije potrebna
        stbi_image_free(ImageData);
        return Texture;
    }
    else
    {
        std::cout << "Textura nije ucitana! Putanja texture: " << filePath << std::endl;
        stbi_image_free(ImageData);
        return 0;
    }
}
