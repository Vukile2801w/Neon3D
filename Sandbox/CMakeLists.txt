add_executable(Sandbox
    src/main.cpp
)


target_link_libraries(Sandbox
    PRIVATE
        Engine
        $<$<CONFIG:Debug>:NEON_DEBUG>
)