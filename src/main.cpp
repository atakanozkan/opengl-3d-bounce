#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "sphere.h"
#include "cube.h"
#include "model.h"
#include "room.h"

int currentObjectType = 1;
bool wireframeMode = false;
bool setObjectLeftTop = false;
bool set3DmodeOn = false;

float fov = glm::radians(45.0f);
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

/**
    The user needs to set the paths for the shaders and bunny model correctly.
 */
const char* vertexShaderPath = "src/vertex_shader.glsl";
const char* fragmentShaderPath = "src/fragment_shader.glsl";
const char* bunnyModelPath = "model/bunny.off";

std::vector<glm::vec4> colors = {
    glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), // Red
    glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), // Green
    glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), // Blue
    glm::vec4(0.0f, 1.0f, 1.0f, 1.0f), // Cyan
    glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), // White
    glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), // Magenta
    glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), // Yellow
    glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)  // Black
};

int currentColorIndex = 0;
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 11.0f);
glm::vec3 objectPosition = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 objectVelocity = glm::vec3(0.12f, 0.08f, 0.0f);
float objectVelocityZ = -0.1f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void updateObjectPosition(glm::vec3 min, glm::vec3 max, float objectSize);
void moveObjectToLeftTop(glm::vec3 min, glm::vec3 max, float objectSize);

/**
    This function is the main entry point of the application.
    Initializes GLFW and configures it for OpenGL.
    It creates the window and sets up GLAD to handle OpenGL function pointers.
    It creates the depth testing for 3D rendering and sets the functionalities of receving input in window.
    Before main loop, it creates shader programs and set models for rendering. Also it sets up the camera and projection matrices.
    In the main loop, we handle the movement of object and current type of the object (sphere,cube and bunny).
    Also, we set the toggling wireframe mode according to user input and change the color of the object.
    After the mechanics, user interaction and rendering the objects, it swaps the buffers.
    In the last part, it terminates the program.
 */
int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "COMP410 HW1", NULL, NULL);
    
    float aspectRatio = (float)SCR_WIDTH / (float)SCR_HEIGHT;
    
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    glEnable(GL_DEPTH_TEST);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    
    Shader ourShader(vertexShaderPath, fragmentShaderPath);
    Sphere sphere;
    Cube cube;
    Model bunnyModel(bunnyModelPath);
    Room room;
    
    glm::mat4 projection = glm::perspective(fov, aspectRatio, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(
        cameraPos,
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
    
    moveObjectToLeftTop(room.getMinBounds(),room.getMaxBounds(),1.0f);
    
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        
        updateObjectPosition(room.getMinBounds(),room.getMaxBounds(),1.0f);
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glm::mat4 roomModel = glm::mat4(1.0f);
        
        ourShader.use();
        ourShader.setMat4("view", view);
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("model", roomModel);
        ourShader.setInt("useUniformColor", GL_FALSE);
        room.render();
    
        if(setObjectLeftTop){
            moveObjectToLeftTop(room.getMinBounds(),room.getMaxBounds(),1.0f);
            setObjectLeftTop = false;
        }
        
       if (wireframeMode) {
           glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
       } else {
           glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
       }
        
        glm::mat4 model = glm::translate(glm::mat4(1.0f), objectPosition);
        
        
        if(currentObjectType == 2){
            model = glm::rotate(model, 40.02f, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(0.03f));
            ourShader.setMat4("model", model);
            ourShader.setVec4("chosenColor", colors[currentColorIndex]);
            ourShader.setInt("useUniformColor", GL_TRUE);
            bunnyModel.render();
        }
       else if (currentObjectType == 1) {
           model = glm::scale(model, glm::vec3(0.5f));
           ourShader.setMat4("model", model);
           ourShader.setVec4("chosenColor", colors[currentColorIndex]);
           ourShader.setInt("useUniformColor", GL_TRUE);
           sphere.render();
       } 
       else {
           model = glm::scale(model, glm::vec3(0.5f));
           ourShader.setMat4("model", model);
           ourShader.setVec4("chosenColor", colors[currentColorIndex]);
           ourShader.setInt("useUniformColor", GL_TRUE);
           cube.render();
       }

       glfwSwapBuffers(window);
       glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

/**
    This function sets the viewport size and handles window resizing
 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

/**
    This function process the input from user for keyboard and mouse inputs
 */
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

/**
    This funciton handles the keyboard inputs to handle the events.
    Key Events:
    - When user presses key "C", it changes the color index by increasing according to size of color vector.
    - When user presses key "H", it prints the control instructions in the terminal.
    - When user presses key "I", it sets the boolean value "setObjectLeftTop" to true for noticing the main loop to trigger the function "moveObjectToLeftTop"
    - When user presses the key "M", it activates the wire frame mode.
    - When user presses the key "D", the program sets the value "set3DmodeOn" to true, then assigns the velocity of z axis to handle 3D movement.
    - When user presses the key "Q", the program terminates.
 */
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) {
            case GLFW_KEY_C:
                currentColorIndex = (currentColorIndex + 1) % colors.size();
                break;
            case GLFW_KEY_H:
                std::cout << "CONTROLS :\n"
                          << "c - Change the color of current object\n"
                          << "m - Enable wireframe (i.e., as lines) or solid mode\n"
                          << "Left Mouse Click - Change the type of the object (Sphere, Cube or Bunny)\n"
                          << "i - Initialize object position to top-left corner (On turkish keyboard press 'ı')\n"
                          << "h - Display this help message\n"
                          << "d - Set 3D Movement (ON / OFF)\n"
                          << "q - Quit the program\n";
                break;
            case GLFW_KEY_I:
                setObjectLeftTop = true;
                break;
            case GLFW_KEY_M:
                wireframeMode = !wireframeMode;
                break;
            case GLFW_KEY_D:
                set3DmodeOn = !set3DmodeOn;
                if (set3DmodeOn) {
                    objectVelocity.z = objectVelocityZ;
                } else {
                    objectVelocity.z = 0.0f;
                }
                break;
            case GLFW_KEY_Q:
                glfwSetWindowShouldClose(window, true);
                break;
        }
    }
}

