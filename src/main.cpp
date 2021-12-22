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

#define ImConvert(part) \
{\
    part##_color = glm::vec3(Im_##part##_color.x * Im_##part##_color.w, Im_##part##_color.y * Im_##part##_color.w, Im_##part##_color.z * Im_##part##_color.w);\
    part.m_color = part##_color;\
}

#define ImConvert2(part) \
{\
    part##_color = glm::vec3(Im_##part##_color.x * Im_##part##_color.w, Im_##part##_color.y * Im_##part##_color.w, Im_##part##_color.z * Im_##part##_color.w);\
}

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
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // glfwSetCursorPosCallback(window, mouseCallback);
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
    ImVec4 Im_light_color = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);
    ImVec4 Im_body_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImVec4 Im_leftShoulder_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImVec4 Im_rightShoulder_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImVec4 Im_leftArm_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImVec4 Im_rightArm_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    glm::vec3 light_color;
    glm::vec3 body_color;
    glm::vec3 leftShoulder_color;
    glm::vec3 rightShoulder_color;
    glm::vec3 leftArm_color;
    glm::vec3 rightArm_color;
    
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glEnable(GL_DEPTH_TEST);

    // sphere data
    std::vector<unsigned int> sphereIndices;
    std::vector<float> sphereVertices;

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
    unsigned int face_map = loadTexture("../resources/Marc_Dekamps.png");
    unsigned int earth_map = loadTexture("../resources/Mercator-projection.png");

    // unsigned int specular_map = loadTexture("../resources/container2_specular.png");
    // cubeShader.use();
    
    cubeShader.setInt("material.diffuse", 1);
    cubeShader.setInt("material.specular", 1);

    float radius = 5.0f;
    float height = 3.0f;
    float lightAngle = 30.0f;
    float specular_constant = 32.0f;

    Node body ({
        glm::vec3(0.0f),
        glm::vec3(0.0f, 2.0f, 0.0f), // translate
        glm::vec3(2.0f, 3.0f, 1.0f),  // scale
        glm::vec3(0.0f, 1.0f, 0.0f),  // axis
        0.0f                          // degrees    
    }, 1, cubeVAO, colorShader);

    Node head ({
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 1.5f, 0.0f),  // translate
        glm::vec3(1.3f, 1.3f, 1.3f),  // scale
        glm::vec3(0.0f, 1.0f, 0.0f),  // axis
        (float)glm::radians(90.0f)    // degrees
    }, 1, sphereVAO, cubeShader);

    Node leftShoulder({
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.4f, 1.5f, 0.0f),  // translate
        glm::vec3(0.6f, 0.6f, 0.8f),  // scale
        glm::vec3(1.0f, 0.0f, 0.0f),  // axis
        0.0f                          // degrees
    }, 1, sphereVAO, colorShader);

    Node rightShoulder({
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(-1.4f, 1.5f, 0.0f),  // translate
        glm::vec3(0.6f, 0.6f, 0.8f),  // scale
        glm::vec3(0.0f, 0.0f, 1.0f),  // axis
        0.0f                          // degrees
    }, 1, sphereVAO, colorShader);

    Node leftArm({
        glm::vec3(0.0f, -1.3f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),  // translate
        glm::vec3(0.3f, 1.5f, 0.3f),  // scale
        glm::vec3(0.0f, 0.0f, 1.0f),  // axis
        // 0.0f
        (float)glm::radians(30.0f)    // degrees
    }, 1, cubeVAO, colorShader);

    Node rightArm({
        glm::vec3(0.0f, -1.3f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),  // translate
        glm::vec3(0.3f, 1.5f, 0.3f),  // scale
        glm::vec3(0.0f, 0.0f, 1.0f),  // axis
        // 0.0f
        (float)glm::radians(-30.0f)    // degrees
    }, 1, cubeVAO, colorShader);

    Node leftElbow({
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, -1.1f, 0.0f),  // translate
        glm::vec3(0.5f, 0.5f, 0.5f),  // scale
        glm::vec3(1.0f, 0.0f, 0.0f),  // axis
        0.0f
    }, 1, sphereVAO, colorShader);

    Node rightElbow({
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, -1.1f, 0.0f),  // translate
        glm::vec3(0.5f, 0.5f, 0.5f),  // scale
        glm::vec3(0.0f, 0.0f, 1.0f),  // axis
        (float)glm::radians(-30.0f)
    }, 1, sphereVAO, colorShader);

    Node leftForearm({
        glm::vec3(0.0f, -0.8f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),  // translate
        glm::vec3(0.2f, 1.0f, 0.2f),  // scale
        glm::vec3(1.0f, 0.0f, 0.0f),  // axis
        0.0f
    }, 1, cubeVAO, colorShader);

    Node rightForearm({
        glm::vec3(0.0f, -0.8f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),  // translate
        glm::vec3(0.2f, 1.0f, 0.2f),  // scale
        glm::vec3(1.0f, 0.0f, 0.0f),  // axis
        0.0f
    }, 1, cubeVAO, colorShader);
    
    Node hip({
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, -2.0f, 0.0f),  // translate
        glm::vec3(2.0f, 0.5f, 1.0f),  // scale
        glm::vec3(1.0f, 0.0f, 0.0f),  // axis
        0.0f
    }, 1, sphereVAO, colorShader);

    Node leftThigh({
        glm::vec3(0.5f, -1.5f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),  // translate
        glm::vec3(0.7f, 2.0f, 0.5f),  // scale
        glm::vec3(1.0f, 0.0f, 0.0f),  // axis
        0.0f
    }, 1, cubeVAO, colorShader);

    Node rightThigh({
        glm::vec3(-0.5f, -1.5f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),  // translate
        glm::vec3(0.7f, 2.0f, 0.5f),  // scale
        glm::vec3(1.0f, 0.0f, 0.0f),  // axis
        0.0f
    }, 1, cubeVAO, colorShader);

    Node lightCube ({
        glm::vec3(0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),  // translate
        glm::vec3(1.0f, 1.0f, 1.0f),  // scale
        glm::vec3(1.0f, 0.0f, 0.0f),  // axis
        0.0f
    }, 1, cubeVAO, lightShader);

    Node Earth ({
        glm::vec3(0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),  // translate
        glm::vec3(3.0f, 3.0f, 3.0f),  // scale
        glm::vec3(1.0f, 0.0f, 0.0f),  // axis
        0.0f
    }, 1, sphereVAO, cubeShader);

    body.addChild(&head);
    body.addChild(&leftShoulder);
    body.addChild(&rightShoulder);
    leftShoulder.addChild(&leftArm);
    rightShoulder.addChild(&rightArm);
    leftArm.addChild(&leftElbow);
    rightArm.addChild(&rightElbow);
    leftElbow.addChild(&leftForearm);
    rightElbow.addChild(&rightForearm);

    hip.addChild(&leftThigh);
    hip.addChild(&rightThigh);

    hip.addChild(&body);

    body.m_color = glm::vec3(0.0f, 1.0f, 0.0f);
    leftShoulder.m_color = glm::vec3(0.0f, 1.0f, 0.0f);

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


        ImGui::Begin("Light Settings");

        ImGui::Text("radius");

        ImGui::SliderFloat("radius", &radius, 0.0f, 10.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::SliderFloat("height", &height, -4.0f, 4.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::SliderFloat("light angle", &lightAngle, 0.0f, 360.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("light color", (float*)&Im_light_color);        // Edit 3 floats representing a color
        ImGui::End();

        ImGui::Begin("Color Settings");

        ImGui::ColorEdit3("body color", (float*)&Im_body_color);        // Edit 3 floats representing a color
        ImGui::ColorEdit3("left shoulder color", (float*)&Im_leftShoulder_color);        // Edit 3 floats representing a color
        ImGui::ColorEdit3("right shoulder color", (float*)&Im_rightShoulder_color);        // Edit 3 floats representing a color
        ImGui::ColorEdit3("left arm color", (float*)&Im_leftArm_color);        // Edit 3 floats representing a color
        ImGui::ColorEdit3("right arm color", (float*)&Im_rightArm_color);        // Edit 3 floats representing a color
        ImGui::SliderFloat("specular", &specular_constant, 0.0f, 50.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

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
        cubeShader.setFloat("material.shininess", specular_constant);
        // configure light
        lightPos = glm::vec3(radius * sin(glm::radians(lightAngle)), height, radius * cos(glm::radians(lightAngle)));
        light_color = glm::vec3(Im_light_color.x * Im_light_color.w, Im_light_color.y * Im_light_color.w, Im_light_color.z * Im_light_color.w);

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
        glBindTexture(GL_TEXTURE_2D, earth_map);

        Earth.draw(glm::mat4(1.0f));

        glBindTexture(GL_TEXTURE_2D, face_map);

        // configure colorShader
        // ---------------------
        colorShader.use();
        colorShader.setMat4("projection", projection);
        colorShader.setMat4("view", view);
        colorShader.setMat4("model", model);
        colorShader.setVec3("lightPos", lightPos);
        colorShader.setVec3("lightColor", light_color);
        // colorShader.setVec3("objectColor", glm::vec3(0.5f, 0.5f, 0.5f));

        // configure lightShader
        // ---------------------
        lightShader.use();
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        lightShader.setMat4("model", model);
        lightShader.setVec3("inputColor", light_color);
        
        lightCube.draw(model);

        // animation
        float angle = (float)glfwGetTime();
        rightShoulder.m_trans.m_degrees = glm::radians(-80.0f + 30.0f * sin(angle * 2));
        rightElbow.m_trans.m_degrees = glm::radians(-50.0f + 30.0f * sin(angle * 2));
        rightThigh.m_trans.m_degrees = glm::radians(30.0f * sin(angle * 2));
        leftThigh.m_trans.m_degrees = -glm::radians(30.0f * sin(angle * 2));
        leftShoulder.m_trans.m_degrees = glm::radians(45.0f * sin(angle * 2));
        body.m_trans.m_degrees = glm::radians(20.0f * sin(angle * 2));
        glm::mat4 overallModel = glm::rotate(glm::mat4(1.0f), -(float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));

        // color
        ImConvert(body);
        ImConvert(leftShoulder);
        ImConvert(rightShoulder);
        ImConvert(leftArm);
        ImConvert(rightArm);
        
        overallModel = glm::translate(overallModel, glm::vec3(5.0f, 0.0f, 0.0f));
        hip.draw(overallModel);

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
