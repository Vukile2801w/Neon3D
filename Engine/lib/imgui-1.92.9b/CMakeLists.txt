add_library(GUI
    imgui.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
    imgui_demo.cpp

    backends/imgui_impl_glfw.cpp
    backends/imgui_impl_opengl3.cpp
)


target_include_directories(GUI
    PUBLIC
        include

    PRIVATE
        ../glfw/include
)


target_link_libraries(GUI
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../glfw/libglfw3.a
        GLAD
)