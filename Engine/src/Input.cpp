#include "Input.hpp"

#include "Assert.hpp"
#include "Events/EventBus.hpp"
#include "Events/KeyPressedEvent.hpp"
#include "Events/KeyReleasedEvent.hpp"
#include "Events/MouseButtonPressedEvent.hpp"
#include "Events/MouseButtonReleasedEvent.hpp"
#include "Events/MouseMovedEvent.hpp"
#include "Events/MouseScrolledEvent.hpp"

#include "glad/glad.h"
#include "glfw/glfw3.h"

namespace Neon
{
    Input::Input(EventBus &eventBus) : m_eventBus(eventBus)
    {

        for (int i = 1; i < Key::KeyCount; i++)
        {
            m_keyData[i] = KeyData{static_cast<Key>(i),
                                   false,
                                   false,
                                   false};
        }
        for (int i = 1; i < MouseButton::MouseButtonCount; i++)
        {
            m_mouseStatus.buttons[i] = MouseButtonData{static_cast<MouseButton>(i),
                                                       false,
                                                       false,
                                                       false};
        }
    }

    void Input::handleInput()
    {
        m_mouseStatus.scroll = {0.0f, 0.0f};

        for (auto &data : m_keyData)
        {
            data.isPressed = false;
            data.isReleased = false;
        }

        for (auto &data : m_mouseStatus.buttons)
        {
            data.isPressed = false;
            data.isReleased = false;
        }

        glfwPollEvents();
    }

    // ================================== //
    //      Keyboard Button Functions     //
    // ================================== //

    bool Input::isKeyDown(Key key) const
    {
        if (key <= Key::KeyUnknown || key >= Key::KeyCount)
            return false;

        return m_keyData[key].isDown;
    }

    bool Input::isKeyPressed(Key key) const
    {
        if (key <= Key::KeyUnknown || key >= Key::KeyCount)
            return false;

        return m_keyData[key].isPressed;
    }

    bool Input::isKeyReleased(Key key) const
    {
        if (key <= Key::KeyUnknown || key >= Key::KeyCount)
            return false;

        return m_keyData[key].isReleased;
    }

    // =============================== //
    //      Mouse Button Functions     //
    // =============================== //

    bool Input::isMouseButtonDown(MouseButton key) const
    {
        if (key <= MouseButton::MouseButtonUnknown || key >= MouseButton::MouseButtonCount)
            return false;

        return m_mouseStatus.buttons[static_cast<size_t>(key)].isDown;
    }

    bool Input::isMouseButtonPressed(MouseButton key) const
    {
        if (key <= MouseButton::MouseButtonUnknown || key >= MouseButton::MouseButtonCount)
            return false;

        return m_mouseStatus.buttons[static_cast<size_t>(key)].isPressed;
    }

    bool Input::isMouseButtonReleased(MouseButton key) const
    {
        if (key <= MouseButton::MouseButtonUnknown || key >= MouseButton::MouseButtonCount)
            return false;

        return m_mouseStatus.buttons[static_cast<size_t>(key)].isReleased;
    }

    glm::vec2 Input::getScroll() const
    {
        return m_mouseStatus.pos;
    }

    glm::vec2 Input::getMousePosition() const
    {
        return m_mouseStatus.pos;
    }

    // =============================== //
    //         Events Functions        //
    // =============================== //

    void Input::onKey(
        int key, int scancode,
        int action, int mods)
    {
        Key neonKey = fromGLFWKey(key);

        if (neonKey == Key::KeyUnknown)
            NEON_ASSERT(
                neonKey != Key::KeyUnknown,
                "Input received an unknown GLFW key");

        switch (action)
        {
        case GLFW_PRESS:
            m_keyData[static_cast<int>(neonKey)].isDown = true;
            m_keyData[static_cast<int>(neonKey)].isPressed = true;

            m_eventBus.emit(
                KeyPressedEvent(neonKey, false));
            break;

        case GLFW_REPEAT:
            m_eventBus.emit(
                KeyPressedEvent(neonKey, true));
            break;

        case GLFW_RELEASE:
            m_keyData[static_cast<int>(neonKey)].isDown = false;
            m_keyData[static_cast<int>(neonKey)].isReleased = true;

            m_eventBus.emit(
                KeyReleasedEvent(neonKey));
            break;

        default:
            NEON_ASSERT(
                false,
                "Input::onKey received an invalid GLFW action");
            break;
        }
    }

