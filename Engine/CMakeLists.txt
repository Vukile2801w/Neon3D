add_subdirectory(lib/glad)


add_library(Engine
    src/Application.cpp
    src/Transform.cpp
    src/Logging.cpp
    src/Camera.cpp
    src/Input.cpp
    src/Time.cpp
    
    # Mesh implementations
    src/Mesh/VertexBuffer.cpp
    src/Mesh/VertexArray.cpp
    src/Mesh/IndexBuffer.cpp
    src/Mesh/MeshParser.cpp
    src/Mesh/Mesh.cpp
    
    # Events
    src/Events/MouseButtonReleasedEvent.cpp
    src/Events/MouseButtonPressedEvent.cpp
    src/Events/WindowLostFocusEvent.cpp
    src/Events/WindowResizedEvent.cpp
    src/Events/MouseScrolledEvent.cpp
    src/Events/WindowFocusedEvent.cpp
    src/Events/WindowClosedEvent.cpp
    src/Events/KeyReleasedEvent.cpp
    src/Events/KeyPressedEvent.cpp
    src/Events/MouseMovedEvent.cpp
    
    # Scene
    src/Scene/GameObject.cpp
    src/Scene/GameObjectFactory.cpp
    src/Scene/SceneSerializer.cpp
    src/Scene/Behavior.cpp
    src/Scene/Scene.cpp
    
    # Rendering
    src/Rendering/ShaderStage.cpp
    src/Rendering/Renderer.cpp
    src/Rendering/Material.cpp
    src/Rendering/Texture.cpp
    src/Rendering/Shader.cpp
    src/Rendering/Window.cpp

    # Assets
    src/Assets/AssetManager.cpp



    # lib
    lib/fast_obj/fast_obj.cpp
)


target_include_directories(Engine
    PUBLIC
        include
        # 'lib' holds nlohmann/json.hpp - moved from PRIVATE to PUBLIC because
        # GameObject.hpp (a public header) now references nlohmann::json for the
        # onSerialize() hook, and any consumer overriding it (e.g. Sandbox's Cube)
        # needs the same include path. Treated the same way lib/glm already is:
        # a third-party type a public header genuinely needs, not a backend to hide -
        # see CONTRIBUTING.md §12.
        lib/nlohmann
        lib/fast_obj
        lib/glm

    PRIVATE
        lib/glfw/include
        lib/stb
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