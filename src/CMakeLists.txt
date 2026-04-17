# ---------------------------------------------
# Platform
# ---------------------------------------------
add_subdirectory(platform)

# ---------------------------------------------
# Aggregate Engine Target
# ---------------------------------------------

add_library(windy_engine INTERFACE)

target_link_libraries(windy_engine INTERFACE
    Windy::platform
)

add_library(Windy::engine ALIAS windy_engine)