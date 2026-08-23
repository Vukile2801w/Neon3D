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
        $<$<CONFIG:Debug>:NEON_DEBUG>
)