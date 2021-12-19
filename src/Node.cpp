#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#include "Shader.hpp"


struct Transformation
{
    glm::vec3 m_translate;
    glm::vec3 m_scale;
    glm::vec3 m_axis;
    float m_degrees;

    // full-parameter constructor
    Transformation (const glm::vec3 & translate, const glm::vec3 & scale, const glm::vec3 & axis, float degrees)
        : m_translate(translate),
          m_scale(scale),
          m_axis(axis),
          m_degrees(degrees) {}

    // default constructor
    Transformation () 
        : m_translate(glm::vec3(1.0f)),
          m_scale(glm::vec3(1.0f)),
          m_axis(glm::vec3(0.0f, 1.0f, 0.0f)), // by default rotate around y-axis
          m_degrees(0.0f) {}

    glm::mat4 getTrans (const glm::mat4 & model) {
        glm::mat4 temp = glm::rotate(model, m_degrees, m_axis);
        temp = glm::translate(temp, m_translate);
        temp = glm::scale(temp, m_scale);
        return temp;
    }

};

struct Node {
public:
    Transformation m_trans;
    std::vector<Node*> m_children;
    unsigned int m_VAO;
    Shader m_shader;
    // glm::mat4 m_model;

    Node (
        const Transformation & trans,
        int child_num, 
        unsigned int VAO,
        const Shader & shader
    )
        : m_trans(trans),
          m_VAO(VAO),
          m_shader(shader)
    {
        m_children.reserve(child_num); // reserve vector
        std::cout << "Node Constructed !\n";
    }

    void addChild (Node * node)
    {
        m_children.push_back(node);
    }

    void draw (const glm::mat4 & model)
    {
        glBindVertexArray(m_VAO);
        glm::mat4 new_model = m_trans.getTrans(model);
        // m_model = new_model;
        m_shader.setMat4("model", new_model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        for (Node * child : m_children) {
            child->draw(new_model);
        }

    }
};