    void Input::onMouseButton(
        int button,
        int action,
        int mods)
    {
        MouseButton neonButton = fromGLFWMouseButton(button);
        NEON_ASSERT(
            neonButton != MouseButton::MouseButtonUnknown,
            "Input received an unknown GLFW mouse button");

        auto &data =
            m_mouseStatus.buttons[static_cast<int>(neonButton)];

        switch (action)
        {
        case GLFW_PRESS:
            data.isDown = true;
            data.isPressed = true;

            m_eventBus.emit(
                MouseButtonPressedEvent(neonButton));
            break;

        case GLFW_RELEASE:
            data.isDown = false;
            data.isReleased = true;

            m_eventBus.emit(
                MouseButtonReleasedEvent(neonButton));
            break;

        default:
            NEON_ASSERT(
                false,
                "Input::onMouseButton received an invalid GLFW action");
            break;
        }
    }

    void Input::onMouseMovement(
        double x,
        double y)
    {
        glm::vec2 position(
            static_cast<float>(x),
            static_cast<float>(y));

        glm::vec2 delta =
            position - m_mouseStatus.pos;

        m_mouseStatus.pos = position;

        m_eventBus.emit(
            MouseMovedEvent(position, delta));
    }

    void Input::onMouseScroll(
        double xOffset,
        double yOffset)
    {

        glm::vec2 offset(
            static_cast<float>(xOffset),
            static_cast<float>(yOffset));

        m_mouseStatus.scroll += offset;

        m_eventBus.emit(
            MouseScrolledEvent(offset));
    }

    // =============================== //
    //         Helper Functions        //
    // =============================== //

