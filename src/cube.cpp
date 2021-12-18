#include <glad/glad.h>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"

class Cube {

public:
    unsigned int m_VAO;
    Shader shader;
    glm::mat4 m_model;

    Cube (const char * vertexPath, const char * fragmentPath)
        :shader(vertexPath, fragmentPath), m_model(glm::mat4(1.0f))
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

    void set_pro_and_view (const glm::mat4 & projection, const glm::mat4 & view)
    {
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
    }

    void move () {
        
    }

    void set_model (const glm::mat4 & model)
    {
        m_model = model;
        shader.setMat4("model", m_model);
    }

private:
};

class Model {
public:

    Cube m_shape;

    std::vector<glm::mat4> m_models;
    std::vector<glm::vec3> m_translates;

public:

    Model (const Cube & shape, const std::vector<glm::vec3> translates)
        :m_shape(shape), m_translates(translates)
    {
        m_models.reserve(m_translates.size());
        for (glm::vec3 translate : m_translates) {
            m_models.emplace_back(glm::translate(glm::mat4(1.0f), translate));
        }
    }

    void draw () {
        glBindVertexArray(m_shape.m_VAO);

        for (glm::mat4 model : m_models) {
            m_shape.set_model(model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }

    // relative position!!
    void translate (const glm::vec3 & target) {
        if ( m_translates.size() == 0 ) {
            std::cout << "Model::translate || No translates are set !!\n";
            return;
        }

        for (glm::vec3 & translate : m_translates) {
            translate = translate + target;
        }

        for (glm::mat4 model : m_models) {
            model = glm::translate(model, target);
        }
    }

    void rotate_with_y (const float degrees)
    {
        if ( m_translates.size() == 0 ) {
            std::cout << "Model::rotate || No models set !!\n";
            return;
        }

        std::vector<glm::mat4> temp_models;
        temp_models.reserve(m_translates.size());

        for (glm::vec3 & translate : m_translates) {
            glm::mat4 rotates = glm::rotate(glm::mat4(1.0f), degrees, glm::vec3(0.0f, 1.0f, 0.0f));
            temp_models.emplace_back(glm::translate(rotates, translate));
        }

        m_models = temp_models;
    }

};