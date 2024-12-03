#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

//stb_image.h je header-only biblioteka za ucitavanje tekstura.
//Potrebno je definisati STB_IMAGE_IMPLEMENTATION prije njenog ukljucivanja
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


enum DisplayMode { GUI, TRANSPARENT, LOGO };
DisplayMode currentMode = GUI;
bool logoTransition = false;


float freezerTemp = -38.0f, fridgeTemp = 4.0f, brightness = 5.0f;
float desiredFreezerTemp = -18.0f, desiredFridgeTemp = 2.2f;

double x_pos, y_pos;

double logo_x, logo_y = 0.0f;
bool isPulsing2 = false;
bool moveLogo3 = false;
bool moveLogo4 = false;
float angle4 = 0.0f;
bool moveLogo5 = false;
float direction = 1.0f;

std::chrono::time_point<std::chrono::steady_clock> lastInteractionTime;
float colorIntensity = 0.0f;
float increment = 0.01f;

void stopKey2();
void stopKey3();
void stopKey4();
void stopKey5();
float calculateBlueTint(float temperature, float minTemp, float maxTemp);
float scaleBrightness();
unsigned int compileShader(GLenum type, const char* source);
unsigned int createShader(const char* vsSource, const char* fsSource);
static unsigned loadImageToTexture(const char* filePath);

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    x_pos = xpos;
    y_pos = ypos;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    //std::cout << x_pos << ", " << y_pos << std::endl;
    if (currentMode == GUI) {
        if (action == GLFW_PRESS) {
            std::cout << "klik na gui" << std::endl;
            lastInteractionTime = std::chrono::steady_clock::now();
            // samo levi klik
            if (button == GLFW_MOUSE_BUTTON_LEFT &&
                glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) != GLFW_PRESS &&
                glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS) {
                if (x_pos >= 122 and x_pos <= 157 and y_pos >= 368 and y_pos <= 405) {
                    freezerTemp += 0.1;
                    if (freezerTemp > -18.0) freezerTemp = -18.0;
                    std::cout << "poveca zamrzivac za 0.1" << std::endl;
                }
                else if (x_pos >= 227 and x_pos <= 262 and y_pos >= 368 and y_pos <= 405) {
                    freezerTemp -= 0.1;
                    if (freezerTemp < -38.0) freezerTemp = -38.0;
                    std::cout << "smanjuje zamrzivac za 0.1" << std::endl;
                }
                else if (x_pos >= 122 and x_pos <= 157 and y_pos >= 170 and y_pos <= 205) {
                    fridgeTemp += 0.1;
                    if (fridgeTemp > 7.0) fridgeTemp = 7.0;
                    std::cout << "poveca frizider za 0.1" << std::endl;
                }
                else if (x_pos >= 227 and x_pos <= 262 and y_pos >= 170 and y_pos <= 205) {
                    fridgeTemp -= 0.1;
                    if (fridgeTemp < 0.0) fridgeTemp = 0.0;
                    std::cout << "smanjuje frizider za 0.1" << std::endl;
                } ////////////////////////////////////////////////////////////
                else if (x_pos >= 122 and x_pos <= 157 and y_pos >= 468 and y_pos <= 505) {
                    desiredFreezerTemp += 0.1;
                    if (desiredFreezerTemp > -18.0) desiredFreezerTemp = -18.0;
                    std::cout << "poveca zeljenu zamrzivaca za 0.1" << std::endl;
                }
                else if (x_pos >= 227 and x_pos <= 262 and y_pos >= 468 and y_pos <= 505) {
                    desiredFreezerTemp -= 0.1;
                    if (desiredFreezerTemp < -38.0) desiredFreezerTemp = -38.0;
                    std::cout << "smanjuje zeljenu zamrzivaca za 0.1" << std::endl;
                }
                else if (x_pos >= 122 and x_pos <= 157 and y_pos >= 270 and y_pos <= 305) {
                    desiredFridgeTemp += 0.1;
                    if (desiredFridgeTemp > 7.0) desiredFridgeTemp = 7.0;
                    std::cout << "poveca zeljenu frizidera za 0.1" << std::endl;
                }
                else if (x_pos >= 227 and x_pos <= 262 and y_pos >= 270 and y_pos <= 305) {
                    desiredFridgeTemp -= 0.1;
                    if (desiredFridgeTemp < 0.0) desiredFridgeTemp = 0.0;
                    std::cout << "smanjuje zeljenu frizidera za 0.1" << std::endl;
                }
            }
            // oba klika na transparent dugme
            if (x_pos >= 140 and x_pos <= 260 and y_pos >= 678 and y_pos <= 718) {
                currentMode = TRANSPARENT;
                lastInteractionTime = std::chrono::steady_clock::now();
            }
            // oba klika na brightness +
            if (x_pos >= 122 and x_pos <= 157 and y_pos >= 568 and y_pos <= 605) {
                brightness += 1;
                if (brightness > 5.0) brightness = 5.0;
                std::cout << "povecava osvetljenje za 1" << std::endl;
            }
            // oba klika na brightness -
            if (x_pos >= 227 and x_pos <= 262 and y_pos >= 568 and y_pos <= 605) {
                brightness -= 1;
                if (brightness < 0.0) brightness = 0.0;
                std::cout << "smanjuje osvetljenje za 1" << std::endl;
            }
            // oba klika plus ctrl
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
                if (x_pos >= 122 and x_pos <= 156 and y_pos >= 368 and y_pos <= 405) {
                    freezerTemp += 1;
                    if (freezerTemp > -18.0) freezerTemp = -18.0;
                    std::cout << "poveca zamrzivac za 1" << std::endl;
                }
                else if (x_pos >= 227 and x_pos <= 262 and y_pos >= 368 and y_pos <= 405) {
                    freezerTemp -= 1;
                    if (freezerTemp < -38.0) freezerTemp = -38.0;
                    std::cout << "smanjuje zamrzivac za 1" << std::endl;
                }
                else if (x_pos >= 122 and x_pos <= 157 and y_pos >= 170 and y_pos <= 205) {
                    fridgeTemp += 1;
                    if (fridgeTemp > 7.0) fridgeTemp = 7.0;
                    std::cout << "poveca frizider za 1" << std::endl;
                }
                else if (x_pos >= 227 and x_pos <= 262 and y_pos >= 170 and y_pos <= 205) {
                    fridgeTemp -= 1;
                    if (fridgeTemp < 0.0) fridgeTemp = 0.0;
                    std::cout << "smanjuje frizider za 1" << std::endl;
                }////////////////////////////////////////////////////////////
                else if (x_pos >= 122 and x_pos <= 157 and y_pos >= 468 and y_pos <= 505) {
                    desiredFreezerTemp += 1;
                    if (desiredFreezerTemp > -18.0) desiredFreezerTemp = -18.0;
                    std::cout << "poveca zeljenu zamrzivaca za 1" << std::endl;
                }
                else if (x_pos >= 227 and x_pos <= 262 and y_pos >= 468 and y_pos <= 505) {
                    desiredFreezerTemp -= 1;
                    if (desiredFreezerTemp < -38.0) desiredFreezerTemp = -38.0;
                    std::cout << "smanjuje zeljenu zamrzivaca za 1" << std::endl;
                }
                else if (x_pos >= 122 and x_pos <= 157 and y_pos >= 270 and y_pos <= 305) {
                    desiredFridgeTemp += 1;
                    if (desiredFridgeTemp > 7.0) desiredFridgeTemp = 7.0;
                    std::cout << "poveca zeljenu frizidera za 1" << std::endl;
                }
                else if (x_pos >= 227 and x_pos <= 262 and y_pos >= 270 and y_pos <= 305) {
                    desiredFridgeTemp -= 1;
                    if (desiredFridgeTemp < 0.0) desiredFridgeTemp = 0.0;
                    std::cout << "smanjuje zeljenu frizidera za 1" << std::endl;
                }
            }
            //oba klika plus shift
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
                if (x_pos >= 122 and x_pos <= 156 and y_pos >= 368 and y_pos <= 405) {
                    freezerTemp += 5;
                    if (freezerTemp > -18.0) freezerTemp = -18.0;
                    std::cout << "poveca zamrzivac za 5" << std::endl;
                }
                else if (x_pos >= 227 and x_pos <= 262 and y_pos >= 368 and y_pos <= 405) {
                    freezerTemp -= 5;
                    if (freezerTemp < -38.0) freezerTemp = -38.0;
                    std::cout << "smanjuje zamrzivac za 5" << std::endl;
                }
                else if (x_pos >= 122 and x_pos <= 157 and y_pos >= 170 and y_pos <= 205) {
                    fridgeTemp += 5;
                    if (fridgeTemp > 7.0) fridgeTemp = 7.0;
                    std::cout << "poveca frizider za 5" << std::endl;
                }
                else if (x_pos >= 227 and x_pos <= 262 and y_pos >= 170 and y_pos <= 205) {
                    fridgeTemp -= 5;
                    if (fridgeTemp < 0.0) fridgeTemp = 0.0;
                    std::cout << "smanjuje frizider za 5" << std::endl;
                }////////////////////////////////////////////////////////////
                else if (x_pos >= 122 and x_pos <= 157 and y_pos >= 468 and y_pos <= 505) {
                    desiredFreezerTemp += 5;
                    if (desiredFreezerTemp > -18.0) desiredFreezerTemp = -18.0;
                    std::cout << "poveca zeljenu zamrzivaca za 5" << std::endl;
                }
                else if (x_pos >= 227 and x_pos <= 262 and y_pos >= 468 and y_pos <= 505) {
                    desiredFreezerTemp -= 5;
                    if (desiredFreezerTemp < -38.0) desiredFreezerTemp = -38.0;
                    std::cout << "smanjuje zeljenu zamrzivaca za 5" << std::endl;
                }
                else if (x_pos >= 122 and x_pos <= 157 and y_pos >= 270 and y_pos <= 305) {
                    desiredFridgeTemp += 5;
                    if (desiredFridgeTemp > 7.0) desiredFridgeTemp = 7.0;
                    std::cout << "poveca zeljenu frizidera za 5" << std::endl;
                }
                else if (x_pos >= 227 and x_pos <= 262 and y_pos >= 270 and y_pos <= 305) {
                    desiredFridgeTemp -= 5;
                    if (desiredFridgeTemp < 0.0) desiredFridgeTemp = 0.0;
                    std::cout << "smanjuje zeljenu frizidera za 5" << std::endl;
                }
            }
        }
    }
    else if (currentMode == TRANSPARENT) {
        if (action == GLFW_PRESS) {
            std::cout << "klik na transparent" << std::endl;
            lastInteractionTime = std::chrono::steady_clock::now();
        }
    }
    else if (currentMode == LOGO) {
        if (action == GLFW_PRESS) {
            if (isPulsing2) stopKey2();
            if (moveLogo3) stopKey3();
            if (moveLogo4) stopKey4();
            if (moveLogo5) stopKey5();
            lastInteractionTime = std::chrono::steady_clock::now();
            currentMode = GUI;
        }
    }
}

