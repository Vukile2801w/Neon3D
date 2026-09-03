#ifndef NEON_INPUT
#define NEON_INPUT

#include <array>
#include <iostream>

#include "Events/EventBus.hpp"

#include "glm.hpp"

namespace Neon
{
    class Input
    {
    public:
        enum Key
        {
            KeyUnknown,
            KeyA,
            KeyB,
            KeyC,
            KeyD,
            KeyE,
            KeyF,
            KeyG,
            KeyH,
            KeyI,
            KeyJ,
            KeyK,
            KeyL,
            KeyM,
            KeyN,
            KeyO,
            KeyP,
            KeyQ,
            KeyR,
            KeyS,
            KeyT,
            KeyU,
            KeyV,
            KeyW,
            KeyX,
            KeyY,
            KeyZ,
            Key0,
            Key1,
            Key2,
            Key3,
            Key4,
            Key5,
            Key6,
            Key7,
            Key8,
            Key9,
            KeySpace,
            KeyEnter,
            KeyTab,
            KeyBackspace,
            KeyDelete,
            KeyEscape,
            KeyShift,
            KeyCtrl,
            KeyAlt,
            KeyUp,
            KeyDown,
            KeyLeft,
            KeyRight,
            KeyHome,
            KeyEnd,
            KeyPageUp,
            KeyPageDOWN,
            KeyF1,
            KeyF2,
            KeyF3,
            KeyF4,
            KeyF5,
            KeyF6,
            KeyF7,
            KeyF8,
            KeyF9,
            KeyF10,
            KeyF11,
            KeyF12,
            KeyNumPlus,
            KeyNumMinus,
            KeyNumSlash,
            KeyNumAsterisk,
            KeyNumEnter,

            KeyCount
        };
        static constexpr std::array<const char *, KeyCount> keyName = {
            "KeyUnknown",   // 0
            "KeyA",         // 1
            "KeyB",         // 2
            "KeyC",         // 3
            "KeyD",         // 4
            "KeyE",         // 5
            "KeyF",         // 6
            "KeyG",         // 7
            "KeyH",         // 8
            "KeyI",         // 9
            "KeyJ",         // 10
            "KeyK",         // 11
            "KeyL",         // 12
            "KeyM",         // 13
            "KeyN",         // 14
            "KeyO",         // 15
            "KeyP",         // 16
            "KeyQ",         // 17
            "KeyR",         // 18
            "KeyS",         // 19
            "KeyT",         // 20
            "KeyU",         // 21
            "KeyV",         // 22
            "KeyW",         // 23
            "KeyX",         // 24
            "KeyY",         // 25
            "KeyZ",         // 26
            "Key0",         // 27
            "Key1",         // 28
            "Key2",         // 29
            "Key3",         // 30
            "Key4",         // 31
            "Key5",         // 32
            "Key6",         // 33
            "Key7",         // 34
            "Key8",         // 35
            "Key9",         // 36
            "KeySpace",     // 37
            "KeyEnter",     // 38
            "KeyTab",       // 39
            "KeyBackspace", // 40
            "KeyDelete",    // 41
            "KeyEscape",    // 42
            "KeyShift",     // 43
            "KeyCtrl",      // 44
            "KeyAlt",       // 45
            "KeyUp",        // 46
            "KeyDown",      // 47
            "KeyLeft",      // 48
            "KeyRight",     // 49
            "KeyHome",      // 50
            "KeyEnd",       // 51
            "KeyPageUp",    // 52
            "KeyPageDown",  // 53
            "KeyF1",        // 54
            "KeyF2",        // 55
            "KeyF3",        // 56
            "KeyF4",        // 57
            "KeyF5",        // 58
            "KeyF6",        // 59
            "KeyF7",        // 60
            "KeyF8",        // 61
            "KeyF9",        // 62
            "KeyF10",       // 63
            "KeyF11",       // 64
            "KeyF12",       // 65

            "KeyNumPlus",
            "KeyNumMinus",
            "KeyNumSlash",
            "KeyNumAsterisk",
            "KeyNumEnter",
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
            MouseButtonUnknown,
            MouseButtonLeft,
            MouseButtonRight,
            MouseButtonMiddle,
            MouseButton4,
            MouseButton5,
            MouseButtonCount
        };
        static constexpr std::array<const char *, MouseButtonCount> mouseButtonName = {
            "MouseButtonUnknown", // 0
            "MouseButtonLeft",    // 1
            "MouseButtonRight",   // 2
            "MouseButtonMiddle",  // 3
            "MouseButton4",       // 4
            "MouseButton5"        // 5
        };

        static const char *toString(MouseButton mouseButton)
        {
            return mouseButtonName[mouseButton];
        }

        static const char *toString(Key key)
        {
            return keyName[key];
        }

        struct MouseButtonData
        {
            MouseButton button;
            bool isDown;     // 0 or 1 for buttons
            bool isPressed;  // scroll wheel doesn't have pressed/released, but for buttons it does
            bool isReleased; // same as above
        };

        struct MouseStatus
        {
            MouseButtonData buttons[MouseButtonCount];

            glm::vec2 pos{0.0f};
            glm::vec2 scroll{0.0f};
        };

        enum class CursorMode
        {
            Normal,
            Hidden,
            Disabled
        };

        Input(EventBus &eventBus);
        ~Input() = default;

        void handleInput();

        bool isKeyDown(Key key) const;
        bool isKeyPressed(Key key) const;
        bool isKeyReleased(Key key) const;

        bool isMouseButtonDown(MouseButton key) const;
        bool isMouseButtonPressed(MouseButton key) const;
        bool isMouseButtonReleased(MouseButton key) const;

        glm::vec2 getMousePosition() const;

        float getScrollX() const;
        float getScrollY() const;
        glm::vec2 getScroll() const;

        void onKey(int key, int scancode, int action, int mods); // WARNING : Only engine internal use intended
        void onMouseMovement(double x, double y);                // WARNING : Only engine internal use intended
        void onMouseButton(int button, int action, int mods);    // WARNING : Only engine internal use intended
        void onMouseScroll(double xOffset, double yOffset);      // WARNING : Only engine internal use intended

        void setCursorMode(CursorMode mode) { m_cursorMode = mode; };
        CursorMode getCursorMode() const { return m_cursorMode; };

    protected:
        std::array<KeyData, KeyCount> m_keyData;
        MouseStatus m_mouseStatus;

    private:
        Input(const Input &) = delete;
        Input &operator=(const Input &) = delete;

        static int toGLFWKey(Key key);
        static Key fromGLFWKey(int key);

        static int toGLFWMouseButton(MouseButton button);
        static MouseButton fromGLFWMouseButton(int button);

        EventBus &m_eventBus;

        CursorMode m_cursorMode = CursorMode::Normal;
    };
}

#endif // INPUT_HPP