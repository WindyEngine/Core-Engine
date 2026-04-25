# ---------------------------------------------
# Layers
# ---------------------------------------------
add_subdirectory(platform)
add_subdirectory(core)

# ---------------------------------------------
# Aggregate Engine Target
# ---------------------------------------------

add_library(windy_engine INTERFACE)

target_link_libraries(windy_engine INTERFACE
    Windy::platform
    Windy::core
)

add_library(Windy::engine ALIAS windy_engine)