int main(void)
{

    if (!glfwInit())
    {
        std::cout<<"GLFW Biblioteka se nije ucitala! :(\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    unsigned int wWidth = 400;
    unsigned int wHeight = 800;
    const char wTitle[] = "Smart Fridge";
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

    unsigned int unifiedShader = createShader("basic.vert", "basic.frag");
    unsigned int blueTintShader = createShader("basic.vert", "blueTint.frag");
    unsigned int logoTransitionShader = createShader("basic.vert", "logoColorIntensity.frag");
    unsigned int logoShader = createShader("logo.vert", "logo.frag");
    unsigned int brightnessShader = createShader("brightness.vert", "brightness.frag");
    unsigned int indices[] = {
        0, 1, 2, // Prvi trougao
        1, 2, 3  // Drugi trougao
    };
    unsigned int stride = (2 + 2) * sizeof(float);

    // TRANSPARENT FRIDGE
    float verticesFridge[] = {
        // X      Y      S    T
        -0.9f,  0.05f,  0.0f, 0.0f, // Donji levi ugao
         0.9f,  0.05f,  1.0f, 0.0f, // Donji desni ugao
        -0.9f,  0.94f, 0.0f, 1.0f, // Gornji levi ugao
         0.9f,  0.94f, 1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_Fridge;
    glGenVertexArrays(1, &VAO_Fridge);
    glBindVertexArray(VAO_Fridge);
    unsigned int VBO_Fridge;
    glGenBuffers(1, &VBO_Fridge);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Fridge);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesFridge), verticesFridge, GL_STATIC_DRAW);
    unsigned int EBO_Fridge;
    glGenBuffers(1, &EBO_Fridge);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Fridge);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // PLAVA TINTA FRIDGE
    unsigned int VAO_BlueTint_Fridge;
    glGenVertexArrays(1, &VAO_BlueTint_Fridge);
    glBindVertexArray(VAO_BlueTint_Fridge);
    unsigned int VBO_BlueTint_Fridge;
    glGenBuffers(1, &VBO_BlueTint_Fridge);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_BlueTint_Fridge);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesFridge), verticesFridge, GL_STATIC_DRAW);
    unsigned int EBO_BlueTint_Fridge;
    glGenBuffers(1, &EBO_BlueTint_Fridge);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_BlueTint_Fridge);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    //TRANSPARENT FREEZER
    float verticesFreezer[] = {
        // X      Y      S    T
        -0.9f, -0.94f,  0.0f, 0.0f, // Donji levi ugao
         0.9f, -0.94f,  1.0f, 0.0f, // Donji desni ugao
        -0.9f, -0.05f,  0.0f, 1.0f, // Gornji levi ugao
         0.9f, -0.05f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_Freezer;
    glGenVertexArrays(1, &VAO_Freezer);
    glBindVertexArray(VAO_Freezer);
    unsigned int VBO_Freezer;
    glGenBuffers(1, &VBO_Freezer);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Freezer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesFreezer), verticesFreezer, GL_STATIC_DRAW);
    unsigned int EBO_Freezer;
    glGenBuffers(1, &EBO_Freezer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Freezer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // PLAVA TINTA FREEZER
    unsigned int VAO_BlueTint_Freezer;
    glGenVertexArrays(1, &VAO_BlueTint_Freezer);
    glBindVertexArray(VAO_BlueTint_Freezer);
    unsigned int VBO_BlueTint_Freezer;
    glGenBuffers(1, &VBO_BlueTint_Freezer);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_BlueTint_Freezer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesFreezer), verticesFreezer, GL_STATIC_DRAW);
    unsigned int EBO_BlueTint_Freezer;
    glGenBuffers(1, &EBO_BlueTint_Freezer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_BlueTint_Freezer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // LOGO MODE DISPLAY
    float verticesFullScreenLogo[] = {
        // X      Y      S    T
        -0.9f, -0.94f,  0.0f, 0.0f, // Donji levi ugao
         0.9f, -0.94f,  1.0f, 0.0f, // Donji desni ugao
        -0.9f,  0.94f,  0.0f, 1.0f, // Gornji levi ugao
         0.9f,  0.94f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_LOGO_display;
    glGenVertexArrays(1, &VAO_LOGO_display);
    glBindVertexArray(VAO_LOGO_display);
    unsigned int VBO_LOGO_display;
    glGenBuffers(1, &VBO_LOGO_display);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_LOGO_display);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesFullScreenLogo), verticesFullScreenLogo, GL_STATIC_DRAW);
    unsigned int EBO_LOGO_display;
    glGenBuffers(1, &EBO_LOGO_display);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_LOGO_display);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // LOGO 
    float verticesLogo[] = {
        // X      Y      S    T
        -0.2f, -0.2f,  0.0f, 0.0f, // Donji levi ugao
         0.2f, -0.2f,  1.0f, 0.0f, // Donji desni ugao
        -0.2f,  0.2f,  0.0f, 1.0f, // Gornji levi ugao
         0.2f,  0.2f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_LOGO;
    glGenVertexArrays(1, &VAO_LOGO);
    glBindVertexArray(VAO_LOGO);
    unsigned int VBO_LOGO;
    glGenBuffers(1, &VBO_LOGO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_LOGO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesLogo), verticesLogo, GL_STATIC_DRAW);
    unsigned int EBO_LOGO;
    glGenBuffers(1, &EBO_LOGO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_LOGO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //HOUR 0
    float verticesHour0[] = {
        // X      Y      S    T
        -0.3f,  0.8f,  0.0f, 0.0f, // Donji levi ugao
        -0.22f, 0.8f,  1.0f, 0.0f, // Donji desni ugao
        -0.3f,  0.85f,  0.0f, 1.0f, // Gornji levi ugao
        -0.22f, 0.85f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_HOUR0;
    glGenVertexArrays(1, &VAO_HOUR0);
    glBindVertexArray(VAO_HOUR0);
    unsigned int VBO_HOUR0;
    glGenBuffers(1, &VBO_HOUR0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_HOUR0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesHour0), verticesHour0, GL_STATIC_DRAW);
    unsigned int EBO_HOUR0;
    glGenBuffers(1, &EBO_HOUR0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_HOUR0);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    //HOUR 1
    float verticesHour1[] = {
        // X      Y      S    T
        -0.22f,  0.8f,  0.0f, 0.0f, // Donji levi ugao
        -0.14f, 0.8f,  1.0f, 0.0f, // Donji desni ugao
        -0.22f,  0.85f,  0.0f, 1.0f, // Gornji levi ugao
        -0.14f, 0.85f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_HOUR1;
    glGenVertexArrays(1, &VAO_HOUR1);
    glBindVertexArray(VAO_HOUR1);
    unsigned int VBO_HOUR1;
    glGenBuffers(1, &VBO_HOUR1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_HOUR1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesHour1), verticesHour1, GL_STATIC_DRAW);
    unsigned int EBO_HOUR1;
    glGenBuffers(1, &EBO_HOUR1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_HOUR1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    //: 1
    float verticesHM1[] = {
        // X      Y      S    T
        -0.14f,  0.81f,  0.0f, 0.0f, // Donji levi ugao
        -0.11f, 0.81f,  1.0f, 0.0f, // Donji desni ugao
        -0.14f,  0.83f,  0.0f, 1.0f, // Gornji levi ugao
        -0.11f, 0.83f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_HM1;
    glGenVertexArrays(1, &VAO_HM1);
    glBindVertexArray(VAO_HM1);
    unsigned int VBO_HM1;
    glGenBuffers(1, &VBO_HM1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_HM1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesHM1), verticesHM1, GL_STATIC_DRAW);
    unsigned int EBO_HM1;
    glGenBuffers(1, &EBO_HM1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_HM1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    //MINUTE 0
    float verticesMinute0[] = {
        // X      Y      S    T
        -0.11f,  0.8f,  0.0f, 0.0f, // Donji levi ugao
        -0.03f,  0.8f,  1.0f, 0.0f, // Donji desni ugao
        -0.11f,  0.85f,  0.0f, 1.0f, // Gornji levi ugao
        -0.03f,  0.85f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_MINUTE0;
    glGenVertexArrays(1, &VAO_MINUTE0);
    glBindVertexArray(VAO_MINUTE0);
    unsigned int VBO_MINUTE0;
    glGenBuffers(1, &VBO_MINUTE0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_MINUTE0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesMinute0), verticesMinute0, GL_STATIC_DRAW);
    unsigned int EBO_MINUTE0;
    glGenBuffers(1, &EBO_MINUTE0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_MINUTE0);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    //MINUTE 1
    float verticesMinute1[] = {
        // X      Y      S    T
        -0.03f,  0.8f,  0.0f, 0.0f, // Donji levi ugao
         0.05f,  0.8f,  1.0f, 0.0f, // Donji desni ugao
        -0.03f,  0.85f,  0.0f, 1.0f, // Gornji levi ugao
         0.05f,  0.85f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_MINUTE1;
    glGenVertexArrays(1, &VAO_MINUTE1);
    glBindVertexArray(VAO_MINUTE1);
    unsigned int VBO_MINUTE1;
    glGenBuffers(1, &VBO_MINUTE1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_MINUTE1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesMinute1), verticesMinute1, GL_STATIC_DRAW);
    unsigned int EBO_MINUTE1;
    glGenBuffers(1, &EBO_MINUTE1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_MINUTE1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    //: 2
    float verticesHM2[] = {
        // X      Y      S    T
        0.05f,  0.81f,  0.0f, 0.0f, // Donji levi ugao
        0.08f,  0.81f,  1.0f, 0.0f, // Donji desni ugao
        0.05f,  0.83f,  0.0f, 1.0f, // Gornji levi ugao
        0.08f,  0.83f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_HM2;
    glGenVertexArrays(1, &VAO_HM2);
    glBindVertexArray(VAO_HM2);
    unsigned int VBO_HM2;
    glGenBuffers(1, &VBO_HM2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_HM2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesHM2), verticesHM2, GL_STATIC_DRAW);
    unsigned int EBO_HM2;
    glGenBuffers(1, &EBO_HM2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_HM2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    //SECOND 0
    float verticesSecond0[] = {
        // X      Y      S    T
        0.08f,  0.8f,  0.0f, 0.0f, // Donji levi ugao
        0.16f, 0.8f,  1.0f, 0.0f, // Donji desni ugao
        0.08f,  0.85f,  0.0f, 1.0f, // Gornji levi ugao
        0.16f, 0.85f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_SECOND0;
    glGenVertexArrays(1, &VAO_SECOND0);
    glBindVertexArray(VAO_SECOND0);
    unsigned int VBO_SECOND0;
    glGenBuffers(1, &VBO_SECOND0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_SECOND0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesSecond0), verticesSecond0, GL_STATIC_DRAW);
    unsigned int EBO_SECOND0;
    glGenBuffers(1, &EBO_SECOND0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_SECOND0);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    //SECOND 1
    float verticesSecond1[] = {
        // X      Y      S    T
        0.16f,  0.8f,  0.0f, 0.0f, // Donji levi ugao
        0.24f, 0.8f,  1.0f, 0.0f, // Donji desni ugao
        0.16f,  0.85f,  0.0f, 1.0f, // Gornji levi ugao
        0.24f, 0.85f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_SECOND1;
    glGenVertexArrays(1, &VAO_SECOND1);
    glBindVertexArray(VAO_SECOND1);
    unsigned int VBO_SECOND1;
    glGenBuffers(1, &VBO_SECOND1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_SECOND1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesSecond1), verticesSecond1, GL_STATIC_DRAW);
    unsigned int EBO_SECOND1;
    glGenBuffers(1, &EBO_SECOND1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_SECOND1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    //fridge current temp - temp1
    float verticesTEMP1[] = {
        // X      Y      S    T
        -0.3f,  0.6f,  0.0f, 0.0f, // Donji levi ugao
         0.3f,  0.6f,  1.0f, 0.0f, // Donji desni ugao
        -0.3f,  0.65f,  0.0f, 1.0f, // Gornji levi ugao
         0.3f,  0.65f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_TEMP1;
    glGenVertexArrays(1, &VAO_TEMP1);
    glBindVertexArray(VAO_TEMP1);
    unsigned int VBO_TEMP1;
    glGenBuffers(1, &VBO_TEMP1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TEMP1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTEMP1), verticesTEMP1, GL_STATIC_DRAW);
    unsigned int EBO_TEMP1;
    glGenBuffers(1, &EBO_TEMP1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_TEMP1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    //fridge desired temp - temp2
    float verticesTEMP2[] = {
        // X      Y      S    T
        -0.4f,  0.35f,  0.0f, 0.0f, // Donji levi ugao
         0.4f,  0.35f,  1.0f, 0.0f, // Donji desni ugao
        -0.4f,  0.4f,  0.0f, 1.0f, // Gornji levi ugao
         0.4f,  0.4f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_TEMP2;
    glGenVertexArrays(1, &VAO_TEMP2);
    glBindVertexArray(VAO_TEMP2);
    unsigned int VBO_TEMP2;
    glGenBuffers(1, &VBO_TEMP2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TEMP2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTEMP2), verticesTEMP2, GL_STATIC_DRAW);
    unsigned int EBO_TEMP2;
    glGenBuffers(1, &EBO_TEMP2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_TEMP2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //freezer current temp - temp3
    float verticesTEMP3[] = {
        // X      Y      S    T
        -0.3f,  0.1f,  0.0f, 0.0f, // Donji levi ugao
         0.3f,  0.1f,  1.0f, 0.0f, // Donji desni ugao
        -0.3f,  0.15f,  0.0f, 1.0f, // Gornji levi ugao
         0.3f,  0.15f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_TEMP3;
    glGenVertexArrays(1, &VAO_TEMP3);
    glBindVertexArray(VAO_TEMP3);
    unsigned int VBO_TEMP3;
    glGenBuffers(1, &VBO_TEMP3);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TEMP3);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTEMP3), verticesTEMP3, GL_STATIC_DRAW);
    unsigned int EBO_TEMP3;
    glGenBuffers(1, &EBO_TEMP3);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_TEMP3);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //freezer desired temp - temp4
    float verticesTEMP4[] = {
        // X      Y      S    T
        -0.4f,  -0.15f,  0.0f, 0.0f, // Donji levi ugao
         0.4f,  -0.15f,  1.0f, 0.0f, // Donji desni ugao
        -0.4f,  -0.1f,  0.0f, 1.0f, // Gornji levi ugao
         0.4f,  -0.1f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_TEMP4;
    glGenVertexArrays(1, &VAO_TEMP4);
    glBindVertexArray(VAO_TEMP4);
    unsigned int VBO_TEMP4;
    glGenBuffers(1, &VBO_TEMP4);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TEMP4);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTEMP4), verticesTEMP4, GL_STATIC_DRAW);
    unsigned int EBO_TEMP4;
    glGenBuffers(1, &EBO_TEMP4);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_TEMP4);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    //brightness tint
    float verticesFullScreen[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f 
    };
    unsigned int VAO_B_tint;
    glGenVertexArrays(1, &VAO_B_tint);
    glBindVertexArray(VAO_B_tint);
    unsigned int VBO_B_tint;
    glGenBuffers(1, &VBO_B_tint);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_B_tint);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesFullScreen), verticesFullScreen, GL_STATIC_DRAW);
    unsigned int EBO_B_tint;
    glGenBuffers(1, &EBO_B_tint);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_B_tint);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(2 * sizeof(float)));
    //glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    //brightness
    float verticesB[] = {
        // X      Y      S    T
        -0.2f,  -0.4f,  0.0f, 0.0f, // Donji levi ugao
         0.2f,  -0.4f,  1.0f, 0.0f, // Donji desni ugao
        -0.2f,  -0.35f,  0.0f, 1.0f, // Gornji levi ugao
         0.2f,  -0.35f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_B;
    glGenVertexArrays(1, &VAO_B);
    glBindVertexArray(VAO_B);
    unsigned int VBO_B;
    glGenBuffers(1, &VBO_B);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_B);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesB), verticesB, GL_STATIC_DRAW);
    unsigned int EBO_B;
    glGenBuffers(1, &EBO_B);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_B);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // TEMP1 2.RED +
    float verticesTEMP1_plus[] = {
        // X      Y      S    T
        -0.4f,  0.48f,  0.0f, 0.0f, // Donji levi ugao
        -0.2f,  0.48f,  1.0f, 0.0f, // Donji desni ugao
        -0.4f,  0.58f,  0.0f, 1.0f, // Gornji levi ugao
        -0.2f,  0.58f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_TEMP1_plus;
    glGenVertexArrays(1, &VAO_TEMP1_plus);
    glBindVertexArray(VAO_TEMP1_plus);
    unsigned int VBO_TEMP1_plus;
    glGenBuffers(1, &VBO_TEMP1_plus);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TEMP1_plus);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTEMP1_plus), verticesTEMP1_plus, GL_STATIC_DRAW);
    unsigned int EBO_TEMP1_plus;
    glGenBuffers(1, &EBO_TEMP1_plus);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_TEMP1_plus);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // TEMP1 2.RED -
    float verticesTEMP1_minus[] = {
        // X      Y      S    T
        0.12f,  0.48f,  0.0f, 0.0f, // Donji levi ugao
        0.32f,  0.48f,  1.0f, 0.0f, // Donji desni ugao
        0.12f,  0.58f,  0.0f, 1.0f, // Gornji levi ugao
        0.322f, 0.58f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_TEMP1_minus;
    glGenVertexArrays(1, &VAO_TEMP1_minus);
    glBindVertexArray(VAO_TEMP1_minus);
    unsigned int VBO_TEMP1_minus;
    glGenBuffers(1, &VBO_TEMP1_minus);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TEMP1_minus);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTEMP1_minus), verticesTEMP1_minus, GL_STATIC_DRAW);
    unsigned int EBO_TEMP1_minus;
    glGenBuffers(1, &EBO_TEMP1_minus);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_TEMP1_minus);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // TEMP2 2.RED +
    float verticesTEMP2_plus[] = {
        // X      Y      S    T
        -0.4f,  0.23f,  0.0f, 0.0f, // Donji levi ugao
        -0.2f,  0.23f,  1.0f, 0.0f, // Donji desni ugao
        -0.4f,  0.33f,  0.0f, 1.0f, // Gornji levi ugao
        -0.2f,  0.33f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_TEMP2_plus;
    glGenVertexArrays(1, &VAO_TEMP2_plus);
    glBindVertexArray(VAO_TEMP2_plus);
    unsigned int VBO_TEMP2_plus;
    glGenBuffers(1, &VBO_TEMP2_plus);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TEMP2_plus);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTEMP2_plus), verticesTEMP2_plus, GL_STATIC_DRAW);
    unsigned int EBO_TEMP2_plus;
    glGenBuffers(1, &EBO_TEMP2_plus);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_TEMP2_plus);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // TEMP2 2.RED -
    float verticesTEMP2_minus[] = {
        // X      Y      S    T
        0.12f,  0.23f,  0.0f, 0.0f, // Donji levi ugao
        0.32f,  0.23f,  1.0f, 0.0f, // Donji desni ugao
        0.12f,  0.33f,  0.0f, 1.0f, // Gornji levi ugao
        0.322f, 0.33f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_TEMP2_minus;
    glGenVertexArrays(1, &VAO_TEMP2_minus);
    glBindVertexArray(VAO_TEMP2_minus);
    unsigned int VBO_TEMP2_minus;
    glGenBuffers(1, &VBO_TEMP2_minus);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TEMP2_minus);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTEMP2_minus), verticesTEMP2_minus, GL_STATIC_DRAW);
    unsigned int EBO_TEMP2_minus;
    glGenBuffers(1, &EBO_TEMP2_minus);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_TEMP2_minus);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // TEMP3 2.RED +
    float verticesTEMP3_plus[] = {
        // X      Y      S    T
        -0.4f,  -0.02f,  0.0f, 0.0f, // Donji levi ugao
        -0.2f,  -0.02f,  1.0f, 0.0f, // Donji desni ugao
        -0.4f,   0.08f,  0.0f, 1.0f, // Gornji levi ugao
        -0.2f,   0.08f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_TEMP3_plus;
    glGenVertexArrays(1, &VAO_TEMP3_plus);
    glBindVertexArray(VAO_TEMP3_plus);
    unsigned int VBO_TEMP3_plus;
    glGenBuffers(1, &VBO_TEMP3_plus);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TEMP3_plus);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTEMP3_plus), verticesTEMP3_plus, GL_STATIC_DRAW);
    unsigned int EBO_TEMP3_plus;
    glGenBuffers(1, &EBO_TEMP3_plus);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_TEMP3_plus);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // TEMP3 2.RED -
    float verticesTEMP3_minus[] = {
        // X      Y      S    T
        0.12f,  -0.02f,  0.0f, 0.0f, // Donji levi ugao
        0.32f,  -0.02f,  1.0f, 0.0f, // Donji desni ugao
        0.12f,   0.08f,  0.0f, 1.0f, // Gornji levi ugao
        0.322f,  0.08f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_TEMP3_minus;
    glGenVertexArrays(1, &VAO_TEMP3_minus);
    glBindVertexArray(VAO_TEMP3_minus);
    unsigned int VBO_TEMP3_minus;
    glGenBuffers(1, &VBO_TEMP3_minus);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TEMP3_minus);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTEMP3_minus), verticesTEMP3_minus, GL_STATIC_DRAW);
    unsigned int EBO_TEMP3_minus;
    glGenBuffers(1, &EBO_TEMP3_minus);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_TEMP3_minus);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // TEMP4 2.RED +
    float verticesTEMP4_plus[] = {
        // X      Y      S    T
        -0.4f,  -0.27f,  0.0f, 0.0f, // Donji levi ugao
        -0.2f,  -0.27f,  1.0f, 0.0f, // Donji desni ugao
        -0.4f,  -0.17f,  0.0f, 1.0f, // Gornji levi ugao
        -0.2f,  -0.17f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_TEMP4_plus;
    glGenVertexArrays(1, &VAO_TEMP4_plus);
    glBindVertexArray(VAO_TEMP4_plus);
    unsigned int VBO_TEMP4_plus;
    glGenBuffers(1, &VBO_TEMP4_plus);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TEMP4_plus);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTEMP4_plus), verticesTEMP4_plus, GL_STATIC_DRAW);
    unsigned int EBO_TEMP4_plus;
    glGenBuffers(1, &EBO_TEMP4_plus);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_TEMP4_plus);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // TEMP4 2.RED -
    float verticesTEMP4_minus[] = {
        // X      Y      S    T
        0.12f,  -0.27f,  0.0f, 0.0f, // Donji levi ugao
        0.32f,  -0.27f,  1.0f, 0.0f, // Donji desni ugao
        0.12f,  -0.17f,  0.0f, 1.0f, // Gornji levi ugao
        0.322f, -0.17f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_TEMP4_minus;
    glGenVertexArrays(1, &VAO_TEMP4_minus);
    glBindVertexArray(VAO_TEMP4_minus);
    unsigned int VBO_TEMP4_minus;
    glGenBuffers(1, &VBO_TEMP4_minus);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TEMP4_minus);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTEMP4_minus), verticesTEMP4_minus, GL_STATIC_DRAW);
    unsigned int EBO_TEMP4_minus;
    glGenBuffers(1, &EBO_TEMP4_minus);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_TEMP4_minus);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // brightness 2.RED +
    float verticesB_plus[] = {
        // X      Y      S    T
        -0.4f,  -0.52f,  0.0f, 0.0f, // Donji levi ugao
        -0.2f,  -0.52f,  1.0f, 0.0f, // Donji desni ugao
        -0.4f,  -0.42f,  0.0f, 1.0f, // Gornji levi ugao
        -0.2f,  -0.42f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_B_plus;
    glGenVertexArrays(1, &VAO_B_plus);
    glBindVertexArray(VAO_B_plus);
    unsigned int VBO_B_plus;
    glGenBuffers(1, &VBO_B_plus);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_B_plus);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesB_plus), verticesB_plus, GL_STATIC_DRAW);
    unsigned int EBO_B_plus;
    glGenBuffers(1, &EBO_B_plus);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_B_plus);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // brightness 2.RED -
    float verticesB_minus[] = {
        // X      Y      S    T
        0.12f,  -0.52f,  0.0f, 0.0f, // Donji levi ugao
        0.32f,  -0.52f,  1.0f, 0.0f, // Donji desni ugao
        0.12f,  -0.42f,  0.0f, 1.0f, // Gornji levi ugao
        0.322f, -0.42f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_B_minus;
    glGenVertexArrays(1, &VAO_B_minus);
    glBindVertexArray(VAO_B_minus);
    unsigned int VBO_B_minus;
    glGenBuffers(1, &VBO_B_minus);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_B_minus);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesB_minus), verticesB_minus, GL_STATIC_DRAW);
    unsigned int EBO_B_minus;
    glGenBuffers(1, &EBO_B_minus);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_B_minus);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    //brightness c1
    float verticesB_c1[] = {
        // X      Y      S    T
        -0.11f,  -0.5f,  0.0f, 0.0f, // Donji levi ugao
        -0.06f,  -0.5f,  1.0f, 0.0f, // Donji desni ugao
        -0.11f,  -0.45f,  0.0f, 1.0f, // Gornji levi ugao
        -0.06f,  -0.45f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_B_c1;
    glGenVertexArrays(1, &VAO_B_c1);
    glBindVertexArray(VAO_B_c1);
    unsigned int VBO_B_c1;
    glGenBuffers(1, &VBO_B_c1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_B_c1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesB_c1), verticesB_c1, GL_STATIC_DRAW);
    unsigned int EBO_B_c1;
    glGenBuffers(1, &EBO_B_c1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_B_c1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    //brightness .
    float verticesB_DOT[] = {
        // X      Y      S    T
        -0.06f,  -0.5f,  0.0f, 0.0f, // Donji levi ugao
        -0.04f,  -0.5f,  1.0f, 0.0f, // Donji desni ugao
        -0.06f,  -0.49f,  0.0f, 1.0f, // Gornji levi ugao
        -0.04f,  -0.49f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_B_DOT;
    glGenVertexArrays(1, &VAO_B_DOT);
    glBindVertexArray(VAO_B_DOT);
    unsigned int VBO_B_DOT;
    glGenBuffers(1, &VBO_B_DOT);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_B_DOT);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesB_DOT), verticesB_DOT, GL_STATIC_DRAW);
    unsigned int EBO_B_DOT;
    glGenBuffers(1, &EBO_B_DOT);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_B_DOT);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    //brightness c2
    float verticesB_c2[] = {
        // X      Y      S    T
        -0.04f,  -0.5f,  0.0f, 0.0f, // Donji levi ugao
        0.01f,   -0.5f,  1.0f, 0.0f, // Donji desni ugao
        -0.04f,  -0.45f,  0.0f, 1.0f, // Gornji levi ugao
        0.01f,   -0.45f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_B_c2;
    glGenVertexArrays(1, &VAO_B_c2);
    glBindVertexArray(VAO_B_c2);
    unsigned int VBO_B_c2;
    glGenBuffers(1, &VBO_B_c2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_B_c2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesB_c2), verticesB_c2, GL_STATIC_DRAW);
    unsigned int EBO_B_c2;
    glGenBuffers(1, &EBO_B_c2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_B_c2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    //TEMP1 c1
    float verticesTEMP1_c1[] = {
        // X      Y      S    T
        -0.11f,  0.5f,  0.0f, 0.0f, // Donji levi ugao
        -0.06f,  0.5f,  1.0f, 0.0f, // Donji desni ugao
        -0.11f,  0.55f,  0.0f, 1.0f, // Gornji levi ugao
        -0.06f,  0.55f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_TEMP1_c1;
    glGenVertexArrays(1, &VAO_TEMP1_c1);
    glBindVertexArray(VAO_TEMP1_c1);
    unsigned int VBO_TEMP1_c1;
    glGenBuffers(1, &VBO_TEMP1_c1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TEMP1_c1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTEMP1_c1), verticesTEMP1_c1, GL_STATIC_DRAW);
    unsigned int EBO_TEMP1_c1;
    glGenBuffers(1, &EBO_TEMP1_c1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_TEMP1_c1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    //TEMP1 .
    float verticesTEMP1_DOT[] = {
        // X      Y      S    T
        -0.06f,  0.5f,  0.0f, 0.0f, // Donji levi ugao
        -0.04f,  0.5f,  1.0f, 0.0f, // Donji desni ugao
        -0.06f,  0.51f,  0.0f, 1.0f, // Gornji levi ugao
        -0.04f,  0.51f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_TEMP1_DOT;
    glGenVertexArrays(1, &VAO_TEMP1_DOT);
    glBindVertexArray(VAO_TEMP1_DOT);
    unsigned int VBO_TEMP1_DOT;
    glGenBuffers(1, &VBO_TEMP1_DOT);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TEMP1_DOT);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTEMP1_DOT), verticesTEMP1_DOT, GL_STATIC_DRAW);
    unsigned int EBO_TEMP1_DOT;
    glGenBuffers(1, &EBO_TEMP1_DOT);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_TEMP1_DOT);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    //TEMP1 c2
    float verticesTEMP1_c2[] = {
        // X      Y      S    T
        -0.04f,  0.5f,  0.0f, 0.0f, // Donji levi ugao
        0.01f,   0.5f,  1.0f, 0.0f, // Donji desni ugao
        -0.04f,  0.55f,  0.0f, 1.0f, // Gornji levi ugao
        0.01f,   0.55f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_TEMP1_c2;
    glGenVertexArrays(1, &VAO_TEMP1_c2);
    glBindVertexArray(VAO_TEMP1_c2);
    unsigned int VBO_TEMP1_c2;
    glGenBuffers(1, &VBO_TEMP1_c2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TEMP1_c2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTEMP1_c2), verticesTEMP1_c2, GL_STATIC_DRAW);
    unsigned int EBO_TEMP1_c2;
    glGenBuffers(1, &EBO_TEMP1_c2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_TEMP1_c2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    //TEMP2 c1
    float verticesTEMP2_c1[] = {
        // X      Y      S    T
        -0.11f,  0.25f,  0.0f, 0.0f, // Donji levi ugao
        -0.06f,  0.25f,  1.0f, 0.0f, // Donji desni ugao
        -0.11f,  0.30f,  0.0f, 1.0f, // Gornji levi ugao
        -0.06f,  0.30f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_TEMP2_c1;
    glGenVertexArrays(1, &VAO_TEMP2_c1);
    glBindVertexArray(VAO_TEMP2_c1);
    unsigned int VBO_TEMP2_c1;
    glGenBuffers(1, &VBO_TEMP2_c1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TEMP2_c1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTEMP2_c1), verticesTEMP2_c1, GL_STATIC_DRAW);
    unsigned int EBO_TEMP2_c1;
    glGenBuffers(1, &EBO_TEMP2_c1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_TEMP2_c1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    //TEMP2 .
    float verticesTEMP2_DOT[] = {
        // X      Y      S    T
        -0.06f,  0.25f,  0.0f, 0.0f, // Donji levi ugao
        -0.04f,  0.25f,  1.0f, 0.0f, // Donji desni ugao
        -0.06f,  0.26f,  0.0f, 1.0f, // Gornji levi ugao
        -0.04f,  0.26f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_TEMP2_DOT;
    glGenVertexArrays(1, &VAO_TEMP2_DOT);
    glBindVertexArray(VAO_TEMP2_DOT);
    unsigned int VBO_TEMP2_DOT;
    glGenBuffers(1, &VBO_TEMP2_DOT);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TEMP2_DOT);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTEMP2_DOT), verticesTEMP2_DOT, GL_STATIC_DRAW);
    unsigned int EBO_TEMP2_DOT;
    glGenBuffers(1, &EBO_TEMP2_DOT);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_TEMP2_DOT);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    //TEMP2 c2
    float verticesTEMP2_c2[] = {
        // X      Y      S    T
        -0.04f,  0.25f,  0.0f, 0.0f, // Donji levi ugao
        0.01f,   0.25f,  1.0f, 0.0f, // Donji desni ugao
        -0.04f,  0.30f,  0.0f, 1.0f, // Gornji levi ugao
        0.01f,   0.30f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_TEMP2_c2;
    glGenVertexArrays(1, &VAO_TEMP2_c2);
    glBindVertexArray(VAO_TEMP2_c2);
    unsigned int VBO_TEMP2_c2;
    glGenBuffers(1, &VBO_TEMP2_c2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TEMP2_c2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTEMP2_c2), verticesTEMP2_c2, GL_STATIC_DRAW);
    unsigned int EBO_TEMP2_c2;
    glGenBuffers(1, &EBO_TEMP2_c2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_TEMP2_c2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    //TEMP3 VALUE -
    float verticesTEMP3_NEG[] = {
        // X      Y      S    T
        -0.14f,  0.0f,  0.0f, 0.0f, // Donji levi ugao
        -0.11f,  0.0f,  1.0f, 0.0f, // Donji desni ugao
        -0.14f,  0.05f,  0.0f, 1.0f, // Gornji levi ugao
        -0.11f,  0.05f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_TEMP3_NEG;
    glGenVertexArrays(1, &VAO_TEMP3_NEG);
    glBindVertexArray(VAO_TEMP3_NEG);
    unsigned int VBO_TEMP3_NEG;
    glGenBuffers(1, &VBO_TEMP3_NEG);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TEMP3_NEG);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTEMP3_NEG), verticesTEMP3_NEG, GL_STATIC_DRAW);
    unsigned int EBO_TEMP3_NEG;
    glGenBuffers(1, &EBO_TEMP3_NEG);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_TEMP3_NEG);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    //TEMP3 c1
    float verticesTEMP3_c1[] = {
        // X      Y      S    T
        -0.11f,  0.0f,  0.0f, 0.0f, // Donji levi ugao
        -0.06f,  0.0f,  1.0f, 0.0f, // Donji desni ugao
        -0.11f,  0.05f,  0.0f, 1.0f, // Gornji levi ugao
        -0.06f,  0.05f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_TEMP3_c1;
    glGenVertexArrays(1, &VAO_TEMP3_c1);
    glBindVertexArray(VAO_TEMP3_c1);
    unsigned int VBO_TEMP3_c1;
    glGenBuffers(1, &VBO_TEMP3_c1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TEMP3_c1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTEMP3_c1), verticesTEMP3_c1, GL_STATIC_DRAW);
    unsigned int EBO_TEMP3_c1;
    glGenBuffers(1, &EBO_TEMP3_c1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_TEMP3_c1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    //TEMP3 c2
    float verticesTEMP3_c2[] = {
        // X      Y      S    T
        -0.06f,  0.0f,  0.0f, 0.0f, // Donji levi ugao
        -0.01f,  0.0f,  1.0f, 0.0f, // Donji desni ugao
        -0.06f,  0.05f,  0.0f, 1.0f, // Gornji levi ugao
        -0.01f,  0.05f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_TEMP3_c2;
    glGenVertexArrays(1, &VAO_TEMP3_c2);
    glBindVertexArray(VAO_TEMP3_c2);
    unsigned int VBO_TEMP3_c2;
    glGenBuffers(1, &VBO_TEMP3_c2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TEMP3_c2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTEMP3_c2), verticesTEMP3_c2, GL_STATIC_DRAW);
    unsigned int EBO_TEMP3_c2;
    glGenBuffers(1, &EBO_TEMP3_c2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_TEMP3_c2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //TEMP3 .
    float verticesTEMP3_DOT[] = {
        // X      Y      S    T
        -0.01f,  0.0f,  0.0f, 0.0f, // Donji levi ugao
        0.01f,   0.0f,  1.0f, 0.0f, // Donji desni ugao
        -0.01f,  0.01f,  0.0f, 1.0f, // Gornji levi ugao
        0.01f,   0.01f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_TEMP3_DOT;
    glGenVertexArrays(1, &VAO_TEMP3_DOT);
    glBindVertexArray(VAO_TEMP3_DOT);
    unsigned int VBO_TEMP3_DOT;
    glGenBuffers(1, &VBO_TEMP3_DOT);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TEMP3_DOT);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTEMP3_DOT), verticesTEMP3_DOT, GL_STATIC_DRAW);
    unsigned int EBO_TEMP3_DOT;
    glGenBuffers(1, &EBO_TEMP3_DOT);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_TEMP3_DOT);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //TEMP3 c3
    float verticesTEMP3_c3[] = {
        // X      Y      S    T
        0.01f,  0.0f,  0.0f, 0.0f, // Donji levi ugao
        0.06f,  0.0f,  1.0f, 0.0f, // Donji desni ugao
        0.01f,  0.05f,  0.0f, 1.0f, // Gornji levi ugao
        0.06f,  0.05f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_TEMP3_c3;
    glGenVertexArrays(1, &VAO_TEMP3_c3);
    glBindVertexArray(VAO_TEMP3_c3);
    unsigned int VBO_TEMP3_c3;
    glGenBuffers(1, &VBO_TEMP3_c3);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TEMP3_c3);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTEMP3_c3), verticesTEMP3_c3, GL_STATIC_DRAW);
    unsigned int EBO_TEMP3_c3;
    glGenBuffers(1, &EBO_TEMP3_c3);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_TEMP3_c3);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    //TEMP4 VALUE -
    float verticesTEMP4_NEG[] = {
        // X      Y      S    T
        -0.14f,  -0.25f,  0.0f, 0.0f, // Donji levi ugao
        -0.11f,  -0.25f,  1.0f, 0.0f, // Donji desni ugao
        -0.14f,  -0.2f,  0.0f, 1.0f, // Gornji levi ugao
        -0.11f,  -0.2f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_TEMP4_NEG;
    glGenVertexArrays(1, &VAO_TEMP4_NEG);
    glBindVertexArray(VAO_TEMP4_NEG);
    unsigned int VBO_TEMP4_NEG;
    glGenBuffers(1, &VBO_TEMP4_NEG);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TEMP4_NEG);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTEMP4_NEG), verticesTEMP4_NEG, GL_STATIC_DRAW);
    unsigned int EBO_TEMP4_NEG;
    glGenBuffers(1, &EBO_TEMP4_NEG);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_TEMP4_NEG);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    

    //TEMP4 c1
    float verticesTEMP4_c1[] = {
        // X      Y      S    T
        -0.11f,  -0.25f,  0.0f, 0.0f, // Donji levi ugao
        -0.06f,  -0.25f,  1.0f, 0.0f, // Donji desni ugao
        -0.11f,  -0.2f,  0.0f, 1.0f, // Gornji levi ugao
        -0.06f,  -0.2f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_TEMP4_c1;
    glGenVertexArrays(1, &VAO_TEMP4_c1);
    glBindVertexArray(VAO_TEMP4_c1);
    unsigned int VBO_TEMP4_c1;
    glGenBuffers(1, &VBO_TEMP4_c1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TEMP4_c1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTEMP4_c1), verticesTEMP4_c1, GL_STATIC_DRAW);
    unsigned int EBO_TEMP4_c1;
    glGenBuffers(1, &EBO_TEMP4_c1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_TEMP4_c1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    //TEMP4 c2
    float verticesTEMP4_c2[] = {
        // X      Y      S    T
        -0.06f,  -0.25f,  0.0f, 0.0f, // Donji levi ugao
        -0.01f,  -0.25f,  1.0f, 0.0f, // Donji desni ugao
        -0.06f,  -0.2f,  0.0f, 1.0f, // Gornji levi ugao
        -0.01f,  -0.2f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_TEMP4_c2;
    glGenVertexArrays(1, &VAO_TEMP4_c2);
    glBindVertexArray(VAO_TEMP4_c2);
    unsigned int VBO_TEMP4_c2;
    glGenBuffers(1, &VBO_TEMP4_c2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TEMP4_c2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTEMP4_c2), verticesTEMP4_c2, GL_STATIC_DRAW);
    unsigned int EBO_TEMP4_c2;
    glGenBuffers(1, &EBO_TEMP4_c2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_TEMP4_c2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    
    //TEMP4 .
    float verticesTEMP4_DOT[] = {
        // X      Y      S    T
        -0.01f,  -0.25f,  0.0f, 0.0f, // Donji levi ugao
        0.01f,   -0.25f,  1.0f, 0.0f, // Donji desni ugao
        -0.01f,  -0.24f,  0.0f, 1.0f, // Gornji levi ugao
        0.01f,   -0.24f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_TEMP4_DOT;
    glGenVertexArrays(1, &VAO_TEMP4_DOT);
    glBindVertexArray(VAO_TEMP4_DOT);
    unsigned int VBO_TEMP4_DOT;
    glGenBuffers(1, &VBO_TEMP4_DOT);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TEMP4_DOT);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTEMP4_DOT), verticesTEMP4_DOT, GL_STATIC_DRAW);
    unsigned int EBO_TEMP4_DOT;
    glGenBuffers(1, &EBO_TEMP4_DOT);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_TEMP4_DOT);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    
    //TEMP4 c3
    float verticesTEMP4_c3[] = {
        // X      Y      S    T
        0.01f,  -0.25f,  0.0f, 0.0f, // Donji levi ugao
        0.06f,  -0.25f,  1.0f, 0.0f, // Donji desni ugao
        0.01f,  -0.2f,  0.0f, 1.0f, // Gornji levi ugao
        0.06f,  -0.2f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_TEMP4_c3;
    glGenVertexArrays(1, &VAO_TEMP4_c3);
    glBindVertexArray(VAO_TEMP4_c3);
    unsigned int VBO_TEMP4_c3;
    glGenBuffers(1, &VBO_TEMP4_c3);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TEMP4_c3);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTEMP4_c3), verticesTEMP4_c3, GL_STATIC_DRAW);
    unsigned int EBO_TEMP4_c3;
    glGenBuffers(1, &EBO_TEMP4_c3);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_TEMP4_c3);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // TRANSPARENT BUTTON
    float verticesBtn[] = {
        // X      Y      S    T
        -0.3f,  -0.8f,  0.0f, 0.0f, // Donji levi ugao
         0.3f,  -0.8f,  1.0f, 0.0f, // Donji desni ugao
        -0.3f,  -0.7f,  0.0f, 1.0f, // Gornji levi ugao
         0.3f,  -0.7f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_btn;
    glGenVertexArrays(1, &VAO_btn);
    glBindVertexArray(VAO_btn);
    unsigned int VBO_btn;
    glGenBuffers(1, &VBO_btn);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_btn);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesBtn), verticesBtn, GL_STATIC_DRAW);
    unsigned int EBO_btn;
    glGenBuffers(1, &EBO_btn);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_btn);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // AUTOR
    float verticesAuthor[] = {
        // X      Y      S    T
        -0.2f,  -1.0f,  0.0f, 0.0f, // Donji levi ugao
         1.0f,  -1.0f,  1.0f, 0.0f, // Donji desni ugao
        -0.2f,  -0.96f,  0.0f, 1.0f, // Gornji levi ugao
         1.0f,  -0.96f,  1.0f, 1.0f  // Gornji desni ugao
    };
    unsigned int VAO_author;
    glGenVertexArrays(1, &VAO_author);
    glBindVertexArray(VAO_author);
    unsigned int VBO_author;
    glGenBuffers(1, &VBO_author);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_author);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesAuthor), verticesAuthor, GL_STATIC_DRAW);
    unsigned int EBO_author;
    glGenBuffers(1, &EBO_author);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_author);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);



    unsigned textureTemp1 = loadImageToTexture("res/textures/temp1.png");
    glBindTexture(GL_TEXTURE_2D, textureTemp1);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(unifiedShader);
    unsigned uTexLocTemp1 = glGetUniformLocation(unifiedShader, "uTex");
    glUniform1i(uTexLocTemp1, 0);
    glUseProgram(0);

    unsigned textureTemp2 = loadImageToTexture("res/textures/temp2.png");
    glBindTexture(GL_TEXTURE_2D, textureTemp2);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(unifiedShader);
    unsigned uTexLocTemp2 = glGetUniformLocation(unifiedShader, "uTex");
    glUniform1i(uTexLocTemp2, 0);
    glUseProgram(0);

    unsigned textureTemp3 = loadImageToTexture("res/textures/temp3.png");
    glBindTexture(GL_TEXTURE_2D, textureTemp3);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(unifiedShader);
    unsigned uTexLocTemp3 = glGetUniformLocation(unifiedShader, "uTex");
    glUniform1i(uTexLocTemp3, 0);
    glUseProgram(0);

    unsigned textureTemp4 = loadImageToTexture("res/textures/temp4.png");
    glBindTexture(GL_TEXTURE_2D, textureTemp4);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(unifiedShader);
    unsigned uTexLocTemp4 = glGetUniformLocation(unifiedShader, "uTex");
    glUniform1i(uTexLocTemp4, 0);
    glUseProgram(0);

    unsigned textureB = loadImageToTexture("res/textures/brightness.png");
    glBindTexture(GL_TEXTURE_2D, textureB);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(unifiedShader);
    unsigned uTexLocB = glGetUniformLocation(unifiedShader, "uTex");
    glUniform1i(uTexLocB, 0);
    glUseProgram(0);

    unsigned texture0 = loadImageToTexture("res/textures/0.png");
    glBindTexture(GL_TEXTURE_2D, texture0);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(unifiedShader);
    unsigned uTexLoc0 = glGetUniformLocation(unifiedShader, "uTex");
    glUniform1i(uTexLoc0, 0);
    glUseProgram(0);
    unsigned texture1 = loadImageToTexture("res/textures/1.png");
    glBindTexture(GL_TEXTURE_2D, texture1);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(unifiedShader);
    unsigned uTexLoc1= glGetUniformLocation(unifiedShader, "uTex");
    glUniform1i(uTexLoc1, 0);
    glUseProgram(0);
    unsigned texture2 = loadImageToTexture("res/textures/2.png");
    glBindTexture(GL_TEXTURE_2D, texture2);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(unifiedShader);
    unsigned uTexLoc2 = glGetUniformLocation(unifiedShader, "uTex");
    glUniform1i(uTexLoc2, 0);
    glUseProgram(0);
    unsigned texture3 = loadImageToTexture("res/textures/3.png");
    glBindTexture(GL_TEXTURE_2D, texture3);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(unifiedShader);
    unsigned uTexLoc3= glGetUniformLocation(unifiedShader, "uTex");
    glUniform1i(uTexLoc3, 0);
    glUseProgram(0);
    unsigned texture4 = loadImageToTexture("res/textures/4.png");
    glBindTexture(GL_TEXTURE_2D, texture4);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(unifiedShader);
    unsigned uTexLoc4 = glGetUniformLocation(unifiedShader, "uTex");
    glUniform1i(uTexLoc4, 0);
    glUseProgram(0);
    unsigned texture5 = loadImageToTexture("res/textures/5.png");
    glBindTexture(GL_TEXTURE_2D, texture5);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(unifiedShader);
    unsigned uTexLoc5 = glGetUniformLocation(unifiedShader, "uTex");
    glUniform1i(uTexLoc5, 0);
    glUseProgram(0);
    unsigned texture6 = loadImageToTexture("res/textures/6.png");
    glBindTexture(GL_TEXTURE_2D, texture6);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(unifiedShader);
    unsigned uTexLoc6 = glGetUniformLocation(unifiedShader, "uTex");
    glUniform1i(uTexLoc6, 0);
    glUseProgram(0);
    unsigned texture7 = loadImageToTexture("res/textures/7.png");
    glBindTexture(GL_TEXTURE_2D, texture7);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(unifiedShader);
    unsigned uTexLoc7 = glGetUniformLocation(unifiedShader, "uTex");
    glUniform1i(uTexLoc7, 0);
    glUseProgram(0);
    unsigned texture8 = loadImageToTexture("res/textures/8.png");
    glBindTexture(GL_TEXTURE_2D, texture8);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(unifiedShader);
    unsigned uTexLoc8 = glGetUniformLocation(unifiedShader, "uTex");
    glUniform1i(uTexLoc8, 0);
    glUseProgram(0);
    unsigned texture9 = loadImageToTexture("res/textures/9.png");
    glBindTexture(GL_TEXTURE_2D, texture9);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(unifiedShader);
    unsigned uTexLoc9 = glGetUniformLocation(unifiedShader, "uTex");
    glUniform1i(uTexLoc9, 0);
    glUseProgram(0);

    unsigned textureNEG = loadImageToTexture("res/textures/neg.png");
    glBindTexture(GL_TEXTURE_2D, textureNEG);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(unifiedShader);
    unsigned uTexLocNEG = glGetUniformLocation(unifiedShader, "uTex");
    glUniform1i(uTexLocNEG, 0);
    glUseProgram(0);

    unsigned textureDOT = loadImageToTexture("res/textures/dot.png");
    glBindTexture(GL_TEXTURE_2D, textureDOT);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(unifiedShader);
    unsigned uTexLocDOT = glGetUniformLocation(unifiedShader, "uTex");
    glUniform1i(uTexLocDOT, 0);
    glUseProgram(0);

    unsigned textureCOLON = loadImageToTexture("res/textures/colon.png");
    glBindTexture(GL_TEXTURE_2D, textureCOLON);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(unifiedShader);
    unsigned uTexLocCOLON = glGetUniformLocation(unifiedShader, "uTex");
    glUniform1i(uTexLocCOLON, 0);
    glUseProgram(0);

    unsigned texture_plus = loadImageToTexture("res/textures/plus.png");
    glBindTexture(GL_TEXTURE_2D, texture_plus);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(unifiedShader);
    unsigned uTexLoc_plus = glGetUniformLocation(unifiedShader, "uTex");
    glUniform1i(uTexLoc_plus, 0);
    glUseProgram(0);

    unsigned texture_minus = loadImageToTexture("res/textures/minus.png");
    glBindTexture(GL_TEXTURE_2D, texture_minus);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(unifiedShader);
    unsigned uTexLoc_minus = glGetUniformLocation(unifiedShader, "uTex");
    glUniform1i(uTexLoc_minus, 0);
    glUseProgram(0);

    unsigned texture_fridge = loadImageToTexture("res/fridge1.png");
    glBindTexture(GL_TEXTURE_2D, texture_fridge);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(unifiedShader);
    unsigned uTexLoc_fridge = glGetUniformLocation(unifiedShader, "uTex");
    glUniform1i(uTexLoc_fridge, 0);
    glUseProgram(0);

    unsigned texture_freezer = loadImageToTexture("res/fridge2.png");
    glBindTexture(GL_TEXTURE_2D, texture_freezer);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(unifiedShader);
    unsigned uTexLoc_freezer = glGetUniformLocation(unifiedShader, "uTex");
    glUniform1i(uTexLoc_freezer, 0);
    glUseProgram(0);

    unsigned texture_button = loadImageToTexture("res/textures/transparent.png");
    glBindTexture(GL_TEXTURE_2D, texture_button);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(unifiedShader);
    unsigned uTexLoc_button = glGetUniformLocation(unifiedShader, "uTex");
    glUniform1i(uTexLoc_button, 0);
    glUseProgram(0);

    unsigned texture_author = loadImageToTexture("res/textures/autor.png");
    glBindTexture(GL_TEXTURE_2D, texture_author);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(unifiedShader);
    unsigned uTexLoc_author = glGetUniformLocation(unifiedShader, "uTex");
    glUniform1i(uTexLoc_author, 0);
    glUseProgram(0);

    unsigned texture_logo = loadImageToTexture("res/textures/logo.png");
    glBindTexture(GL_TEXTURE_2D, texture_logo);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(logoShader);
    unsigned uTexLoc_logo = glGetUniformLocation(logoShader, "uTex");
    unsigned timeLoc_logo = glGetUniformLocation(logoShader, "time");
    unsigned int isPulsingLoc = glGetUniformLocation(logoShader, "isPulsing");
    unsigned int isKey5Loc = glGetUniformLocation(logoShader, "isKey5");
    unsigned uLogoPosXLoc = glGetUniformLocation(logoShader, "logoPosX");
    unsigned uPosLoc_logo = glGetUniformLocation(logoShader, "uPos");
    glUniform1i(uTexLoc_logo, 0);
    glUseProgram(0);
    
    
    const double frameTime = 1.0 / 60.0; // trajanje svakog frejma
    double lastTime = glfwGetTime(); // vreme kada je program poceo
    double deltaTime = 0.0;

    lastInteractionTime = std::chrono::steady_clock::now();

    while (!glfwWindowShouldClose(window))
    {
        double currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            if (isPulsing2) stopKey2();
            if (moveLogo3) stopKey3();
            if (moveLogo4) stopKey4();
            if (moveLogo5) stopKey5();
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        glfwSetCursorPosCallback(window, cursor_position_callback);

        glfwSetMouseButtonCallback(window, mouse_button_callback);
        
        glClearColor(0.2, 0.2, 0.2, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        if (deltaTime >= frameTime) {
            if (currentMode == GUI) {

                auto currentTimeTransparent = std::chrono::steady_clock::now();
                auto timeSinceLastInteraction = std::chrono::duration_cast<std::chrono::seconds>(currentTimeTransparent - lastInteractionTime);

                if (timeSinceLastInteraction.count() >= 5) {
                    logoTransition = true;
                }

                auto now = std::chrono::system_clock::now();
                std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
                std::tm* now_tm = std::localtime(&now_time_t);

                // izdvajanje sata, minuta i sekundi
                int hours = now_tm->tm_hour;
                int minutes = now_tm->tm_min;
                int seconds = now_tm->tm_sec;

                //HOUR 0
                int hour0 = hours / 10;
                glUseProgram(unifiedShader);
                glActiveTexture(GL_TEXTURE0);
                if (hour0 == 0) {
                    glBindTexture(GL_TEXTURE_2D, texture0);
                }
                else if (hour0 == 1) {
                    glBindTexture(GL_TEXTURE_2D, texture1);
                }
                else {
                    glBindTexture(GL_TEXTURE_2D, texture2);
                }
                glBindVertexArray(VAO_HOUR0);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //HOUR 1
                int hour1 = hours % 10;
                glUseProgram(unifiedShader);
                glActiveTexture(GL_TEXTURE0);
                if (hour1 == 0) {
                    glBindTexture(GL_TEXTURE_2D, texture0);
                }
                else if (hour1 == 1) {
                    glBindTexture(GL_TEXTURE_2D, texture1);
                }
                else if (hour1 == 2) {
                    glBindTexture(GL_TEXTURE_2D, texture2);
                }
                else if (hour1 == 3) {
                    glBindTexture(GL_TEXTURE_2D, texture3);
                }
                else if (hour1 == 4) {
                    glBindTexture(GL_TEXTURE_2D, texture4);
                }
                else if (hour1 == 5) {
                    glBindTexture(GL_TEXTURE_2D, texture5);
                }
                else if (hour1 == 6) {
                    glBindTexture(GL_TEXTURE_2D, texture6);
                }
                else if (hour1 == 7) {
                    glBindTexture(GL_TEXTURE_2D, texture7);
                }
                else if (hour1 == 8) {
                    glBindTexture(GL_TEXTURE_2D, texture8);
                }
                else {
                    glBindTexture(GL_TEXTURE_2D, texture9);
                }
                glBindVertexArray(VAO_HOUR1);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //: 1
                glUseProgram(unifiedShader);
                glBindVertexArray(VAO_HM1);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, textureCOLON);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //MINUTE 0
                int minute0 = minutes / 10;
                glUseProgram(unifiedShader);
                glActiveTexture(GL_TEXTURE0);
                if (minute0 == 0) {
                    glBindTexture(GL_TEXTURE_2D, texture0);
                }
                else if (minute0 == 1) {
                    glBindTexture(GL_TEXTURE_2D, texture1);
                }
                else if (minute0 == 2) {
                    glBindTexture(GL_TEXTURE_2D, texture2);
                }
                else if (minute0 == 3) {
                    glBindTexture(GL_TEXTURE_2D, texture3);
                }
                else if (minute0 == 4) {
                    glBindTexture(GL_TEXTURE_2D, texture4);
                }
                else {
                    glBindTexture(GL_TEXTURE_2D, texture5);
                }
                glBindVertexArray(VAO_MINUTE0);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //MINUTE 1
                int minute1 = minutes % 10;
                glUseProgram(unifiedShader);
                glActiveTexture(GL_TEXTURE0);
                if (minute1 == 0) {
                    glBindTexture(GL_TEXTURE_2D, texture0);
                }
                else if (minute1 == 1) {
                    glBindTexture(GL_TEXTURE_2D, texture1);
                }
                else if (minute1 == 2) {
                    glBindTexture(GL_TEXTURE_2D, texture2);
                }
                else if (minute1 == 3) {
                    glBindTexture(GL_TEXTURE_2D, texture3);
                }
                else if (minute1 == 4) {
                    glBindTexture(GL_TEXTURE_2D, texture4);
                }
                else if (minute1 == 5) {
                    glBindTexture(GL_TEXTURE_2D, texture5);
                }
                else if (minute1 == 6) {
                    glBindTexture(GL_TEXTURE_2D, texture6);
                }
                else if (minute1 == 7) {
                    glBindTexture(GL_TEXTURE_2D, texture7);
                }
                else if (minute1 == 8) {
                    glBindTexture(GL_TEXTURE_2D, texture8);
                }
                else {
                    glBindTexture(GL_TEXTURE_2D, texture9);
                }
                glBindVertexArray(VAO_MINUTE1);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //: 2
                glUseProgram(unifiedShader);
                glBindVertexArray(VAO_HM2);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, textureCOLON);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //SECOND 0
                int second0 = seconds / 10;
                glUseProgram(unifiedShader);
                glActiveTexture(GL_TEXTURE0);
                if (second0 == 0) {
                    glBindTexture(GL_TEXTURE_2D, texture0);
                }
                else if (second0 == 1) {
                    glBindTexture(GL_TEXTURE_2D, texture1);
                }
                else if (second0 == 2) {
                    glBindTexture(GL_TEXTURE_2D, texture2);
                }
                else if (second0 == 3) {
                    glBindTexture(GL_TEXTURE_2D, texture3);
                }
                else if (second0 == 4) {
                    glBindTexture(GL_TEXTURE_2D, texture4);
                }
                else {
                    glBindTexture(GL_TEXTURE_2D, texture5);
                }
                glBindVertexArray(VAO_SECOND0);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //SECOND 1
                int second1 = seconds % 10;
                glUseProgram(unifiedShader);
                glActiveTexture(GL_TEXTURE0);
                if (second1 == 0) {
                    glBindTexture(GL_TEXTURE_2D, texture0);
                }
                else if (second1 == 1) {
                    glBindTexture(GL_TEXTURE_2D, texture1);
                }
                else if (second1 == 2) {
                    glBindTexture(GL_TEXTURE_2D, texture2);
                }
                else if (second1 == 3) {
                    glBindTexture(GL_TEXTURE_2D, texture3);
                }
                else if (second1 == 4) {
                    glBindTexture(GL_TEXTURE_2D, texture4);
                }
                else if (second1 == 5) {
                    glBindTexture(GL_TEXTURE_2D, texture5);
                }
                else if (second1 == 6) {
                    glBindTexture(GL_TEXTURE_2D, texture6);
                }
                else if (second1 == 7) {
                    glBindTexture(GL_TEXTURE_2D, texture7);
                }
                else if (second1 == 8) {
                    glBindTexture(GL_TEXTURE_2D, texture8);
                }
                else {
                    glBindTexture(GL_TEXTURE_2D, texture9);
                }
                glBindVertexArray(VAO_SECOND1);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //fridge current temp - temp1
                glUseProgram(unifiedShader);
                glBindVertexArray(VAO_TEMP1);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, textureTemp1);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //fridge desired temp - temp2
                glUseProgram(unifiedShader);
                glBindVertexArray(VAO_TEMP2);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, textureTemp2);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //freezer current temp - temp3
                glUseProgram(unifiedShader);
                glBindVertexArray(VAO_TEMP3);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, textureTemp3);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //freezer desired temp - temp4
                glUseProgram(unifiedShader);
                glBindVertexArray(VAO_TEMP4);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, textureTemp4);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //freezer desired temp - temp4
                glUseProgram(unifiedShader);
                glBindVertexArray(VAO_B);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, textureB);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //TEMP1 2. RED +
                glUseProgram(unifiedShader);
                glBindVertexArray(VAO_TEMP1_plus);
                glActiveTexture(GL_TEXTURE0); 
                glBindTexture(GL_TEXTURE_2D, texture_plus);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //TEMP1 2. RED -
                glUseProgram(unifiedShader);
                glBindVertexArray(VAO_TEMP1_minus);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture_minus);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //TEMP2 2. RED +
                glUseProgram(unifiedShader);
                glBindVertexArray(VAO_TEMP2_plus);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture_plus);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //TEMP2 2. RED -
                glUseProgram(unifiedShader);
                glBindVertexArray(VAO_TEMP2_minus);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture_minus);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //TEMP3 2. RED +
                glUseProgram(unifiedShader);
                glBindVertexArray(VAO_TEMP3_plus);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture_plus);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //TEMP3 2. RED -
                glUseProgram(unifiedShader);
                glBindVertexArray(VAO_TEMP3_minus);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture_minus);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //TEMP4 2. RED +
                glUseProgram(unifiedShader);
                glBindVertexArray(VAO_TEMP4_plus);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture_plus);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //TEMP4 2. RED -
                glUseProgram(unifiedShader);
                glBindVertexArray(VAO_TEMP4_minus);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture_minus);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //brightness 2. RED +
                glUseProgram(unifiedShader);
                glBindVertexArray(VAO_B_plus);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture_plus);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //brightness 2. RED -
                glUseProgram(unifiedShader);
                glBindVertexArray(VAO_B_minus);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture_minus);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //brightness c1
                int b_c1 = static_cast<int>(abs(brightness)) % 10;
                glUseProgram(unifiedShader);
                glActiveTexture(GL_TEXTURE0);
                if (b_c1 == 0) {
                    glBindTexture(GL_TEXTURE_2D, texture0);
                }
                else if (b_c1 == 1) {
                    glBindTexture(GL_TEXTURE_2D, texture1);
                }
                else if (b_c1 == 2) {
                    glBindTexture(GL_TEXTURE_2D, texture2);
                }
                else if (b_c1 == 3) {
                    glBindTexture(GL_TEXTURE_2D, texture3);
                }
                else if (b_c1 == 4) {
                    glBindTexture(GL_TEXTURE_2D, texture4);
                }
                else {
                    glBindTexture(GL_TEXTURE_2D, texture5);
                }
                glBindVertexArray(VAO_B_c1);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //TEMP1 .
                glUseProgram(unifiedShader);
                glBindVertexArray(VAO_B_DOT);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, textureDOT);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //TEMP1 c2
                int b_c2_Helper = static_cast<int>(abs(brightness) * 10);
                int b_c2 = b_c2_Helper % 10;
                glUseProgram(unifiedShader);
                glActiveTexture(GL_TEXTURE0);
                if (b_c2 == 0) {
                    glBindTexture(GL_TEXTURE_2D, texture0);
                }
                else if (b_c2 == 1) {
                    glBindTexture(GL_TEXTURE_2D, texture1);
                }
                else if (b_c2 == 2) {
                    glBindTexture(GL_TEXTURE_2D, texture2);
                }
                else if (b_c2 == 3) {
                    glBindTexture(GL_TEXTURE_2D, texture3);
                }
                else if (b_c2 == 4) {
                    glBindTexture(GL_TEXTURE_2D, texture4);
                }
                else if (b_c2 == 5) {
                    glBindTexture(GL_TEXTURE_2D, texture5);
                }
                else if (b_c2 == 6) {
                    glBindTexture(GL_TEXTURE_2D, texture6);
                }
                else if (b_c2 == 7) {
                    glBindTexture(GL_TEXTURE_2D, texture7);
                }
                else if (b_c2 == 8) {
                    glBindTexture(GL_TEXTURE_2D, texture8);
                }
                else {
                    glBindTexture(GL_TEXTURE_2D, texture9);
                }
                glBindVertexArray(VAO_B_c2);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //TEMP1 c1
                int t1_c1 = static_cast<int>(abs(fridgeTemp)) % 10;
                glUseProgram(unifiedShader);
                glActiveTexture(GL_TEXTURE0);
                if (t1_c1 == 0) {
                    glBindTexture(GL_TEXTURE_2D, texture0);
                }
                else if (t1_c1 == 1) {
                    glBindTexture(GL_TEXTURE_2D, texture1);
                }
                else if (t1_c1 == 2) {
                    glBindTexture(GL_TEXTURE_2D, texture2);
                }
                else if (t1_c1 == 3) {
                    glBindTexture(GL_TEXTURE_2D, texture3);
                }
                else if (t1_c1 == 4) {
                    glBindTexture(GL_TEXTURE_2D, texture4);
                }
                else if (t1_c1 == 5) {
                    glBindTexture(GL_TEXTURE_2D, texture5);
                }
                else if (t1_c1 == 6) {
                    glBindTexture(GL_TEXTURE_2D, texture6);
                }
                else {
                    glBindTexture(GL_TEXTURE_2D, texture7);
                }
                glBindVertexArray(VAO_TEMP1_c1);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //TEMP1 .
                glUseProgram(unifiedShader);
                glBindVertexArray(VAO_TEMP1_DOT);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, textureDOT);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //TEMP1 c2
                int t1_c2_Helper = static_cast<int>(abs(fridgeTemp) * 10);
                int t1_c2 = t1_c2_Helper % 10;
                glUseProgram(unifiedShader);
                glActiveTexture(GL_TEXTURE0);
                if (t1_c2 == 0) {
                    glBindTexture(GL_TEXTURE_2D, texture0);
                }
                else if (t1_c2 == 1) {
                    glBindTexture(GL_TEXTURE_2D, texture1);
                }
                else if (t1_c2 == 2) {
                    glBindTexture(GL_TEXTURE_2D, texture2);
                }
                else if (t1_c2 == 3) {
                    glBindTexture(GL_TEXTURE_2D, texture3);
                }
                else if (t1_c2 == 4) {
                    glBindTexture(GL_TEXTURE_2D, texture4);
                }
                else if (t1_c2 == 5) {
                    glBindTexture(GL_TEXTURE_2D, texture5);
                }
                else if (t1_c2 == 6) {
                    glBindTexture(GL_TEXTURE_2D, texture6);
                }
                else if (t1_c2 == 7) {
                    glBindTexture(GL_TEXTURE_2D, texture7);
                }
                else if (t1_c2 == 8) {
                    glBindTexture(GL_TEXTURE_2D, texture8);
                }
                else {
                    glBindTexture(GL_TEXTURE_2D, texture9);
                }
                glBindVertexArray(VAO_TEMP1_c2);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //TEMP2 c1
                int t2_c1 = static_cast<int>(abs(desiredFridgeTemp)) % 10;
                glUseProgram(unifiedShader);
                glActiveTexture(GL_TEXTURE0);
                if (t2_c1 == 0) {
                    glBindTexture(GL_TEXTURE_2D, texture0);
                }
                else if (t2_c1 == 1) {
                    glBindTexture(GL_TEXTURE_2D, texture1);
                }
                else if (t2_c1 == 2) {
                    glBindTexture(GL_TEXTURE_2D, texture2);
                }
                else if (t2_c1 == 3) {
                    glBindTexture(GL_TEXTURE_2D, texture3);
                }
                else if (t2_c1 == 4) {
                    glBindTexture(GL_TEXTURE_2D, texture4);
                }
                else if (t2_c1 == 5) {
                    glBindTexture(GL_TEXTURE_2D, texture5);
                }
                else if (t2_c1 == 6) {
                    glBindTexture(GL_TEXTURE_2D, texture6);
                }
                else {
                    glBindTexture(GL_TEXTURE_2D, texture7);
                }
                glBindVertexArray(VAO_TEMP2_c1);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //TEMP2 .
                glUseProgram(unifiedShader);
                glBindVertexArray(VAO_TEMP2_DOT);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, textureDOT);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //TEMP2 c2
                int t2_c2_Helper = static_cast<int>(abs(desiredFridgeTemp) * 10);
                int t2_c2 = t2_c2_Helper % 10;
                glUseProgram(unifiedShader);
                glActiveTexture(GL_TEXTURE0);
                if (t2_c2 == 0) {
                    glBindTexture(GL_TEXTURE_2D, texture0);
                }
                else if (t2_c2 == 1) {
                    glBindTexture(GL_TEXTURE_2D, texture1);
                }
                else if (t2_c2 == 2) {
                    glBindTexture(GL_TEXTURE_2D, texture2);
                }
                else if (t2_c2 == 3) {
                    glBindTexture(GL_TEXTURE_2D, texture3);
                }
                else if (t2_c2 == 4) {
                    glBindTexture(GL_TEXTURE_2D, texture4);
                }
                else if (t2_c2 == 5) {
                    glBindTexture(GL_TEXTURE_2D, texture5);
                }
                else if (t2_c2 == 6) {
                    glBindTexture(GL_TEXTURE_2D, texture6);
                }
                else if (t2_c2 == 7) {
                    glBindTexture(GL_TEXTURE_2D, texture7);
                }
                else if (t2_c2 == 8) {
                    glBindTexture(GL_TEXTURE_2D, texture8);
                }
                else {
                    glBindTexture(GL_TEXTURE_2D, texture9);
                }
                glBindVertexArray(VAO_TEMP2_c2);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //TEMP3 VALUE -
                glUseProgram(unifiedShader);
                glBindVertexArray(VAO_TEMP3_NEG);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, textureNEG);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //TEMP3 c1
                int t3_c1 = static_cast<int>(abs(freezerTemp)) / 10;
                glUseProgram(unifiedShader);
                glActiveTexture(GL_TEXTURE0);
                if (t3_c1 == 1) {
                    glBindTexture(GL_TEXTURE_2D, texture1);
                }
                else if (t3_c1 == 2) {
                    glBindTexture(GL_TEXTURE_2D, texture2);
                }
                else if (t3_c1 == 3) {
                    glBindTexture(GL_TEXTURE_2D, texture3);
                }
                else {
                    glBindTexture(GL_TEXTURE_2D, texture4);
                }
                glBindVertexArray(VAO_TEMP3_c1);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //TEMP3 c2
                int t3_c2 = static_cast<int>(abs(freezerTemp)) % 10;
                glUseProgram(unifiedShader);
                glActiveTexture(GL_TEXTURE0);
                if (t3_c2 == 0) {
                    glBindTexture(GL_TEXTURE_2D, texture0);
                }
                else if (t3_c2 == 1) {
                    glBindTexture(GL_TEXTURE_2D, texture1);
                }
                else if (t3_c2 == 2) {
                    glBindTexture(GL_TEXTURE_2D, texture2);
                }
                else if (t3_c2 == 3) {
                    glBindTexture(GL_TEXTURE_2D, texture3);
                }
                else if (t3_c2 == 4) {
                    glBindTexture(GL_TEXTURE_2D, texture4);
                }
                else if (t3_c2 == 5) {
                    glBindTexture(GL_TEXTURE_2D, texture5);
                }
                else if (t3_c2 == 6) {
                    glBindTexture(GL_TEXTURE_2D, texture6);
                }
                else if (t3_c2 == 7) {
                    glBindTexture(GL_TEXTURE_2D, texture7);
                }
                else if (t3_c2 == 8) {
                    glBindTexture(GL_TEXTURE_2D, texture8);
                }
                else {
                    glBindTexture(GL_TEXTURE_2D, texture9);
                }
                glBindVertexArray(VAO_TEMP3_c2);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //TEMP3 .
                glUseProgram(unifiedShader);
                glBindVertexArray(VAO_TEMP3_DOT);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, textureDOT);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //TEMP3 c3
                int t3_c3_Helper = static_cast<int>(abs(freezerTemp) * 10);
                int t3_c3 = t3_c3_Helper % 10;
                glUseProgram(unifiedShader);
                glActiveTexture(GL_TEXTURE0);
                if (t3_c3 == 0) {
                    glBindTexture(GL_TEXTURE_2D, texture0);
                }
                else if (t3_c3 == 1) {
                    glBindTexture(GL_TEXTURE_2D, texture1);
                }
                else if (t3_c3 == 2) {
                    glBindTexture(GL_TEXTURE_2D, texture2);
                }
                else if (t3_c3 == 3) {
                    glBindTexture(GL_TEXTURE_2D, texture3);
                }
                else if (t3_c3 == 4) {
                    glBindTexture(GL_TEXTURE_2D, texture4);
                }
                else if (t3_c3 == 5) {
                    glBindTexture(GL_TEXTURE_2D, texture5);
                }
                else if (t3_c3 == 6) {
                    glBindTexture(GL_TEXTURE_2D, texture6);
                }
                else if (t3_c3 == 7) {
                    glBindTexture(GL_TEXTURE_2D, texture7);
                }
                else if (t3_c3 == 8) {
                    glBindTexture(GL_TEXTURE_2D, texture8);
                }
                else {
                    glBindTexture(GL_TEXTURE_2D, texture9);
                }
                glBindVertexArray(VAO_TEMP3_c3);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);
                
                //TEMP4 VALUE -
                glUseProgram(unifiedShader);
                glBindVertexArray(VAO_TEMP4_NEG);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, textureNEG);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //TEMP4 c1
                int t4_c1 = static_cast<int>(abs(desiredFreezerTemp)) / 10;
                glUseProgram(unifiedShader);
                glActiveTexture(GL_TEXTURE0);
                if (t4_c1 == 1) {
                    glBindTexture(GL_TEXTURE_2D, texture1);
                }
                else if (t4_c1 == 2) {
                    glBindTexture(GL_TEXTURE_2D, texture2);
                }
                else if (t4_c1 == 3) {
                    glBindTexture(GL_TEXTURE_2D, texture3);
                }
                else {
                    glBindTexture(GL_TEXTURE_2D, texture4);
                }
                glBindVertexArray(VAO_TEMP4_c1);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //TEMP4 c2
                int t4_c2 = static_cast<int>(abs(desiredFreezerTemp)) % 10;
                glUseProgram(unifiedShader);
                glActiveTexture(GL_TEXTURE0);
                if (t4_c2 == 0) {
                    glBindTexture(GL_TEXTURE_2D, texture0);
                }
                else if (t4_c2 == 1) {
                    glBindTexture(GL_TEXTURE_2D, texture1);
                }
                else if (t4_c2 == 2) {
                    glBindTexture(GL_TEXTURE_2D, texture2);
                }
                else if (t4_c2 == 3) {
                    glBindTexture(GL_TEXTURE_2D, texture3);
                }
                else if (t4_c2 == 4) {
                    glBindTexture(GL_TEXTURE_2D, texture4);
                }
                else if (t4_c2 == 5) {
                    glBindTexture(GL_TEXTURE_2D, texture5);
                }
                else if (t4_c2 == 6) {
                    glBindTexture(GL_TEXTURE_2D, texture6);
                }
                else if (t4_c2 == 7) {
                    glBindTexture(GL_TEXTURE_2D, texture7);
                }
                else if (t4_c2 == 8) {
                    glBindTexture(GL_TEXTURE_2D, texture8);
                }
                else {
                    glBindTexture(GL_TEXTURE_2D, texture9);
                }
                glBindVertexArray(VAO_TEMP4_c2);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //TEMP4 .
                glUseProgram(unifiedShader);
                glBindVertexArray(VAO_TEMP4_DOT);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, textureDOT);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //TEMP4 c3
                int t4_c3_Helper = static_cast<int>(abs(desiredFreezerTemp) * 10);
                int t4_c3 = t4_c3_Helper % 10;
                glUseProgram(unifiedShader);
                glActiveTexture(GL_TEXTURE0);
                if (t4_c3 == 0) {
                    glBindTexture(GL_TEXTURE_2D, texture0);
                }
                else if (t4_c3 == 1) {
                    glBindTexture(GL_TEXTURE_2D, texture1);
                }
                else if (t4_c3 == 2) {
                    glBindTexture(GL_TEXTURE_2D, texture2);
                }
                else if (t4_c3 == 3) {
                    glBindTexture(GL_TEXTURE_2D, texture3);
                }
                else if (t4_c3 == 4) {
                    glBindTexture(GL_TEXTURE_2D, texture4);
                }
                else if (t4_c3 == 5) {
                    glBindTexture(GL_TEXTURE_2D, texture5);
                }
                else if (t4_c3 == 6) {
                    glBindTexture(GL_TEXTURE_2D, texture6);
                }
                else if (t4_c3 == 7) {
                    glBindTexture(GL_TEXTURE_2D, texture7);
                }
                else if (t4_c3 == 8) {
                    glBindTexture(GL_TEXTURE_2D, texture8);
                }
                else {
                    glBindTexture(GL_TEXTURE_2D, texture9);
                }
                glBindVertexArray(VAO_TEMP4_c3);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //TRANSPARENT MODE BUTTON
                glUseProgram(unifiedShader);
                glBindVertexArray(VAO_btn);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture_button);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                float alpha = scaleBrightness();
                glUseProgram(brightnessShader);
                glUniform1f(glGetUniformLocation(brightnessShader, "uAlpha"), alpha);
                glBindVertexArray(VAO_B_tint);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                if (logoTransition) {
                    if (colorIntensity < 1.0f) {
                        colorIntensity += increment;
                    }
                    glUseProgram(logoTransitionShader);
                    int colorIntensityLoc = glGetUniformLocation(logoTransitionShader, "colorIntensity");
                    glUniform1f(colorIntensityLoc, colorIntensity);
                    glBindVertexArray(VAO_LOGO_display);
                    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                    glBindVertexArray(0);
                    glUseProgram(0);
                    if (colorIntensity >= 1.0f) {
                        logoTransition = false;
                        std::cout << "Prelazi u LOGO rezim.\n";
                        currentMode = LOGO;
                    }

                }
            
            }
            else if (currentMode == TRANSPARENT) {
                auto currentTimeTransparent = std::chrono::steady_clock::now();
                auto timeSinceLastInteraction = std::chrono::duration_cast<std::chrono::seconds>(currentTimeTransparent - lastInteractionTime);

                if (timeSinceLastInteraction.count() >= 5) {
                    logoTransition = true;
                }
            
                float blueTintFridge = calculateBlueTint(fridgeTemp, 0.0f, 7.0f);
                float blueTintFreezer = calculateBlueTint(freezerTemp, -38.0f, -18.0f);


                //TRANSPARENT FRIDGE
                glUseProgram(unifiedShader);
                glBindVertexArray(VAO_Fridge);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture_fridge);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //BLUE TINT FRIDGE
                glUseProgram(blueTintShader);
                int blueTintFridgeLocation = glGetUniformLocation(blueTintShader, "blueTint");
                glUniform1f(blueTintFridgeLocation, blueTintFridge);
                glBindVertexArray(VAO_BlueTint_Fridge);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture_fridge);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //TRANSPARENT FREEZER
                glUseProgram(unifiedShader);
                glBindVertexArray(VAO_Freezer);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture_freezer);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                //BLUE TINT FREEZER
                glUseProgram(blueTintShader);
                int blueTintFreezerLocation = glGetUniformLocation(blueTintShader, "blueTint");
                glUniform1f(blueTintFreezerLocation, blueTintFreezer);
                glBindVertexArray(VAO_BlueTint_Freezer);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture_freezer);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                float alpha = scaleBrightness();
                glUseProgram(brightnessShader);
                glUniform1f(glGetUniformLocation(brightnessShader, "uAlpha"), alpha);
                glBindVertexArray(VAO_B_tint);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);
                glUseProgram(0);

                if (logoTransition) {
                    if (colorIntensity < 1.0f) {
                        colorIntensity += increment;
                    }
                    glUseProgram(logoTransitionShader);
                    int colorIntensityLoc = glGetUniformLocation(logoTransitionShader, "colorIntensity");
                    glUniform1f(colorIntensityLoc, colorIntensity);
                    glBindVertexArray(VAO_LOGO_display);
                    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                    glBindVertexArray(0);
                    glUseProgram(0);
                    if (colorIntensity >= 1.0f) {
                        logoTransition = false;
                        std::cout << "Prelazi u LOGO rezim.\n";
                        currentMode = LOGO;
                    }

                }
            }
            else if (currentMode == LOGO) {
                // BLACK DISPLAY
                colorIntensity = 0.0f;
                glUseProgram(logoTransitionShader);
                int colorIntensityLoc = glGetUniformLocation(logoTransitionShader, "colorIntensity");
                glUniform1f(colorIntensityLoc, 1.0);
                glBindVertexArray(VAO_LOGO_display);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                glBindVertexArray(0);
                glUseProgram(0);

                if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
                {
                    if (isPulsing2) stopKey2();
                    if (moveLogo3) stopKey3();
                    if (moveLogo4) stopKey4();
                    if (moveLogo5) stopKey5();
                    logo_x = 0.0f;
                    logo_y = 0.0f;
                }

                if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
                {
                    if (isPulsing2) stopKey2();
                    if (moveLogo4) stopKey4();
                    if (moveLogo5) stopKey5();
                    moveLogo3 = true;
                }

                if (moveLogo3) {
                    double logo_desno = logo_x + 0.2f;
                    if (logo_desno <= 0.9f) {
                        logo_x += 0.01f;
                    }
                    else {
                        logo_x = -0.7f;
                    }
                }

                if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
                {
                    if (isPulsing2) stopKey2();
                    if (moveLogo3) stopKey3();
                    if (moveLogo5) stopKey5();
                    moveLogo4 = true;
                }

                if (moveLogo4) {
                    float radius = 0.25f;
                    float speed = 0.05f;
                    angle4 -= speed;
                    logo_x = radius * cos(angle4);
                    logo_y = radius * sin(angle4);

                }

                if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
                {
                    if (moveLogo3) stopKey3();
                    if (moveLogo4) stopKey4();
                    if (moveLogo5) stopKey5();
                    isPulsing2 = true;
                }

                if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
                {
                    if (moveLogo3) stopKey3();
                    if (moveLogo4) stopKey4();
                    if (isPulsing2) stopKey2();
                    moveLogo5 = true;
                }
                
                // LOGO
                glUseProgram(logoShader);
                glBindVertexArray(VAO_LOGO);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture_logo);
                glUniform2f(uPosLoc_logo, logo_x, logo_y);

                if (isPulsing2)
                {
                    float currentTime = glfwGetTime();
                    glUniform1f(timeLoc_logo, currentTime);
                }
                else
                {
                    glUniform1f(timeLoc_logo, 0.0f);
                }

                // LOGO nastavak na osnovu pulsiranja i tastera 5
                glUniform1i(isPulsingLoc, isPulsing2);
                glUniform1i(isKey5Loc, moveLogo5);

                if (moveLogo5) {
                    if (logo_x + 0.2f >= 0.9f || logo_x - 0.2f <= -0.9f)
                    {
                        direction *= -1;
                    }
                    logo_x += direction * 0.01f;
                }

                glUniform1f(uLogoPosXLoc, logo_x);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);

            
            }

            //AUTOR
            glUseProgram(unifiedShader);
            glBindVertexArray(VAO_author);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_author);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindVertexArray(0);
            glUseProgram(0);
        

            lastTime = currentTime;
        }
        else {
            // spavaj kako bi se postigao ciljani FPS
            double sleepTime = frameTime - deltaTime;
            if (sleepTime > 0) {
                 std::this_thread::sleep_for(std::chrono::milliseconds((int)(sleepTime * 1000)));
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glDeleteBuffers(1, &VBO_TEMP1);
    glDeleteVertexArrays(1, &VAO_TEMP1);

    glDeleteBuffers(1, &VBO_TEMP2);
    glDeleteVertexArrays(1, &VAO_TEMP2);

    glDeleteBuffers(1, &VBO_TEMP3);
    glDeleteVertexArrays(1, &VAO_TEMP3);

    glDeleteBuffers(1, &VBO_TEMP4);
    glDeleteVertexArrays(1, &VAO_TEMP4);

    glDeleteBuffers(1, &VBO_B);
    glDeleteVertexArrays(1, &VAO_B);

    glDeleteBuffers(1, &VBO_B_tint);
    glDeleteVertexArrays(1, &VAO_B_tint);

    glDeleteBuffers(1, &VBO_TEMP1_plus);
    glDeleteVertexArrays(1, &VAO_TEMP1_plus);

    glDeleteBuffers(1, &VBO_TEMP1_minus);
    glDeleteVertexArrays(1, &VAO_TEMP1_minus);

    glDeleteBuffers(1, &VBO_TEMP2_plus);
    glDeleteVertexArrays(1, &VAO_TEMP2_plus);

    glDeleteBuffers(1, &VBO_TEMP2_minus);
    glDeleteVertexArrays(1, &VAO_TEMP2_minus);

    glDeleteBuffers(1, &VBO_TEMP3_plus);
    glDeleteVertexArrays(1, &VAO_TEMP3_plus);

    glDeleteBuffers(1, &VBO_TEMP3_minus);
    glDeleteVertexArrays(1, &VAO_TEMP3_minus);

    glDeleteBuffers(1, &VBO_TEMP4_plus);
    glDeleteVertexArrays(1, &VAO_TEMP4_plus);

    glDeleteBuffers(1, &VBO_TEMP4_minus);
    glDeleteVertexArrays(1, &VAO_TEMP4_minus);

    glDeleteBuffers(1, &VBO_B_plus);
    glDeleteVertexArrays(1, &VAO_B_plus);

    glDeleteBuffers(1, &VBO_B_minus);
    glDeleteVertexArrays(1, &VAO_B_minus);

    glDeleteBuffers(1, &VBO_TEMP3_NEG);
    glDeleteVertexArrays(1, &VAO_TEMP3_NEG);

    glDeleteBuffers(1, &VBO_TEMP3_c1);
    glDeleteVertexArrays(1, &VAO_TEMP3_c1);

    glDeleteBuffers(1, &VBO_TEMP3_c2);
    glDeleteVertexArrays(1, &VAO_TEMP3_c2);

    glDeleteBuffers(1, &VBO_TEMP3_DOT);
    glDeleteVertexArrays(1, &VAO_TEMP3_DOT);

    glDeleteBuffers(1, &VBO_TEMP3_c3);
    glDeleteVertexArrays(1, &VAO_TEMP3_c3);

    glDeleteBuffers(1, &VBO_TEMP4_NEG);
    glDeleteVertexArrays(1, &VAO_TEMP4_NEG);

    glDeleteBuffers(1, &VBO_TEMP4_c1);
    glDeleteVertexArrays(1, &VAO_TEMP4_c1);

    glDeleteBuffers(1, &VBO_TEMP4_c2);
    glDeleteVertexArrays(1, &VAO_TEMP4_c2);

    glDeleteBuffers(1, &VBO_TEMP4_DOT);
    glDeleteVertexArrays(1, &VAO_TEMP4_DOT);

    glDeleteBuffers(1, &VBO_TEMP4_c3);
    glDeleteVertexArrays(1, &VAO_TEMP4_c3);

    glDeleteBuffers(1, &VBO_TEMP1_c1);
    glDeleteVertexArrays(1, &VAO_TEMP1_c1);

    glDeleteBuffers(1, &VBO_TEMP1_c2);
    glDeleteVertexArrays(1, &VAO_TEMP1_c2);

    glDeleteBuffers(1, &VBO_TEMP1_DOT);
    glDeleteVertexArrays(1, &VAO_TEMP1_DOT);

    glDeleteBuffers(1, &VBO_TEMP2_c1);
    glDeleteVertexArrays(1, &VAO_TEMP2_c1);

    glDeleteBuffers(1, &VBO_TEMP2_c2);
    glDeleteVertexArrays(1, &VAO_TEMP2_c2);

    glDeleteBuffers(1, &VBO_TEMP2_DOT);
    glDeleteVertexArrays(1, &VAO_TEMP2_DOT);

    glDeleteBuffers(1, &VBO_B_c1);
    glDeleteVertexArrays(1, &VAO_B_c1);

    glDeleteBuffers(1, &VBO_B_c2);
    glDeleteVertexArrays(1, &VAO_B_c2);

    glDeleteBuffers(1, &VBO_B_DOT);
    glDeleteVertexArrays(1, &VAO_B_DOT);

    glDeleteBuffers(1, &VBO_Fridge);
    glDeleteVertexArrays(1, &VAO_Fridge);

    glDeleteBuffers(1, &VBO_BlueTint_Fridge);
    glDeleteVertexArrays(1, &VAO_BlueTint_Fridge);

    glDeleteBuffers(1, &VBO_Freezer);
    glDeleteVertexArrays(1, &VAO_Freezer);

    glDeleteBuffers(1, &VBO_BlueTint_Freezer);
    glDeleteVertexArrays(1, &VAO_BlueTint_Freezer);

    glDeleteBuffers(1, &VBO_btn);
    glDeleteVertexArrays(1, &VAO_btn);

    glDeleteBuffers(1, &VBO_author);
    glDeleteVertexArrays(1, &VAO_author);

    glDeleteBuffers(1, &VBO_LOGO_display);
    glDeleteVertexArrays(1, &VAO_LOGO_display);

    glDeleteBuffers(1, &VBO_LOGO);
    glDeleteVertexArrays(1, &VAO_LOGO);

    glDeleteTextures(1, &textureTemp1);
    glDeleteTextures(1, &textureTemp2);
    glDeleteTextures(1, &textureTemp3);
    glDeleteTextures(1, &textureTemp4);
    glDeleteTextures(1, &textureB);
    glDeleteTextures(1, &texture0);
    glDeleteTextures(1, &texture1);
    glDeleteTextures(1, &texture2);
    glDeleteTextures(1, &texture3);
    glDeleteTextures(1, &texture4);
    glDeleteTextures(1, &texture5);
    glDeleteTextures(1, &texture6);
    glDeleteTextures(1, &texture7);
    glDeleteTextures(1, &texture8);
    glDeleteTextures(1, &texture9);
    glDeleteTextures(1, &textureNEG);
    glDeleteTextures(1, &textureDOT);
    glDeleteTextures(1, &texture_plus);
    glDeleteTextures(1, &texture_minus);
    glDeleteTextures(1, &texture_fridge);
    glDeleteTextures(1, &texture_freezer);
    glDeleteTextures(1, &texture_button);
    glDeleteTextures(1, &texture_author);
    glDeleteTextures(1, &texture_logo);
    
    
    glDeleteProgram(unifiedShader);
    glDeleteProgram(blueTintShader);
    glDeleteProgram(logoTransitionShader);
    glDeleteProgram(logoShader);

    glfwTerminate();
    return 0;
}


void stopKey2() {
    isPulsing2 = false;
}

void stopKey3() {
    moveLogo3 = false;
    logo_x, logo_y = 0.0f;
}

void stopKey4() {
    moveLogo4 = false;
    logo_x, logo_y, angle4 = 0.0f;
}

void stopKey5() {
    moveLogo5 = false;
    logo_x, logo_y = 0.0f;
}

float calculateBlueTint(float temperature, float minTemp, float maxTemp) {
    float normalized = (temperature - minTemp) / (maxTemp - minTemp);
    return 1.0f - normalized;
}

float scaleBrightness() {
    // opseg ulaznog brightness-a
    float old_min = 0.0f;
    float old_max = 5.0f;

    // opseg izlaznog brightness-a
    float new_min = 0.5f;
    float new_max = 0.0f;

    // skaliranje vrednosti
    float scaled_value = new_min + (new_max - new_min) * ((brightness - old_min) / (old_max - old_min));
    return scaled_value;
}


unsigned int compileShader(GLenum type, const char* source)
{
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
     const char* sourceCode = temp.c_str();

    int shader = glCreateShader(type);
    
    int success;
    char infoLog[512];
    glShaderSource(shader, 1, &sourceCode, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
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
    
    unsigned int program;
    unsigned int vertexShader;
    unsigned int fragmentShader;

    program = glCreateProgram();

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource);
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource);

    
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);
    glValidateProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        std::cout << "Objedinjeni sejder ima gresku! Greska: \n";
        std::cout << infoLog << std::endl;
    }

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
        case 2: InternalFormat = GL_RG; break;
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