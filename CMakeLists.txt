cmake_minimum_required(VERSION 3.0.0)
project(lag-smartcard VERSION 0.1.0)

option(USE_MEMORY_MANAGER "Use Memory Manager" ON) 
option(USE_STRESS_MEMORY_MANAGER "Use Stress Memory Manager" OFF) 

if(USE_MEMORY_MANAGER)
    set(FLAG_USE_MEMORY_MANAGER ON)
endif(USE_MEMORY_MANAGER)
unset(USE_MEMORY_MANAGER CACHE) 

if(USE_STRESS_MEMORY_MANAGER)
    set(FLAG_USE_STRESS_MEMORY_MANAGER ON)
endif(USE_STRESS_MEMORY_MANAGER)
unset(USE_STRESS_MEMORY_MANAGER CACHE) 

# ============================================================================
# BUILD CONFIGURATION
# ----------------------------------------------------------------------------

set(CMAKE_CXX_STANDARD 17 )
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_STANDARD 17 )
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(PLATFORM_STR "x64")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
  set(PLATFORM_STR "x32")
endif ()

set(ROOT_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/../${PLATFORM_STR}/${CMAKE_BUILD_TYPE})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${ROOT_OUTPUT_DIRECTORY}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${ROOT_OUTPUT_DIRECTORY}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ROOT_OUTPUT_DIRECTORY}/bin)

if(MSVC)
    # Avoid link warning error LNK4098 with default library LIBCMT
    # or do this set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
endif()

# ============================================================================
# DEPENDENCIES
# ----------------------------------------------------------------------------

# Base library

if(FLAG_USE_MEMORY_MANAGER)
    set(TSG_MEMORY_MANAGER_DEFINITIONS "TSG_USE_MEMORY_MANAGER")
    if(FLAG_USE_STRESS_MEMORY_MANAGER)
        set(TSG_MEMORY_MANAGER_LINK_LIB "tsg_mmgr_stress")
    else()
        set(TSG_MEMORY_MANAGER_LINK_LIB "tsg_mmgr")
    endif(FLAG_USE_STRESS_MEMORY_MANAGER)
else()
    set(TSG_MEMORY_MANAGER_DEFINITIONS "")
    set(TSG_MEMORY_MANAGER_LINK_LIB "")
endif(FLAG_USE_MEMORY_MANAGER)


# WINSCARD dependency

set(WINSCARD_LIB "winscard")

# Lua

set(LUA_DIR "${CMAKE_SOURCE_DIR}/../../../lua/lua-5.4.2_Win32_vc16_lib")
set(LUA_INCLUDE_DIR "${LUA_DIR}/include")
set(LUA_LIB "${LUA_DIR}/lua54.lib")

if(EXISTS ${LUA_LIB})
  message("LUA_LIB found ${LUA_LIB}")
else()
message("LUA_LIB NOT found ${LUA_LIB}")
endif()

# LuaBridge3

# ============================================================================
# FUNCTIONS
# ----------------------------------------------------------------------------

function(symbolic_link_to_build_dir target_name origin destination)
   add_custom_command(TARGET ${target_name}  POST_BUILD
       COMMAND ${CMAKE_COMMAND} -E create_symlink
           ${origin}
           $<TARGET_FILE_DIR:${target_name}>/${destination})
endfunction()

function(copy_to_target_build_dir target_name)
    add_custom_command(TARGET ${target_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${ARGN}
            $<TARGET_FILE_DIR:${target_name}>)
endfunction()