    int Input::toGLFWKey(Key key)
    {
        switch (key)
        {
        case Key::KeyA:
            return GLFW_KEY_A;
        case Key::KeyB:
            return GLFW_KEY_B;
        case Key::KeyC:
            return GLFW_KEY_C;
        case Key::KeyD:
            return GLFW_KEY_D;
        case Key::KeyE:
            return GLFW_KEY_E;
        case Key::KeyF:
            return GLFW_KEY_F;
        case Key::KeyG:
            return GLFW_KEY_G;
        case Key::KeyH:
            return GLFW_KEY_H;
        case Key::KeyI:
            return GLFW_KEY_I;
        case Key::KeyJ:
            return GLFW_KEY_J;
        case Key::KeyK:
            return GLFW_KEY_K;
        case Key::KeyL:
            return GLFW_KEY_L;
        case Key::KeyM:
            return GLFW_KEY_M;
        case Key::KeyN:
            return GLFW_KEY_N;
        case Key::KeyO:
            return GLFW_KEY_O;
        case Key::KeyP:
            return GLFW_KEY_P;
        case Key::KeyQ:
            return GLFW_KEY_Q;
        case Key::KeyR:
            return GLFW_KEY_R;
        case Key::KeyS:
            return GLFW_KEY_S;
        case Key::KeyT:
            return GLFW_KEY_T;
        case Key::KeyU:
            return GLFW_KEY_U;
        case Key::KeyV:
            return GLFW_KEY_V;
        case Key::KeyW:
            return GLFW_KEY_W;
        case Key::KeyX:
            return GLFW_KEY_X;
        case Key::KeyY:
            return GLFW_KEY_Y;
        case Key::KeyZ:
            return GLFW_KEY_Z;

        case Key::Key0:
            return GLFW_KEY_0;
        case Key::Key1:
            return GLFW_KEY_1;
        case Key::Key2:
            return GLFW_KEY_2;
        case Key::Key3:
            return GLFW_KEY_3;
        case Key::Key4:
            return GLFW_KEY_4;
        case Key::Key5:
            return GLFW_KEY_5;
        case Key::Key6:
            return GLFW_KEY_6;
        case Key::Key7:
            return GLFW_KEY_7;
        case Key::Key8:
            return GLFW_KEY_8;
        case Key::Key9:
            return GLFW_KEY_9;

        case Key::KeySpace:
            return GLFW_KEY_SPACE;
        case Key::KeyEnter:
            return GLFW_KEY_ENTER;
        case Key::KeyTab:
            return GLFW_KEY_TAB;
        case Key::KeyBackspace:
            return GLFW_KEY_BACKSPACE;
        case Key::KeyDelete:
            return GLFW_KEY_DELETE;
        case Key::KeyEscape:
            return GLFW_KEY_ESCAPE;

        case Key::KeyShift:
            return GLFW_KEY_LEFT_SHIFT;
        case Key::KeyCtrl:
            return GLFW_KEY_LEFT_CONTROL;
        case Key::KeyAlt:
            return GLFW_KEY_LEFT_ALT;

        case Key::KeyUp:
            return GLFW_KEY_UP;
        case Key::KeyDown:
            return GLFW_KEY_DOWN;
        case Key::KeyLeft:
            return GLFW_KEY_LEFT;
        case Key::KeyRight:
            return GLFW_KEY_RIGHT;

        case Key::KeyHome:
            return GLFW_KEY_HOME;
        case Key::KeyEnd:
            return GLFW_KEY_END;
        case Key::KeyPageUp:
            return GLFW_KEY_PAGE_UP;
        case Key::KeyPageDOWN:
            return GLFW_KEY_PAGE_DOWN;

        case Key::KeyF1:
            return GLFW_KEY_F1;
        case Key::KeyF2:
            return GLFW_KEY_F2;
        case Key::KeyF3:
            return GLFW_KEY_F3;
        case Key::KeyF4:
            return GLFW_KEY_F4;
        case Key::KeyF5:
            return GLFW_KEY_F5;
        case Key::KeyF6:
            return GLFW_KEY_F6;
        case Key::KeyF7:
            return GLFW_KEY_F7;
        case Key::KeyF8:
            return GLFW_KEY_F8;
        case Key::KeyF9:
            return GLFW_KEY_F9;
        case Key::KeyF10:
            return GLFW_KEY_F10;
        case Key::KeyF11:
            return GLFW_KEY_F11;
        case Key::KeyF12:
            return GLFW_KEY_F12;

        default:
            return -1;
        }
    }
    Input::Key Input::fromGLFWKey(int key)
    {
        switch (key)
        {
        case GLFW_KEY_A:
            return Key::KeyA;
        case GLFW_KEY_B:
            return Key::KeyB;
        case GLFW_KEY_C:
            return Key::KeyC;
        case GLFW_KEY_D:
            return Key::KeyD;
        case GLFW_KEY_E:
            return Key::KeyE;
        case GLFW_KEY_F:
            return Key::KeyF;
        case GLFW_KEY_G:
            return Key::KeyG;
        case GLFW_KEY_H:
            return Key::KeyH;
        case GLFW_KEY_I:
            return Key::KeyI;
        case GLFW_KEY_J:
            return Key::KeyJ;
        case GLFW_KEY_K:
            return Key::KeyK;
        case GLFW_KEY_L:
            return Key::KeyL;
        case GLFW_KEY_M:
            return Key::KeyM;
        case GLFW_KEY_N:
            return Key::KeyN;
        case GLFW_KEY_O:
            return Key::KeyO;
        case GLFW_KEY_P:
            return Key::KeyP;
        case GLFW_KEY_Q:
            return Key::KeyQ;
        case GLFW_KEY_R:
            return Key::KeyR;
        case GLFW_KEY_S:
            return Key::KeyS;
        case GLFW_KEY_T:
            return Key::KeyT;
        case GLFW_KEY_U:
            return Key::KeyU;
        case GLFW_KEY_V:
            return Key::KeyV;
        case GLFW_KEY_W:
            return Key::KeyW;
        case GLFW_KEY_X:
            return Key::KeyX;
        case GLFW_KEY_Y:
            return Key::KeyY;
        case GLFW_KEY_Z:
            return Key::KeyZ;

        case GLFW_KEY_0:
            return Key::Key0;
        case GLFW_KEY_1:
            return Key::Key1;
        case GLFW_KEY_2:
            return Key::Key2;
        case GLFW_KEY_3:
            return Key::Key3;
        case GLFW_KEY_4:
            return Key::Key4;
        case GLFW_KEY_5:
            return Key::Key5;
        case GLFW_KEY_6:
            return Key::Key6;
        case GLFW_KEY_7:
            return Key::Key7;
        case GLFW_KEY_8:
            return Key::Key8;
        case GLFW_KEY_9:
            return Key::Key9;

        case GLFW_KEY_SPACE:
            return Key::KeySpace;
        case GLFW_KEY_ENTER:
            return Key::KeyEnter;
        case GLFW_KEY_TAB:
            return Key::KeyTab;
        case GLFW_KEY_BACKSPACE:
            return Key::KeyBackspace;
        case GLFW_KEY_DELETE:
            return Key::KeyDelete;
        case GLFW_KEY_ESCAPE:
            return Key::KeyEscape;

        case GLFW_KEY_LEFT_SHIFT:
        case GLFW_KEY_RIGHT_SHIFT:
            return Key::KeyShift;

        case GLFW_KEY_LEFT_CONTROL:
        case GLFW_KEY_RIGHT_CONTROL:
            return Key::KeyCtrl;

        case GLFW_KEY_LEFT_ALT:
        case GLFW_KEY_RIGHT_ALT:
            return Key::KeyAlt;

        case GLFW_KEY_UP:
            return Key::KeyUp;
        case GLFW_KEY_DOWN:
            return Key::KeyDown;
        case GLFW_KEY_LEFT:
            return Key::KeyLeft;
        case GLFW_KEY_RIGHT:
            return Key::KeyRight;

        case GLFW_KEY_HOME:
            return Key::KeyHome;
        case GLFW_KEY_END:
            return Key::KeyEnd;
        case GLFW_KEY_PAGE_UP:
            return Key::KeyPageUp;
        case GLFW_KEY_PAGE_DOWN:
            return Key::KeyPageDOWN;

        case GLFW_KEY_F1:
            return Key::KeyF1;
        case GLFW_KEY_F2:
            return Key::KeyF2;
        case GLFW_KEY_F3:
            return Key::KeyF3;
        case GLFW_KEY_F4:
            return Key::KeyF4;
        case GLFW_KEY_F5:
            return Key::KeyF5;
        case GLFW_KEY_F6:
            return Key::KeyF6;
        case GLFW_KEY_F7:
            return Key::KeyF7;
        case GLFW_KEY_F8:
            return Key::KeyF8;
        case GLFW_KEY_F9:
            return Key::KeyF9;
        case GLFW_KEY_F10:
            return Key::KeyF10;
        case GLFW_KEY_F11:
            return Key::KeyF11;
        case GLFW_KEY_F12:
            return Key::KeyF12;

        default:
            return Key::KeyUnknown;
        }
    }

