set(TARGET_NAME tsg_mmgr)
add_library(${TARGET_NAME} STATIC source/mmgr.cpp)
target_include_directories(${TARGET_NAME} PUBLIC "include")
target_compile_definitions(${TARGET_NAME} PUBLIC TSG_USE_MEMORY_MANAGER)
set_target_properties(${TARGET_NAME}
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)


set(TARGET_NAME tsg_mmgr_stress)
add_library(${TARGET_NAME} STATIC source/mmgr.cpp)
target_include_directories(${TARGET_NAME} PUBLIC "include")
target_compile_definitions(${TARGET_NAME} PUBLIC TSG_USE_MEMORY_MANAGER STRESS_TEST)
set_target_properties(${TARGET_NAME}
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)
