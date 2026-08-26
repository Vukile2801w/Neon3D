add_executable(Sandbox
    src/main.cpp
)

target_include_directories(Sandbox
    PUBLIC
        include
)

target_link_libraries(Sandbox
    PRIVATE
        Engine
)

target_link_options(Sandbox
    PRIVATE
        -static-libgcc
        -static-libstdc++
        -static
)

target_compile_definitions(Sandbox
    PRIVATE
        $<$<CONFIG:Debug>:NEON_DEBUG>
)

add_custom_command(TARGET Sandbox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:Sandbox>/Sandbox/assets"
)