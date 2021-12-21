// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include <iostream>
#include <vector>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
// #include <stb_image.h>
#include <GL/glut.h>
#include "utils.cpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.hpp"
#include "Shader.hpp"
#include "Node.cpp"
// #include "cube.cpp"

#define DRAW cubeShader.setMat4("model", trans); \
        glDrawArrays(GL_TRIANGLES, 0, 36); \
        trans = glm::mat4(1.0f)

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

#define WINDOW_WIDTH 1920.0f
#define WINDOW_HEIGHT 1080.0f

// camera
Camera camera;
float lastX = WINDOW_WIDTH / 2.0f, lastY = WINDOW_HEIGHT / 2.0f;

// time
float lastTime = 0.0f;
float currentTime = 0.0f; 
float deltaTime = 0.0f;

// lighting 
glm::vec3 lightPos;

void mouseCallback (GLFWwindow * window, double xPos, double yPos) {
    float sensitivity = 0.1f;

    // process mouse data
    // ------------------

    float x_offset = xPos - lastX;
    float y_offset = lastY - yPos;

    lastX = xPos;
    lastY = yPos;

    camera.ProcessMouseMovement(x_offset, y_offset);
}

void processInput (GLFWwindow * window, Camera & camera, float deltaTime) {
    const float sensitivity = 2.5f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W)) {
        // cameraPos += sensitivity * cameraFront;
        // camera.processKeyboardInput(FORWARD, deltaTime);
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S)) {
        // camera.processKeyboardInput(BACKWARD, deltaTime);
        // cameraPos -= sensitivity * cameraFront;
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A)) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D)) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_R)) {
        camera.ProcessKeyboard(UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_F)) {
        camera.ProcessKeyboard(DOWN, deltaTime);
    }
}

