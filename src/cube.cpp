#include <glad/glad.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"

class Cube {

public:
    unsigned int m_VAO;
    Shader shader;

    Cube (const char * vertexPath, const char * fragmentPath)
        :shader(vertexPath, fragmentPath)
    {
        glGenVertexArrays(1, &m_VAO);

        glBindVertexArray(m_VAO);
        // position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(0);

        // nromals
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
    }

    ~Cube () {
        std::cout << "Cube Destroied ~" << std::endl;
    };

private:
};