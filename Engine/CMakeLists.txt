add_subdirectory(lib/glad)


add_library(Engine
    src/Window.cpp
    src/Shader.cpp
    src/VertexArray.cpp
    src/VertexBuffer.cpp
    src/IndexBuffer.cpp
)


target_include_directories(Engine
    PUBLIC
        include

    PRIVATE
        lib/glfw/include
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