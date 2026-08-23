#ifndef NEON_INPUT
#define NEON_INPUT

#include <array>
#include <iostream>

#include "glm.hpp"

#include "Window.hpp"

namespace Neon
{
    class Input
    {
    public:
        enum Key
        {
            KEY_UNKNOWN,
            KEY_A,
            KEY_B,
            KEY_C,
            KEY_D,
            KEY_E,
            KEY_F,
            KEY_G,
            KEY_H,
            KEY_I,
            KEY_J,
            KEY_K,
            KEY_L,
            KEY_M,
            KEY_N,
            KEY_O,
            KEY_P,
            KEY_Q,
            KEY_R,
            KEY_S,
            KEY_T,
            KEY_U,
            KEY_V,
            KEY_W,
            KEY_X,
            KEY_Y,
            KEY_Z,
            KEY_0,
            KEY_1,
            KEY_2,
            KEY_3,
            KEY_4,
            KEY_5,
            KEY_6,
            KEY_7,
            KEY_8,
            KEY_9,
            KEY_SPACE,
            KEY_ENTER,
            KEY_TAB,
            KEY_BACKSPACE,
            KEY_DELETE,
            KEY_ESCAPE,
            KEY_SHIFT,
            KEY_CTRL,
            KEY_ALT,
            KEY_UP,
            KEY_DOWN,
            KEY_LEFT,
            KEY_RIGHT,
            KEY_HOME,
            KEY_END,
            KEY_PAGE_UP,
            KEY_PAGE_DOWN,
            KEY_F1,
            KEY_F2,
            KEY_F3,
            KEY_F4,
            KEY_F5,
            KEY_F6,
            KEY_F7,
            KEY_F8,
            KEY_F9,
            KEY_F10,
            KEY_F11,
            KEY_F12,

            KEY_COUNT
        };
        static constexpr std::array<const char *, KEY_COUNT> keyName = {
            "KEY_UNKNOWN",   // 0
            "KEY_A",         // 1
            "KEY_B",         // 2
            "KEY_C",         // 3
            "KEY_D",         // 4
            "KEY_E",         // 5
            "KEY_F",         // 6
            "KEY_G",         // 7
            "KEY_H",         // 8
            "KEY_I",         // 9
            "KEY_J",         // 10
            "KEY_K",         // 11
            "KEY_L",         // 12
            "KEY_M",         // 13
            "KEY_N",         // 14
            "KEY_O",         // 15
            "KEY_P",         // 16
            "KEY_Q",         // 17
            "KEY_R",         // 18
            "KEY_S",         // 19
            "KEY_T",         // 20
            "KEY_U",         // 21
            "KEY_V",         // 22
            "KEY_W",         // 23
            "KEY_X",         // 24
            "KEY_Y",         // 25
            "KEY_Z",         // 26
            "KEY_0",         // 27
            "KEY_1",         // 28
            "KEY_2",         // 29
            "KEY_3",         // 30
            "KEY_4",         // 31
            "KEY_5",         // 32
            "KEY_6",         // 33
            "KEY_7",         // 34
            "KEY_8",         // 35
            "KEY_9",         // 36
            "KEY_SPACE",     // 37
            "KEY_ENTER",     // 38
            "KEY_TAB",       // 39
            "KEY_BACKSPACE", // 40
            "KEY_DELETE",    // 41
            "KEY_ESCAPE",    // 42
            "KEY_SHIFT",     // 43
            "KEY_CTRL",      // 44
            "KEY_ALT",       // 45
            "KEY_UP",        // 46
            "KEY_DOWN",      // 47
            "KEY_LEFT",      // 48
            "KEY_RIGHT",     // 49
            "KEY_HOME",      // 50
            "KEY_END",       // 51
            "KEY_PAGE_UP",   // 52
            "KEY_PAGE_DOWN", // 53
            "KEY_F1",        // 54
            "KEY_F2",        // 55
            "KEY_F3",        // 56
            "KEY_F4",        // 57
            "KEY_F5",        // 58
            "KEY_F6",        // 59
            "KEY_F7",        // 60
            "KEY_F8",        // 61
            "KEY_F9",        // 62
            "KEY_F10",       // 63
            "KEY_F11",       // 64
            "KEY_F12"        // 65
        };

        struct KeyData
        {
            Key key;
            // status
            bool isDown;
            bool isPressed;
            bool isReleased;
        };

        enum MouseButton
        {
            MOUSE_BUTTON_UNKNOWN,
            MOUSE_BUTTON_LEFT,
            MOUSE_BUTTON_RIGHT,
            MOUSE_BUTTON_MIDDLE,
            MOUSE_BUTTON_4,
            MOUSE_BUTTON_5,
            MOUSE_BUTTON_COUNT
        };
        static constexpr std::array<const char *, MOUSE_BUTTON_COUNT> mouseButtonName = {
            "MOUSE_BUTTON_UNKNOWN", // 0
            "MOUSE_BUTTON_LEFT",    // 1
            "MOUSE_BUTTON_RIGHT",   // 2
            "MOUSE_BUTTON_MIDDLE",  // 3
            "MOUSE_SCROLL_UP",      // 4
            "MOUSE_SCROLL_DOWN"     // 5
        };

        struct MouseButtonData
        {
            MouseButton button;
            float value;     // 0 or 1 for buttons, or scroll amount for scroll wheel
            bool isPressed;  // scroll wheel doesn't have pressed/released, but for buttons it does
            bool isReleased; // same as above
        };

        struct MouseStatus
        {
            MouseButtonData buttons[MOUSE_BUTTON_COUNT];
            glm::vec2 pos;
            double scrool;
        };

        Input(Window *window);
        ~Input();

        void handleInput();

        bool isKeyDown(Key key);
        bool isKeyPressed(Key key);
        bool isKeyReleased(Key key);

        // Can also be used for mouse wheel scroll wheel. True if scrolled in given direction, false if not.
        bool isMouseButtonDown(MouseButton key);
        // Can also be used for mouse wheel scroll wheel. True if scrolled in given direction this frame.
        bool isMouseButtonPressed(MouseButton key);
        // Can also be used for mouse wheel scroll wheel. True if stop scrolling in given direction this frame.
        bool isMouseButtonReleased(MouseButton key);

        float getMouseScrollValue();

    protected:
        std::array<KeyData, KEY_COUNT> m_keyData;
        MouseStatus m_mouseStatus;

    private:
        Input(const Input &) = delete;
        Input &operator=(const Input &) = delete;

        GLFWwindow *m_window;
    };
    inline std::ostream &operator<<(std::ostream &os, Input::Key key)
    {
        return os << Input::keyName[static_cast<int>(key)];
    }
    inline std::ostream &operator<<(std::ostream &os, Input::MouseButton key)
    {
        return os << Input::mouseButtonName[static_cast<int>(key)];
    }
}

#endif // INPUT_HPP