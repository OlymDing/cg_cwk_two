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
#include <utils.h>

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

void renderSphere();

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
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };
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

    // generate sphere data

    // generate CCW index list of sphere triangles
    // k1--k1+1
    // |  / |
    // | /  |
    // k2--k2+1
    // unsigned int sectorCount = 30, stackCount = 30;
    // float sphereRadius = 1.0f;
    // const float PI = glm::pi<float>();

    // std::vector<float> sphereVertices;
    // sphereVertices.reserve(sectorCount * stackCount * 8);

    // float x, y, z, xy;                              // vertex position
    // float nx, ny, nz, lengthInv = 1.0f / sphereRadius;    // vertex normal
    // float s, t;                                     // vertex texCoord

    // float sectorStep = 2 * PI / sectorCount;
    // float stackStep = PI / stackCount;
    // float sectorAngle, stackAngle;

    // for(int i = 0; i <= stackCount; ++i)
    // {
    //     stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
    //     xy = sphereRadius * cosf(stackAngle);             // r * cos(u)
    //     z = sphereRadius * sinf(stackAngle);              // r * sin(u)

    //     // add (sectorCount+1) vertices per stack
    //     // the first and last vertices have same position and normal, but different tex coords
    //     for(int j = 0; j <= sectorCount; ++j)
    //     {
    //         sectorAngle = j * sectorStep;           // starting from 0 to 2pi

    //         // vertex position (x, y, z)
    //         x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
    //         y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
    //         sphereVertices.push_back(x);
    //         sphereVertices.push_back(y);
    //         sphereVertices.push_back(z);

    //         // normalized vertex normal (nx, ny, nz)
    //         nx = x * lengthInv;
    //         ny = y * lengthInv;
    //         nz = z * lengthInv;
    //         sphereVertices.push_back(nx);
    //         sphereVertices.push_back(ny);
    //         sphereVertices.push_back(nz);

    //         // vertex tex coord (s, t) range between [0, 1]
    //         s = (float)j / sectorCount;
    //         t = (float)i / stackCount;
    //         sphereVertices.push_back(s);
    //         sphereVertices.push_back(t);
    //     }
    // }

    // std::vector<unsigned int> indices;

    // unsigned int k1, k2;
    // for(int i = 0; i < stackCount; ++i)
    // {
    //     k1 = i * (sectorCount + 1);     // beginning of current stack
    //     k2 = k1 + sectorCount + 1;      // beginning of next stack

    //     for(int j = 0; j < sectorCount; ++j, ++k1, ++k2)
    //     {
    //         // 2 triangles per sector excluding 1st and last stacks
    //         if(i != 0)
    //         {
    //             // addIndices(k1, k2, k1+1);   // k1---k2---k1+1
    //             indices.push_back(k1);
    //             indices.push_back(k2);
    //             indices.push_back(k1+1);
    //         }

    //         if(i != (stackCount-1))
    //         {
    //             // addIndices(k1+1, k2, k2+1); // k1+1---k2---k2+1
    //             indices.push_back(k1+1);
    //             indices.push_back(k2);
    //             indices.push_back(k2+1);
    //         }
    //     }
    // }
    
    // cube with texture
    // -----------------

    // load data into GPU (VBO)
    unsigned int cubeVAO, VBO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    // set one cube and its parsing way
    glBindVertexArray(cubeVAO);
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    // nromals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // textures
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // light cube 
    // ----------

    // set another cube to represent light source
    unsigned int lightVAO;
    glBindVertexArray(lightVAO);
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    // sphere
    // ------

    // load sphere data
    // unsigned int sphereVBO, sphereVAO;

    // glGenVertexArrays(1, &sphereVAO);
    // glBindVertexArray(sphereVAO);

    // glGenBuffers(1, &sphereVBO);
    // glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(sphereVertices.data()), sphereVertices.data(), GL_STATIC_DRAW);

    // unsigned int sphereEBO;
    // glGenBuffers(1, &sphereEBO);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices.data()), indices.data(), GL_STATIC_DRAW);

    // glBindVertexArray(sphereEBO);
    // glVertexAttribPointer(0, 3, GL_FLOAT, false, 8 * sizeof(float), (void*)0);
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(1, 3, GL_FLOAT, false, 8 * sizeof(float), (void*)(sizeof(float)*3));
    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(2, 2, GL_FLOAT, false, 8 * sizeof(float), (void*)(sizeof(float)*6));
    // glEnableVertexAttribArray(2);

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
        glm::vec3(1.0f, 2.0f, 1.0f),  // scale
        glm::vec3(0.0f, 1.0f, 0.0f),  // axis
        0.0f                          // degrees
    );

    Transformation secondTrans(
        glm::vec3(0.0f, 0.0f, 0.0f),  // translate
        glm::vec3(1.0f, 1.0f, 1.0f),  // scale
        glm::vec3(0.0f, 1.0f, 0.0f),  // axis
        0.0f                          // degrees
    );

    Transformation thirdTrans(
        glm::vec3(0.0f, 1.5f, 0.0f),  // translate
        glm::vec3(1.0f, 1.0f, 1.0f),  // scale
        glm::vec3(0.0f, 1.0f, 0.0f),  // axis
        40.0f     // degrees
    );

    Node first  (firstTrans,  1, cubeVAO, colorShader);
    Node second (secondTrans, 1, cubeVAO, colorShader);
    Node third  (thirdTrans,  0, cubeVAO, colorShader);

    first.addChild(&third);
    // second.addChild(&third);

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

        // configure lightShader
        // ---------------------
        lightShader.use();
        // calculate model
        // model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        // set projection, view, model & light color
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);
        lightShader.setMat4("model", model);
        lightShader.setVec3("inputColor", light_color);
        // draw the light cube
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // draw the model
        first.draw(glm::mat4(1.0f));
        
        renderSphere();

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

void renderSphere()
{
    unsigned int sphereVAO = 0;
    unsigned int indexCount;
    if (sphereVAO == 0)
    {
        glGenVertexArrays(1, &sphereVAO);

        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uv;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;

        const unsigned int X_SEGMENTS = 64;
        const unsigned int Y_SEGMENTS = 64;
        const float PI = 3.14159265359;
        for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
        {
            for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
            {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                positions.push_back(glm::vec3(xPos, yPos, zPos));
                uv.push_back(glm::vec2(xSegment, ySegment));
                normals.push_back(glm::vec3(xPos, yPos, zPos));
            }
        }

        bool oddRow = false;
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        {
            if (!oddRow) // even rows: y == 0, y == 2; and so on
            {
                for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
                {
                    indices.push_back(y       * (X_SEGMENTS + 1) + x);
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                }
            }

          // 这里奇偶分开添加是有道理的，奇偶分开添加，就能首位相连，自己可以拿笔画一画
            else
            {
                for (int x = X_SEGMENTS; x >= 0; --x)
                {
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    indices.push_back(y       * (X_SEGMENTS + 1) + x);
                }
            }
            oddRow = !oddRow;
        }
        indexCount = indices.size();

        std::vector<float> data;
        for (unsigned int i = 0; i < positions.size(); ++i)
        {
            data.push_back(positions[i].x);
            data.push_back(positions[i].y);
            data.push_back(positions[i].z);
            if (uv.size() > 0)
            {
                data.push_back(uv[i].x);
                data.push_back(uv[i].y);
            }
            if (normals.size() > 0)
            {
                data.push_back(normals[i].x);
                data.push_back(normals[i].y);
                data.push_back(normals[i].z);
            }
        }
        glBindVertexArray(sphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        float stride = (3 + 2 + 3) * sizeof(float);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
    }

    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}