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

#include "loadShader.h"
#include "yjReader.h"
#include "smoothing.h"

using std::cout;
// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// UI

int main()
{
    //xOn = false;


    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Return of The One Bit", NULL, NULL);
    if (window == NULL)
    {
        cout << "Failed to createwindow\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers]
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

    //Load Model
    //YJ bunny("C:/Users/lab/Desktop/yj/ExaggeratedShadingInteractive/ExaggeratedShadingInteractive/bunny/stanford-bunny.yj");
    //YJ bunny("C:/Users/lab/Desktop/yj/ExaggeratedShadingInteractive/ExaggeratedShadingInteractive/golfball/GolfBallOBJ.yj");
    //YJ bunny("C:/Users/lab/Desktop/yj/ExaggeratedShadingInteractive/ExaggeratedShadingInteractive/lucy/lucy.yj");
    YJ bunny(".\\bunny\\stanford-bunny.yj");


    //Sigma values. from featureSize and multiplied by sqrt2 every step.
    float feature = featureSize(bunny.vertices);
    cout << "Feature size of model : " << feature << "\n";
    for (int i = 0; i < 20; i++) sigma[i] = 0.4 * feature * glm::pow(glm::sqrt(2), float(i));
    glm::vec3 cen = center(bunny.vertices); std::cout << "Center of model : " << cen.x << ", " << cen.y << ", " << cen.x << "\n";

    //Load Shaders
    GLuint cosine = loadShader("C:/Users/lab/Desktop/yj/ExaggeratedShadingInteractive/shaders/cosine.vs", "C:/Users/lab/Desktop/yj/ExaggeratedShadingInteractive/shaders/cosine.fs");
    GLuint xShade = loadShader("C:/Users/lab/Desktop/yj/ExaggeratedShadingInteractive/shaders/xShade.vs", "C:/Users/lab/Desktop/yj/ExaggeratedShadingInteractive/shaders/xShade.fs");
    GLuint softToon = loadShader("C:/Users/lab/Desktop/yj/ExaggeratedShadingInteractive/shaders/cosine.vs", "C:/Users/lab/Desktop/yj/ExaggeratedShadingInteractive/shaders/softToon.fs");
    //GLuint principalDirections=loadShader("C:/Users/lab/Desktop/yj/ExaggeratedShadingInteractive/shaders/principalDirections.vs", "C:/Users/lab/Desktop/yj/ExaggeratedShadingInteractive/shaders/principalDirections.fs","C:/Users/lab/Desktop/yj/ExaggeratedShadingInteractive/shaders/principalDirections.gs");




    GLuint* currentShader = &xShade;

    //view
    glm::vec3 cameraPos = glm::vec3(0, 0, 1);
    //light settings
    glm::vec3 lightPos = glm::vec3(-1, 1, 1);
    glm::vec3 lightDiffuse = glm::vec3(1, 1, 1) * diffuse;


    //render loop
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        //yDegrees += 1;
        //yDegrees =int(yDegrees)%360;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        //IMGui new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //IMGui window
        ImGui::Begin("XShade Interactive");

        ImGui::Checkbox("Exaggerated Shading", &xOn);
        ImGui::SliderFloat("Rotate X", &xDegrees, 0.0f, 360.0f);
        ImGui::SliderFloat("Rotate Y", &yDegrees, 0.0f, 360.0f);
        ImGui::SliderFloat("Model Size", &modelSize, 0.05f, 500.0f);
        //ImGui::SliderFloat("Brightness", &diffuse, 0.0f, 2.0f);
        ImGui::SliderInt("Number of Smoothing Scales", &scales, 1, 19);
        ImGui::SliderFloat("Contribution factor of ki", &contributionScale, -5.0f, 5.0f);
        ImGui::SliderFloat("Light by scale clamp coefficient", &clampCoef, 1.0f, 1000.0f);
        ImGui::SliderFloat("Ambient", &ambient, 0.0f, 1.0f);
        ImGui::End();


        //contribution factor
        GLfloat contributionBeforeNorm[20] = { 0 };
        GLfloat contributionSum = 0;
        for (int i = 0; i < scales; i++) {
            contributionBeforeNorm[i] = glm::pow(sigma[i], contributionScale);
            contributionSum += contributionBeforeNorm[i];
        }
        for (int i = 0; i < scales; i++) {
            contribution[i] = contributionBeforeNorm[i] / contributionSum;
            //cout << "Contribution " << i << " : " << contribution[i] << "\n";
        }

        //if (!xOn)currentShader = &softToon;
        if (!xOn)currentShader = &cosine;
        else currentShader = &xShade;

        glUseProgram(*currentShader);

        //Uniforms
        glUniform3f(glGetUniformLocation(*currentShader, "light.position"), lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(glGetUniformLocation(*currentShader, "light.diffuse"), lightDiffuse.x, lightDiffuse.y, lightDiffuse.z);
        if (xOn) {
            // YOU NEED TO BIND PROGRAM WITH GLUSEPROGRAM BEFORE glUNIFORM
            //send contribution ki to shader as uniform (array)
            glUniform1fv(glGetUniformLocation(xShade, "contribution"), 20, contribution);
            glUniform1f(glGetUniformLocation(xShade, "clampCoef"), clampCoef);
            glUniform1i(glGetUniformLocation(xShade, "scales"), scales);
            glUniform1f(glGetUniformLocation(xShade, "ambient"), ambient);
        }

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

