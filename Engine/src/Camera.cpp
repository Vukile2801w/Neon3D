#include "Camera.hpp"

#include "Assert.hpp"

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"

namespace Neon
{
    glm::mat4 Camera::getViewMatrix() const
    {
        glm::mat4 view(1.0f);

        view = glm::rotate(
            view,
            -rotation.x,
            glm::vec3(1.0f, 0.0f, 0.0f));

        view = glm::rotate(
            view,
            -rotation.y,
            glm::vec3(0.0f, 1.0f, 0.0f));

        view = glm::rotate(
            view,
            -rotation.z,
            glm::vec3(0.0f, 0.0f, 1.0f));

        view = glm::translate(view, -position);

        return view;
    }

    glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const
    {
        NEON_ASSERT(
            aspectRatio > 0.0f,
            "Camera aspect ratio must be greater than zero");

        return glm::perspective(
            glm::radians(FOV),
            aspectRatio,
            0.1f,
            100.0f);
    }

    glm::vec3 Camera::getForward() const
    {
        glm::vec3 fwd(getViewMatrix()[0][2], getViewMatrix()[1][2], getViewMatrix()[2][2]);
        return -glm::normalize(fwd);
    }

    glm::vec3 Camera::getRight() const
    {
        return glm::normalize(
            glm::cross(
                getForward(),
                glm::vec3(0.0f, 1.0f, 0.0f)));
    }
}