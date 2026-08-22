#include "glad/glad.h"
#include "glfw/glfw3.h"

#include "Input.hpp"

namespace Neon
{
    Input::Input(Window *window) : m_window(window->getGlfwWindow())
    {
        for (int i = 1; i < Key::KEY_COUNT; i++)
        {
            keyData[i] = KeyData{static_cast<Key>(i),
                                 false,
                                 false,
                                 false};
        }
        for (int i = 1; i < MouseButton::MOUSE_BUTTON_COUNT; i++)
        {
            mouseStatus.buttons[i] = MouseButtonData{static_cast<MouseButton>(i),
                                                     0.0f,
                                                     false,
                                                     false};
        }
    }

    Input::~Input() // Defined the destructor
    {
        // Optional cleanup logic can be placed here
    }

    void Input::handleInput()
    {
        glfwPollEvents();

        // Manual mapping from EcoSim Keys to Raylib Keys
        for (int i = 1; i < Key::KEY_COUNT; i++)
        {
            Key key = static_cast<Key>(i);
            int glfwKey = -1; // Default invalid value

            switch (key)
            {
            case KEY_A:
                glfwKey = GLFW_KEY_A;
                break;
            case KEY_B:
                glfwKey = GLFW_KEY_B;
                break;
            case KEY_C:
                glfwKey = GLFW_KEY_C;
                break;
            case KEY_D:
                glfwKey = GLFW_KEY_D;
                break;
            case KEY_E:
                glfwKey = GLFW_KEY_E;
                break;
            case KEY_F:
                glfwKey = GLFW_KEY_F;
                break;
            case KEY_G:
                glfwKey = GLFW_KEY_G;
                break;
            case KEY_H:
                glfwKey = GLFW_KEY_H;
                break;
            case KEY_I:
                glfwKey = GLFW_KEY_I;
                break;
            case KEY_J:
                glfwKey = GLFW_KEY_J;
                break;
            case KEY_K:
                glfwKey = GLFW_KEY_K;
                break;
            case KEY_L:
                glfwKey = GLFW_KEY_L;
                break;
            case KEY_M:
                glfwKey = GLFW_KEY_M;
                break;
            case KEY_N:
                glfwKey = GLFW_KEY_N;
                break;
            case KEY_O:
                glfwKey = GLFW_KEY_O;
                break;
            case KEY_P:
                glfwKey = GLFW_KEY_P;
                break;
            case KEY_Q:
                glfwKey = GLFW_KEY_Q;
                break;
            case KEY_R:
                glfwKey = GLFW_KEY_R;
                break;
            case KEY_S:
                glfwKey = GLFW_KEY_S;
                break;
            case KEY_T:
                glfwKey = GLFW_KEY_T;
                break;
            case KEY_U:
                glfwKey = GLFW_KEY_U;
                break;
            case KEY_V:
                glfwKey = GLFW_KEY_V;
                break;
            case KEY_W:
                glfwKey = GLFW_KEY_W;
                break;
            case KEY_X:
                glfwKey = GLFW_KEY_X;
                break;
            case KEY_Y:
                glfwKey = GLFW_KEY_Y;
                break;
            case KEY_Z:
                glfwKey = GLFW_KEY_Z;
                break;
            case KEY_0:
                glfwKey = GLFW_KEY_0;
                break;
            case KEY_1:
                glfwKey = GLFW_KEY_1;
                break;
            case KEY_2:
                glfwKey = GLFW_KEY_2;
                break;
            case KEY_3:
                glfwKey = GLFW_KEY_3;
                break;
            case KEY_4:
                glfwKey = GLFW_KEY_4;
                break;
            case KEY_5:
                glfwKey = GLFW_KEY_5;
                break;
            case KEY_6:
                glfwKey = GLFW_KEY_6;
                break;
            case KEY_7:
                glfwKey = GLFW_KEY_7;
                break;
            case KEY_8:
                glfwKey = GLFW_KEY_8;
                break;
            case KEY_9:
                glfwKey = GLFW_KEY_9;
                break;
            case KEY_SPACE:
                glfwKey = GLFW_KEY_SPACE;
                break;
            case KEY_ENTER:
                glfwKey = GLFW_KEY_ENTER;
                break;
            case KEY_TAB:
                glfwKey = GLFW_KEY_TAB;
                break;
            case KEY_BACKSPACE:
                glfwKey = GLFW_KEY_BACKSPACE;
                break;
            case KEY_DELETE:
                glfwKey = GLFW_KEY_DELETE;
                break;
            case KEY_ESCAPE:
                glfwKey = GLFW_KEY_ESCAPE;
                break;
            case KEY_SHIFT:
                glfwKey = GLFW_KEY_LEFT_SHIFT;
                break;
            case KEY_CTRL:
                glfwKey = GLFW_KEY_LEFT_CONTROL;
                break;
            case KEY_ALT:
                glfwKey = GLFW_KEY_LEFT_ALT;
                break;
            case KEY_UP:
                glfwKey = GLFW_KEY_UP;
                break;
            case KEY_DOWN:
                glfwKey = GLFW_KEY_DOWN;
                break;
            case KEY_LEFT:
                glfwKey = GLFW_KEY_LEFT;
                break;
            case KEY_RIGHT:
                glfwKey = GLFW_KEY_RIGHT;
                break;
            case KEY_HOME:
                glfwKey = GLFW_KEY_HOME;
                break;
            case KEY_END:
                glfwKey = GLFW_KEY_END;
                break;
            case KEY_PAGE_UP:
                glfwKey = GLFW_KEY_PAGE_UP;
                break;
            case KEY_PAGE_DOWN:
                glfwKey = GLFW_KEY_PAGE_DOWN;
                break;
            case KEY_F1:
                glfwKey = GLFW_KEY_F1;
                break;
            case KEY_F2:
                glfwKey = GLFW_KEY_F2;
                break;
            case KEY_F3:
                glfwKey = GLFW_KEY_F3;
                break;
            case KEY_F4:
                glfwKey = GLFW_KEY_F4;
                break;
            case KEY_F5:
                glfwKey = GLFW_KEY_F5;
                break;
            case KEY_F6:
                glfwKey = GLFW_KEY_F6;
                break;
            case KEY_F7:
                glfwKey = GLFW_KEY_F7;
                break;
            case KEY_F8:
                glfwKey = GLFW_KEY_F8;
                break;
            case KEY_F9:
                glfwKey = GLFW_KEY_F9;
                break;
            case KEY_F10:
                glfwKey = GLFW_KEY_F10;
                break;
            case KEY_F11:
                glfwKey = GLFW_KEY_F11;
                break;
            case KEY_F12:
                glfwKey = GLFW_KEY_F12;
                break;
            default:
                continue; // Skip unrecognized keys
            }

            bool currentlyDown = glfwGetKey(m_window, glfwKey) == GLFW_PRESS;
            bool wasDown = keyData[i].isDown;

            keyData[i].isPressed =
                currentlyDown && !wasDown;

            keyData[i].isReleased =
                !currentlyDown && wasDown;

            keyData[i].isDown =
                currentlyDown;
        }

        for (int i = 1; i < MouseButton::MOUSE_BUTTON_COUNT; i++)
        {
            if (i <= MouseButton::MOUSE_BUTTON_UNKNOWN || i >= MouseButton::MOUSE_BUTTON_COUNT)
                continue;
            MouseButton button = static_cast<MouseButton>(i);
            int glfwButton = -1;

            switch (button)
            {
            case MouseButton::MOUSE_BUTTON_LEFT:
                glfwButton = GLFW_MOUSE_BUTTON_LEFT;
                break;
            case MouseButton::MOUSE_BUTTON_RIGHT:
                glfwButton = GLFW_MOUSE_BUTTON_RIGHT;
                break;
            case MouseButton::MOUSE_BUTTON_MIDDLE:
                glfwButton = GLFW_MOUSE_BUTTON_MIDDLE;
                break;
            case MouseButton::MOUSE_BUTTON_4:
                glfwButton = GLFW_MOUSE_BUTTON_4;
                break;
            case MouseButton::MOUSE_BUTTON_5:
                glfwButton = GLFW_MOUSE_BUTTON_5;
                break;

            default:
                continue; // Skip unrecognized buttons
            }

            bool currentlyDown = glfwGetMouseButton(m_window, glfwButton) == GLFW_PRESS;
            bool wasDown = mouseStatus.buttons[i].value;

            mouseStatus.buttons[i].isPressed =
                currentlyDown && !wasDown;

            mouseStatus.buttons[i].isReleased =
                !currentlyDown && wasDown;

            mouseStatus.buttons[i].value =
                currentlyDown;
        }
    }

