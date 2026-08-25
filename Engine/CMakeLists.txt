add_subdirectory(lib/glad)


add_library(Engine
    src/Window.cpp
    src/Shader.cpp
    src/Texture.cpp
    src/Material.cpp
    src/Logging.cpp
    src/Transform.cpp
    src/Camera.cpp
    src/Time.cpp
    src/Input.cpp
    src/Application.cpp
    src/Renderer.cpp

    # Mesh implementations
    src/Mesh/VertexArray.cpp
    src/Mesh/VertexBuffer.cpp
    src/Mesh/IndexBuffer.cpp
    src/Mesh/Mesh.cpp

    # Events
    src/Events/MouseMovedEvent.cpp
    src/Events/MouseButtonPressedEvent.cpp
    src/Events/MouseButtonReleasedEvent.cpp
    src/Events/MouseScrolledEvent.cpp
    src/Events/KeyPressedEvent.cpp
    src/Events/KeyReleasedEvent.cpp
    src/Events/WindowClosedEvent.cpp
    src/Events/WindowResizedEvent.cpp
    src/Events/WindowFocusedEvent.cpp
    src/Events/WindowLostFocusEvent.cpp
)


target_include_directories(Engine
    PUBLIC
        include
        
    PRIVATE
        lib/glfw/include
        lib/stb

    PUBLIC
        lib/glm
)


target_link_libraries(Engine
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw/libglfw3.a
        GLAD
        opengl32
        gdi32
        user32
        shell32
)