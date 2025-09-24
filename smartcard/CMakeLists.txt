# TODO make tsg-smartcard a dynamic library loadable at runtime

set(TARGET_NAME tsg_smartcard)

add_library(${TARGET_NAME} STATIC 
    source/smartcard_provider_winscard.cpp
    source/card_connection_winscard.cpp
)
target_include_directories(${TARGET_NAME} PUBLIC 
    "include"
    "../base/include"
)
target_link_libraries(${TARGET_NAME}
    ${TSG_MEMORY_MANAGER_LINK_LIB}
)
target_compile_definitions(${TARGET_NAME} PUBLIC ${TSG_MEMORY_MANAGER_DEFINITIONS})
set_target_properties(${TARGET_NAME}
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

set(TARGET_NAME tsg_lua_smartcard)

add_library(${TARGET_NAME} STATIC 
    source/lua_atr.cpp
)
target_include_directories(${TARGET_NAME} PUBLIC 
    "include"
    "../base/include"
    "../../third-party/LuaBridge3"
    "${LUA_INCLUDE_DIR}"
)
target_link_libraries(${TARGET_NAME}
    ${TSG_MEMORY_MANAGER_LINK_LIB}
    ${LUA_LIB}
    tsg_smartcard
)
target_compile_definitions(${TARGET_NAME} PUBLIC ${TSG_MEMORY_MANAGER_DEFINITIONS})
set_target_properties(${TARGET_NAME}
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)