    void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
    {
        // TODO
    }

    // ================================== //
    //      Keyboard Button Functions     //
    // ================================== //

    bool Input::isKeyDown(Key key)
    {
        if (key <= Key::KEY_UNKNOWN || key >= Key::KEY_COUNT)
            return false;

        return keyData[key].isDown;
    }

    bool Input::isKeyPressed(Key key)
    {
        if (key <= Key::KEY_UNKNOWN || key >= Key::KEY_COUNT)
            return false;

        return keyData[key].isPressed;
    }

    bool Input::isKeyReleased(Key key)
    {
        if (key <= Key::KEY_UNKNOWN || key >= Key::KEY_COUNT)
            return false;

        return keyData[key].isReleased;
    }

    // =============================== //
    //      Mouse Button Functions     //
    // =============================== //

    bool Input::isMouseButtonDown(MouseButton key)
    {
        if (key <= MouseButton::MOUSE_BUTTON_UNKNOWN || key >= MouseButton::MOUSE_BUTTON_COUNT)
            return false;

        return mouseStatus.buttons[static_cast<size_t>(key)].value > 0.0f ? true : false;
    }

    bool Input::isMouseButtonPressed(MouseButton key)
    {
        if (key <= MouseButton::MOUSE_BUTTON_UNKNOWN || key >= MouseButton::MOUSE_BUTTON_COUNT)
            return false;

        return mouseStatus.buttons[static_cast<size_t>(key)].isPressed;
    }

    bool Input::isMouseButtonReleased(MouseButton key)
    {
        if (key <= MouseButton::MOUSE_BUTTON_UNKNOWN || key >= MouseButton::MOUSE_BUTTON_COUNT)
            return false;

        return mouseStatus.buttons[static_cast<size_t>(key)].isReleased;
    }

    float Input::getMouseScrollValue()
    {

        return mouseStatus.scrool;
    }
}
