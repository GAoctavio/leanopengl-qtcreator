#include <iostream>
#include <string>
#define WORKING_DIR "/home/octavio/random/OpenGL/"
#define WORKING_DIR_FILE(F) (WORKING_DIR F)
#define ABS(X) (((X)<0)? -(X): (X))

extern "C"{
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include <SOIL/SOIL.h>
}

// Other includes
#include "shader.h"
#include "camera.h"
#include "model.h"
//glm

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Properties
GLuint screenWidth = 800, screenHeight = 450;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Do_Movement();

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
GLboolean bump=false;

glm::vec3 lightPos(2.3f, -1.6f, -3.0f);

// The MAIN function, from here we start our application and run our Game loop
int main()
{
    // Init GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", nullptr, nullptr); // Windowed
    glfwMakeContextCurrent(window);

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Options
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Initialize GLEW to setup the OpenGL Function pointers
    glewExperimental = GL_TRUE;
    glewInit();

    // Define the viewport dimensions
    glViewport(0, 0, screenWidth, screenHeight);

    // Setup some OpenGL options
    glEnable(GL_DEPTH_TEST);

    // Setup and compile our shaders
    Shader shader(WORKING_DIR_FILE("lighting_shader.vert"),WORKING_DIR_FILE("lighting_shader.frag"));
    Shader lampShader(WORKING_DIR_FILE("lamp_shader.vert"),WORKING_DIR_FILE("lamp_shader.frag"));

    // Load models
    Model ourModel((GLchar*)WORKING_DIR_FILE("Nanosuit/nanosuit.obj"));
    Model lightModel((GLchar*)WORKING_DIR_FILE("Lightbulb/energy_saver-cycles.obj"));

    // Draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Game loop
    unsigned int i=0;
    GLfloat sum=0;
    while(!glfwWindowShouldClose(window))
    {
        // Set frame time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        sum+=deltaTime;
        if(i==100){
            std::cout<<1.0/((sum)/i)<<std::endl<<"bump: "<<(bump?"true":"false")<<std::endl;
            sum=0;
            i=0;
        }
        ++i;

        // Check and call events
        glfwPollEvents();
        Do_Movement();

        // Clear the colorbuffer
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        //glClearColor(0.05f, 0.7f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.Use();   // <-- Don't forget this one!
            // Transformation matrices
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)screenWidth/(float)screenHeight, 0.1f, 100.0f);
            glm::mat4 view = camera.GetViewMatrix();
            glm::mat4 projXview = projection*view;
            glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projXview"), 1, GL_FALSE, glm::value_ptr(projXview));
            // Draw the loaded model
            glm::mat4 model;
            model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // Translate it down a bit so it's at the center of the scene
            model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// It's a bit too big for our scene, so scale it down
            glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glUniform3f(glGetUniformLocation(shader.Program,"viewPos"),camera.Position.x,camera.Position.y,camera.Position.z);

            glUniform3f(glGetUniformLocation(shader.Program,"dirLight.direction"),0.0f,0.0f,-1.0f);
            glUniform3f(glGetUniformLocation(shader.Program, "dirLight.ambient"), 0.5f, 0.5f, 0.5f);
            glUniform3f(glGetUniformLocation(shader.Program, "dirLight.diffuse"), 0.8f, 0.8f, 0.8f);
            glUniform3f(glGetUniformLocation(shader.Program, "dirLight.specular"), 1.0f, 1.0f, 1.0f);

            glUniform3f(glGetUniformLocation(shader.Program, "pointLight[0].position"),lightPos.x,lightPos.y,lightPos.z);
            glUniform3f(glGetUniformLocation(shader.Program, "pointLight[0].ambient"), 0.05f, 0.05f, 0.05f);
            glUniform3f(glGetUniformLocation(shader.Program, "pointLight[0].diffuse"), 1.0f, 1.0f, 1.0f);
            glUniform3f(glGetUniformLocation(shader.Program, "pointLight[0].specular"), 1.0f, 1.0f, 1.0f);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLight[0].constant"), 1.0f);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLight[0].linear"), 0.009);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLight[0].quadratic"), 0.0032);

            glUniform3f(glGetUniformLocation(shader.Program, "pointLight[1].position"),-1.7f, 0.9f, 1.0f);
            glUniform3f(glGetUniformLocation(shader.Program, "pointLight[1].ambient"), 0.05f, 0.05f, 0.05f);
            glUniform3f(glGetUniformLocation(shader.Program, "pointLight[1].diffuse"), 1.0f, 1.0f, 1.0f);
            glUniform3f(glGetUniformLocation(shader.Program, "pointLight[1].specular"), 1.0f, 1.0f, 1.0f);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLight[1].constant"), 1.0f);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLight[1].linear"), 0.009);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLight[1].quadratic"), 0.0032);

            // SpotLight
            glUniform3f(glGetUniformLocation(shader.Program, "spotLight.position"), camera.Position.x, camera.Position.y, camera.Position.z);
            glUniform3f(glGetUniformLocation(shader.Program, "spotLight.direction"), camera.Front.x, camera.Front.y, camera.Front.z);
            glUniform3f(glGetUniformLocation(shader.Program, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
            glUniform3f(glGetUniformLocation(shader.Program, "spotLight.diffuse"), 1.0f, 1.0f, 1.0f);
            glUniform3f(glGetUniformLocation(shader.Program, "spotLight.specular"), 1.0f, 1.0f, 1.0f);
            glUniform1f(glGetUniformLocation(shader.Program, "spotLight.constant"), 1.0f);
            glUniform1f(glGetUniformLocation(shader.Program, "spotLight.linear"), 0.09);
            glUniform1f(glGetUniformLocation(shader.Program, "spotLight.quadratic"), 0.032);
            glUniform1f(glGetUniformLocation(shader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
            glUniform1f(glGetUniformLocation(shader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(17.5f)));

            glUniform1i(glGetUniformLocation(shader.Program,"bump"),bump);
            ourModel.Draw(shader);
/*
            model = glm::translate(glm::mat4(),glm::vec3(2.3f, -1.6f, 0.0f));
            glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
            boxModel.Draw(shader);
*/

        lampShader.Use();
            glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "projXview"), 1, GL_FALSE, glm::value_ptr(projXview));

            model=glm::translate(glm::mat4(),lightPos);
            model=glm::scale(model,glm::vec3(0.1f));
            model=glm::scale(model,glm::vec3(0.1f));
            glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

            lightModel.Draw(lampShader);

            model=glm::translate(glm::mat4(),glm::vec3(-1.7f, 0.9f, 1.0f));
            model=glm::scale(model,glm::vec3(0.1f));
            model=glm::scale(model,glm::vec3(0.1f));
            glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
            lightModel.Draw(lampShader);

        // Swap the buffers
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

// Moves/alters the camera positions based on user input
void Do_Movement()
{
    // Camera controls
    if(keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if(keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if(keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if(keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if(keys[GLFW_KEY_SPACE])
        bump=!bump;
    if(keys[GLFW_KEY_UP])
        lightPos.z-=0.1f;
    if(keys[GLFW_KEY_DOWN])
        lightPos.z+=0.1f;
    if(keys[GLFW_KEY_LEFT])
        lightPos.x-=0.1f;
    if(keys[GLFW_KEY_RIGHT])
        lightPos.x+=0.1f;
    if(keys[GLFW_KEY_PAGE_UP])
        lightPos.y+=0.1f;
    if(keys[GLFW_KEY_PAGE_DOWN])
        lightPos.y-=0.1f;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if(action == GLFW_PRESS)
        keys[key] = true;
    else if(action == GLFW_RELEASE)
        keys[key] = false;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}


