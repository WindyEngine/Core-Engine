# ---------------------------------------------
# Sources
# ---------------------------------------------
file(GLOB_RECURSE SANDBOX_SOURCES
    CONFIGURE_DEPENDS
    *.cpp
    *.mm
)

# ---------------------------------------------
# Executable
# ---------------------------------------------
windy_add_executable(sandbox
    SOURCES ${SANDBOX_SOURCES}
    DEPS
        Windy::engine
)

# ---------------------------------------------
# Startup project
# ---------------------------------------------
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    PROPERTY VS_STARTUP_PROJECT sandbox
)