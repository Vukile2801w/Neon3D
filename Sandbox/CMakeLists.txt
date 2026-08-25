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

target_compile_definitions(Sandbox
    PRIVATE
        $<$<CONFIG:Debug>:NEON_DEBUG>
)