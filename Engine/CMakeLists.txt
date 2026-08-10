add_subdirectory(lib/glad)


add_library(Engine
    src/Window.cpp
    src/Shader.cpp
    src/Texture.cpp
    src/Material.cpp
    # Mesh implementations
    src/Mesh/VertexArray.cpp
    src/Mesh/VertexBuffer.cpp
    src/Mesh/IndexBuffer.cpp
    src/Mesh/Mesh.cpp
)


target_include_directories(Engine
    PUBLIC
        include
        include/Mesh
        
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