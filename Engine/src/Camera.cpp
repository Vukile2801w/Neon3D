#include "Camera.hpp"

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"

namespace Neon
{
    glm::mat4 Camera::getViewMatrix() const
    {
        glm::mat4 view(1.0f);

        view = glm::rotate(
            view,
            -rotation.y,
            glm::vec3(0.0f, 1.0f, 0.0f));

        view = glm::rotate(
            view,
            -rotation.x,
            glm::vec3(1.0f, 0.0f, 0.0f));

        view = glm::translate(view, -position);

        return view;
    }

    glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const
    {
        return glm::perspective(
            glm::radians(FOV),
            aspectRatio,
            0.1f,
            100.0f);
    }
}