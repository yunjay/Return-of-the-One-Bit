#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>

#include "ShaderLoader.h"
#include "Model.h"

using std::cout;
// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// User
glm::vec3 lowColor(0.0,0.0,0.0);
glm::vec3 highColor(1.0,1.0,1.0);

int main()
{
    // glfw: init and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Return of The One Bit", NULL, NULL);
    if (window == NULL)
    {
        cout << "Failed to create window.\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: loads all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    //z buffer
    glEnable(GL_DEPTH_TEST);

    //IMGui init    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430");

    glClearColor(30.0 / 255, 30.0 / 255, 30.0 / 255, 0.0); //background

    //Load Models
    Model bunny("./models/");
    Model brain("./models/");

    //Load Shaders
    GLuint original = loadShader("./shaders/vertex.vs", "./shaders/original.fs");
    /*
    GLuint threshold = loadShader("./shaders/vertex.vs", "./shaders/.fs");
    GLuint random = loadShader("./shaders/vertex.vs", "./shaders/.fs");
    GLuint bayer4 = loadShader("./shaders/vertex.vs", "./shaders/.fs");
    GLuint bayer16 = loadShader("./shaders/vertex.vs", "./shaders/.fs");
    GLuint bayer64 = loadShader("./shaders/vertex.vs", "./shaders/.fs");
    GLuint halftone = loadShader("./shaders/vertex.vs", "./shaders/.fs");
    GLuint voidAndCluster = loadShader("./shaders/vertex.vs", "./shaders/.fs");
    */


    GLuint* currentShader = &original;

    //view
    glm::vec3 cameraPos = glm::vec3(0, 0, 1);
    //light settings
    glm::vec3 lightPos = glm::vec3(-1, 1, 1);


    //render loop
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        //IMGui new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //IMGui window
        ImGui::Begin("Return of The One Bit");

        /*
        ImGui::Checkbox("Exaggerated Shading", &xOn);
        ImGui::SliderFloat("Rotate X", &xDegrees, 0.0f, 360.0f);
        ImGui::SliderFloat("Rotate Y", &yDegrees, 0.0f, 360.0f);
        ImGui::SliderFloat("Model Size", &modelSize, 0.05f, 500.0f);
        //ImGui::SliderFloat("Brightness", &diffuse, 0.0f, 2.0f);
        ImGui::SliderInt("Number of Smoothing Scales", &scales, 1, 19);
        ImGui::SliderFloat("Contribution factor of ki", &contributionScale, -5.0f, 5.0f);
        ImGui::SliderFloat("Light by scale clamp coefficient", &clampCoef, 1.0f, 1000.0f);
        ImGui::SliderFloat("Ambient", &ambient, 0.0f, 1.0f);
        */
        
        ImGui::End();

        glUseProgram(*currentShader);

        //Uniforms
        glUniform3f(glGetUniformLocation(*currentShader, "light.position"), lightPos.x, lightPos.y, lightPos.z);
        

        //opengl matrice transforms are applied from the right side. (last first)
        glm::mat4 model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(0, -0.4, -1.0f));
        model = glm::translate(model, (-1.0f) * cen);
        model = glm::scale(model, glm::vec3(modelSize, modelSize, modelSize));
        model = glm::rotate(model, glm::radians(yDegrees), glm::vec3(0, 1, 0));
        model = glm::rotate(model, glm::radians(xDegrees), glm::vec3(1, 0, 0));
        glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
        glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        glUniformMatrix4fv(glGetUniformLocation(*currentShader, "model"), 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(*currentShader, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(*currentShader, "projection"), 1, GL_FALSE, &projection[0][0]);

        //printShader(bunny, contribution);

        bunny.render(*currentShader);
        /*
        glUseProgram(principalDirections);
        glUniform1f(glGetUniformLocation(principalDirections,"magnitude"), 0.001*feature);
        glUniformMatrix4fv(glGetUniformLocation(principalDirections, "model"), 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(principalDirections, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(principalDirections, "projection"), 1, GL_FALSE, &projection[0][0]);
        */

        //bunny.render(principalDirections);



        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();

    }




    // Delete ImGUI instances
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // glfw: terminate, clearing all previously allocated GLFW resources.=
    glfwTerminate();
    return 0;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