/**
 This function handles mouse button inputs to switch between object types (sphere, bunny, cube)
 */
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        currentObjectType = (currentObjectType + 1) % 3;
    }
}

/**
    This function updates the position of the object based on velocity and collision with room boundaries
    In main loop, it calls the function with min and max parameter which are called from room's constant min and max boundaries.
    Also, objectSize parameter decides the offset value to set the collision boundaries for better viual experience.
    If there is a collision, function change the velocity on the specific axis to opposite value.
 */
void updateObjectPosition(glm::vec3 min, glm::vec3 max, float objectSize) {
    float offset = objectSize * (0.75f);

    glm::vec3 minBounds = min + glm::vec3(offset);
    glm::vec3 maxBounds = max - glm::vec3(offset);
    
    objectPosition += objectVelocity;

    if (objectPosition.x <= minBounds.x || objectPosition.x >= maxBounds.x) {
        objectVelocity.x = -objectVelocity.x;
        objectPosition.x = glm::clamp(objectPosition.x, minBounds.x, maxBounds.x);
    }

    if (objectPosition.y <= minBounds.y || objectPosition.y >= maxBounds.y) {
        objectVelocity.y = -objectVelocity.y;
        objectPosition.y = glm::clamp(objectPosition.y, minBounds.y, maxBounds.y);
    }

    if (objectPosition.z <= minBounds.z || objectPosition.z >= maxBounds.z) {
        objectVelocity.z = -objectVelocity.z;
        objectPosition.z = glm::clamp(objectPosition.z, minBounds.z, maxBounds.z);
    }
}

/**
    This function resets the object's position to the top-left corner of the room by calculating the position with min and max bounds.
    In main we assign the paramaters min and max according to room's constant max and min boundaries.
 */
void moveObjectToLeftTop(glm::vec3 min, glm::vec3 max, float objectSize) {
    glm::vec3 minBounds = min;
    glm::vec3 maxBounds = max;

    glm::vec3 initialPosition = glm::vec3(minBounds.x, maxBounds.y - objectSize, minBounds.z);

    objectPosition = initialPosition;
}

