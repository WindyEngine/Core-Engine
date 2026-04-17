# ---------------------------------------------
# Global Language Standards
# ---------------------------------------------
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------
# Per-Target Enforcement
# ---------------------------------------------
function(windy_apply_standard_settings target)
    target_compile_features(${target} PUBLIC c_std_17)
    target_compile_features(${target} PUBLIC cxx_std_20)
endfunction()

# ---------------------------------------------
# Safety Check
# ---------------------------------------------
if(CMAKE_CXX_STANDARD LESS 20)
    message(FATAL_ERROR "WindyEngine requires at least C++20")
endif()