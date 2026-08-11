#include "Transform.hpp"

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"

namespace Neon
{
    Transform::Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
    {
        this->position = position;
        this->rotation = rotation;
        this->scale = scale;
    }

    glm::mat4 Transform::getMatrix() const
    {
        glm::mat4 model(1.0f);

        model = glm::translate(model, position);

        model = glm::rotate(
            model,
            rotation.x,
            glm::vec3(1.0f, 0.0f, 0.0f));

        model = glm::rotate(
            model,
            rotation.y,
            glm::vec3(0.0f, 1.0f, 0.0f));

        model = glm::rotate(
            model,
            rotation.z,
            glm::vec3(0.0f, 0.0f, 1.0f));

        model = glm::scale(model, scale);

        return model;
    }
}