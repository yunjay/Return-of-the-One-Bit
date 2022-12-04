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
//const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_WIDTH = 600;
//const unsigned int SCR_HEIGHT = 800;
const unsigned int SCR_HEIGHT = 400;
float cameraSpeed = 2.0f;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

//camera
glm::vec3 cameraFront = glm::vec3(0.0, 0.0, -1.0);
glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);
glm::vec3 cameraPos = glm::vec3(0.0, 1.0f, 5.0);
glm::vec3 cameraLeft = glm::normalize(glm::cross(cameraUp,cameraFront));
GLfloat yaw = -90.0f;
GLfloat pitch = 0.0f;


// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// User parameters
glm::vec3 lowColor(51.0f/255.0f, 51.0f/255.0f, 23.0f/255.0f);
glm::vec3 highColor(229.0f/255.0f, 1.0f, 1.0f);
float lightDegrees=0.0f;
bool autoRotate = false;
float pixelThreshold = 0.5f;


void processInput(GLFWwindow* window);

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
    Model floor("./models/floor.obj");
    Model bunny("./models/stanford-bunny.obj");
    Model lucy("./models/lucy.obj");
    Model dragon("./models/xyzrgb_dragon.obj");
    Model teapot("./models/teapot.obj");
    Model boat("./models/Boat.obj");
    Model wall("./models/wall.obj");

    lucy.setPosition(glm::vec3(-2.5f, 0.2f, 2.0f) - lucy.scale * lucy.minBoxPoint);
    dragon.setPosition(glm::vec3(-4.0f, 0.0f, 0.0f)- dragon.scale * dragon.minBoxPoint);
    bunny.setPosition(glm::vec3(0.0f, 0.0f, 3.0f)-bunny.scale * bunny.minBoxPoint); 
    boat.setPosition(glm::vec3(5.0f, 0.0f, 2.0f)-boat.scale * boat.minBoxPoint);
    teapot.setPosition(glm::vec3(2.0f, 0.0f, 0.0f)-teapot.scale * teapot.minBoxPoint);
    
    bunny.setScale(1.5f*bunny.scale);
    dragon.setScale(1.5f*dragon.scale);
    floor.setScale(1.0f);
    wall.setScale(1.0f);
    floor.diffuseScale = 0.2f;
    wall.diffuseScale = 0.1f;

    std::vector<Model*> models;
    models.push_back(&floor);
    models.push_back(&lucy);
    models.push_back(&dragon);
    models.push_back(&bunny);
    models.push_back(&boat);
    models.push_back(&teapot);
    models.push_back(&wall);

    //Load Shaders
    
    GLuint bayer64 = loadShader("./shaders/vertex.vs", "./shaders/bayer64.fs");
    GLuint bayer16 = loadShader("./shaders/vertex.vs", "./shaders/bayer16.fs");
    GLuint bayer4 = loadShader("./shaders/vertex.vs", "./shaders/bayer4.fs");
    GLuint halftone = loadShader("./shaders/vertex.vs", "./shaders/halftone.fs");
    //GLuint voidAndCluster = loadShader("./shaders/vertex.vs", "./shaders/voidAndCluster.fs");
    GLuint random = loadShader("./shaders/vertex.vs", "./shaders/random.fs");
    GLuint threshold = loadShader("./shaders/vertex.vs", "./shaders/threshold.fs");
    GLuint original = loadShader("./shaders/vertex.vs", "./shaders/original.fs");
    
    GLuint *shaders[] = { &bayer64, &bayer16, &bayer4, &halftone, /*&voidAndCluster,*/ &random, &threshold, &original};
    const char* shaderNames[] = { "Bayer 8x8", "Bayer 4x4", "Bayer 2x2", "Halftone", /*"Void and Cluster",*/ "Random", "Threshold", "Original" };

    GLuint* currentShader = &bayer64;
    currentShader = &original;

    //view
    //light settings
    glm::vec3 lightPosInit = glm::vec3(-1.0f, 1.0f, 0.5f);
    glm::vec3 lightPos = lightPosInit;
    //directional light

    //render loop
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        processInput(window);
        
        //clear
        glClearColor(lowColor.x,lowColor.y,lowColor.z,0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        //IMGui new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //IMGui window
        ImGui::Begin("Return of The One Bit");

        static int shaderItem = 0;
        ImGui::ListBox("Dithering Method", &shaderItem, shaderNames, IM_ARRAYSIZE(shaderNames), 4);
        currentShader = shaders[shaderItem];

        //ImGui::SliderFloat("Dithering Threshold", &pixelThreshold, 0.0f, 1.0f);
        ImGui::SliderFloat("Rotate Light Direction", &lightDegrees, 0.0f, 360.0f);
        ImGui::Checkbox("Auto Rotate Light Source", &autoRotate);


        ImGuiColorEditFlags misc_flags = (0 | ImGuiColorEditFlags_NoDragDrop | 0 | ImGuiColorEditFlags_NoOptions);
        ImGui::ColorEdit3("Low Color (Black)", (float*)&lowColor, misc_flags);
        ImGui::ColorEdit3("High Color (White)", (float*)&highColor, misc_flags);



        ImGui::End();

        //Uniforms
        glm::mat4 lightRotate = glm::rotate(glm::mat4(1), glm::radians(lightDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
        lightPos = glm::vec3(lightRotate*glm::vec4(lightPosInit,0.0f));
        if (autoRotate) { lightDegrees += deltaTime * 30.0f; if(lightDegrees>360.0f)lightDegrees-=360.0f; }
        glUseProgram(*currentShader);

        glUniform3f(glGetUniformLocation(*currentShader, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(glGetUniformLocation(*currentShader, "viewPos"), cameraPos.x, cameraPos.y, cameraPos.z);
        glUniform3f(glGetUniformLocation(*currentShader, "lowColor"), lowColor.x, lowColor.y, lowColor.z);
        glUniform3f(glGetUniformLocation(*currentShader, "highColor"), highColor.x, highColor.y, highColor.z);

        glUniform2f(glGetUniformLocation(*currentShader, "resolution"), SCR_WIDTH, SCR_HEIGHT);
        //glUniform1f(glGetUniformLocation(*currentShader, "pixelThreshold"), pixelThreshold);

        //Model specific uniforms are sent in Model class methods.

        //iterate models to render
        for (int i = 0; i < models.size(); i++) {


            //opengl matrices are applied from the right side. (last first)
            glm::mat4 model = glm::mat4(1);
            model = glm::translate(model, models[i]->position);
            model = glm::scale(model, glm::vec3(models[i]->scale, models[i]->scale, models[i]->scale));
            glm::mat4 view = glm::lookAt(cameraPos, cameraPos+cameraFront, cameraUp);
            glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 150.0f);

            //floor lucy dragon bunny boat teapot wall
            if (i == 1){ //lucy
                model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                model= glm::rotate(model,glm::radians(-90.0f),glm::vec3(1.0f,0.0f,0.0f));
            }
            else if (i == 2){
            }
            else if (i==4){ //boat
                model=glm::rotate(model,glm::radians(90.0f),glm::vec3(0.0f,1.0f,0.0f));
            }

            glUniformMatrix4fv(glGetUniformLocation(*currentShader, "model"), 1, GL_FALSE, &model[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(*currentShader, "view"), 1, GL_FALSE, &view[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(*currentShader, "projection"), 1, GL_FALSE, &projection[0][0]);

            models[i]->render(*currentShader);
        }
        

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        glfwSwapBuffers(window);
        glfwPollEvents();

    }



    // Delete ImGUI instances
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // glfwTerminate clears all allocated GLFW resources.
    glfwTerminate();
    return 0;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // makes sure the viewport matches the new window dimensions
    // width and height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraFront * cameraSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos += -1.0f * cameraFront * cameraSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos += cameraLeft * cameraSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += -1.0f * cameraLeft * cameraSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        pitch += 9.0f * cameraSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        pitch -= 9.0f * cameraSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        yaw -= 9.0f * cameraSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        yaw += 9.0f * cameraSpeed * deltaTime;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
    cameraLeft = glm::normalize(glm::cross(cameraUp, cameraFront));;
}
