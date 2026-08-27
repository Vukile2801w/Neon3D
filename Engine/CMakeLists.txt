add_subdirectory(lib/glad)


add_library(Engine
    src/Logging.cpp
    src/Transform.cpp
    src/Camera.cpp
    src/Time.cpp
    src/Input.cpp
    src/Application.cpp
    
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
    
    # Scene
    src/Scene/Behavior.cpp
    src/Scene/GameObject.cpp
    src/Scene/Scene.cpp
    
    # Rendering
    src/Rendering/Renderer.cpp
    src/Rendering/Shader.cpp
    src/Rendering/Material.cpp
    src/Rendering/Window.cpp
    src/Rendering/Texture.cpp

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