    int Input::toGLFWMouseButton(MouseButton button)
    {
        switch (button)
        {
        case MouseButton::MouseButtonLeft:
            return GLFW_MOUSE_BUTTON_LEFT;

        case MouseButton::MouseButtonRight:
            return GLFW_MOUSE_BUTTON_RIGHT;

        case MouseButton::MouseButtonMiddle:
            return GLFW_MOUSE_BUTTON_MIDDLE;

        case MouseButton::MouseButton4:
            return GLFW_MOUSE_BUTTON_4;

        case MouseButton::MouseButton5:
            return GLFW_MOUSE_BUTTON_5;

        default:
            return -1;
        }
    }
    Input::MouseButton Input::fromGLFWMouseButton(int button)
    {
        switch (button)
        {
        case GLFW_MOUSE_BUTTON_LEFT:
            return MouseButton::MouseButtonLeft;

        case GLFW_MOUSE_BUTTON_RIGHT:
            return MouseButton::MouseButtonRight;

        case GLFW_MOUSE_BUTTON_MIDDLE:
            return MouseButton::MouseButtonMiddle;

        case GLFW_MOUSE_BUTTON_4:
            return MouseButton::MouseButton4;

        case GLFW_MOUSE_BUTTON_5:
            return MouseButton::MouseButton5;

        default:
            return MouseButton::MouseButtonUnknown;
        }
    }
}