int main(int, char**)
{    
    // ---------------
    // initializations 
    // ---------------
    glfwInit();

    // set opengl version
    const char* glsl_version = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // set core profile
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow * window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "learning", NULL, NULL);
    if (window == NULL) {
        std::cout << "ERROR::WINDOW_CREATING_FAILURE" << std::endl;
        glfwTerminate();
        return -1;
    }
    // mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwMakeContextCurrent(window);
    // glad set up
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "ERROR::GLAD_RETRIVE_FAILURE" << std::endl;
        glfwTerminate();
        return -1;
    }
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    // imgui states
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    glm::vec3 light_color;
    
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glEnable(GL_DEPTH_TEST);

    // model data
    // ----------

    // cube data
    // float vertices[] = {
    //     // positions          // normals           // texture coords
    //     -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
    //      0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
    //      0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
    //      0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
    //     -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
    //     -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

    //     -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
    //      0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
    //      0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
    //      0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
    //     -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
    //     -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

    //     -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
    //     -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
    //     -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    //     -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    //     -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    //     -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

    //      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
    //      0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
    //      0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    //      0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    //      0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    //      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

    //     -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
    //      0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
    //      0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    //      0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    //     -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
    //     -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

    //     -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
    //      0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
    //      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
    //      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
    //     -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
    //     -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    // };
    // cube positions
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    // sphere data
    std::vector<unsigned int> sphereIndices;
    std::vector<float> sphereVertices;
    
    // cube with texture
    // -----------------

    // load data into GPU (VBO)
    // unsigned int cubeVAO, VBO;
    // glGenBuffers(1, &VBO);
    // glGenVertexArrays(1, &cubeVAO);
    // glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    // // set one cube and its parsing way
    // glBindVertexArray(cubeVAO);
    // // position
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    // glEnableVertexAttribArray(0);
    // // nromals
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    // glEnableVertexAttribArray(1);
    // // textures
    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    // glEnableVertexAttribArray(2);

    unsigned int cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    // cubeVAO is 1
    buildCubeData(cubeVAO);

    // sphere
    // ------

    unsigned int sphereVAO;
    glGenVertexArrays(1, &sphereVAO);
    // sphereVAO is 2
    buildSphereData(sphereVAO, sphereIndices, sphereVertices);

    // -----------
    // set shaders
    // -----------

    Shader cubeShader("../GLSLs/cube_vertex.glsl", "../GLSLs/cube_fragment.glsl");
    Shader lightShader("../GLSLs/light_vertex.glsl", "../GLSLs/light_fragment.glsl");
    Shader colorShader("../GLSLs/colored_vertex.glsl", "../GLSLs/colored_fragment.glsl");

    // ------------
    // load texture
    // ------------
    unsigned int diffuse_map = loadTexture("../resources/awesomeface.png");

    // unsigned int specular_map = loadTexture("../resources/container2_specular.png");
    // cubeShader.use();
    
    cubeShader.setInt("material.diffuse", 1);
    cubeShader.setInt("material.specular", 1);

    float radius = 5.0f;

    Transformation firstTrans(
        glm::vec3(0.0f, -1.0f, 0.0f), // translate
        glm::vec3(1.0f, 1.0f, 1.0f),  // scale
        glm::vec3(0.0f, 1.0f, 0.0f),  // axis
        0.0f                          // degrees
    );

    Transformation secondTrans(
        glm::vec3(0.0f, 1.0f, 0.0f),  // translate
        glm::vec3(1.0f, 1.0f, 1.0f),  // scale
        glm::vec3(0.0f, 1.0f, 0.0f),  // axis
        20.0f                          // degrees
    );

    Transformation thirdTrans(
        glm::vec3(0.0f, 1.0f, 0.0f),  // translate
        glm::vec3(1.0f, 1.0f, 1.0f),  // scale
        glm::vec3(0.0f, 1.0f, 0.0f),  // axis
        40.0f     // degrees
    );

    Node first  (firstTrans,  1, cubeVAO, cubeShader);
    Node second (secondTrans, 1, cubeVAO, colorShader);
    Node third  (thirdTrans,  0, sphereVAO, lightShader);

    first.addChild(&second);
    second.addChild(&third);
    // unsigned int sphereVAO = 0;

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // ---------------------
        // GLFW & imGUI settings
        // ---------------------

        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        ImGui::Begin("Hello, world!");

        ImGui::Text("radius");

        ImGui::SliderFloat("radius", &radius, 0.0f, 10.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clear_color);        // Edit 3 floats representing a color

        ImGui::SameLine();

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        // process time
        // ------------
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        processInput(window, camera, deltaTime);

        // ---------------------
        // configure all shaders
        // ---------------------

        // initlize matrices: projectin & view & model
        // -------------------------------------------
        glm::mat4 projection = 
            glm::perspective(glm::radians(45.0f), WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f,100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        // configure cubeShader
        // --------------------
        cubeShader.use();
        // configure material
        cubeShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        cubeShader.setFloat("material.shininess", 32.0f);
        // configure light
        lightPos = glm::vec3(radius * sin(glfwGetTime()),0.0f,radius * cos(glfwGetTime()));
        light_color = glm::vec3(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w);
        cubeShader.setVec3("light.ambient",  0.3f * light_color);
        cubeShader.setVec3("light.diffuse",  0.5f * light_color);
        cubeShader.setVec3("light.specular",  1.0f * light_color);
        cubeShader.setVec3("light.direction", lightPos);
        // cubeShader.setVec3("light.position", lightPos);
        cubeShader.setVec3("viewPos", camera.Position);
        // set projection & view
        cubeShader.setMat4("projection", projection);
        cubeShader.setMat4("view", view);

        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuse_map);

        // configure colorShader
        // ---------------------
        colorShader.use();
        colorShader.setMat4("projection", projection);
        colorShader.setMat4("view", view);
        colorShader.setMat4("model", model);
        colorShader.setVec3("lightPos", lightPos);
        colorShader.setVec3("lightColor", light_color);
        colorShader.setVec3("objectColor", glm::vec3(0.5f, 0.5f, 0.5f));

        // glBindVertexArray(cubeVAO);
        // glDrawArrays(GL_TRIANGLES, 0, 36);
        // configure lightShader
        // ---------------------
        lightShader.use();
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        lightShader.setMat4("model", model);
        lightShader.setVec3("inputColor", light_color);
        // draw the light cube    
        first.draw(glm::mat4(1.0f));


        // glBindVertexArray(sphereVAO);
        // // std::cout << sphereIndices.size() << std::endl;
        // glDrawElements(GL_TRIANGLE_STRIP, sphereIndices.size(), GL_UNSIGNED_INT, 0);

        // draw UI
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
