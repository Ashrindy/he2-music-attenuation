cmake_minimum_required(VERSION 3.28)
project(he2-music-attenuation VERSION 0.0.2
                 DESCRIPTION "he2-music-attenuation"
                 LANGUAGES CXX)

set(PROJECT_AUTHOR "Ashrindy")
set(PROJECT_LONG_DESCRIPTION "Mutes in-game music when external media is played")
set(PROJECT_COPYRIGHT "Copyright (c) 2026 Ashrindy - Licensed under GPL3")
set(PROJECT_AUTHOR_URL "https://github.com/Ashrindy")
set(PROJECT_UPDATE_SERVER_BASE "https://raw.githubusercontent.com/Ashrindy/hmm-update/main")
option(INSTALL_TO_UPDATE_SERVER "Install to update server" ON)
set(UPDATE_SERVER_PATH "D:\\hmm-update" CACHE PATH "Path to update server")
string(TIMESTAMP PROJECT_BUILD_DATE "%Y-%m-%d")

set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>")

set(SDK_VAR_wars "SONIC_FORCES_SDK")
set(GAME_FOLDER_DEFAULT_wars "E:\\Steam\\steamapps\\common\\SonicForces")
set(GAME_EXECUTABLE_wars "build/main/projects/exec/Sonic Forces.exe")
set(GAME_WORKING_DIRECTORY_wars "build/main/projects/exec")
set(MODS_SUBFOLDER_wars "Mods")
set(UPDATE_SERVER_NAMESPACE_wars "wars")
set(FRIENDLY_GAME_NAME_wars "Sonic Forces")

set(SDK_VAR_rangers "SONIC_FRONTIERS_SDK")
set(GAME_FOLDER_DEFAULT_rangers "E:\\Steam\\steamapps\\common\\SonicFrontiers")
set(GAME_EXECUTABLE_rangers "SonicFrontiers.exe")
set(GAME_WORKING_DIRECTORY_rangers "")
set(MODS_SUBFOLDER_rangers "Mods")
set(UPDATE_SERVER_NAMESPACE_rangers "rangers")
set(FRIENDLY_GAME_NAME_rangers "Sonic Frontiers")

set(SDK_VAR_miller "MILLER_SDK")
set(GAME_FOLDER_DEFAULT_miller "E:\\Steam\\steamapps\\common\\SONIC_X_SHADOW_GENERATIONS")
set(GAME_EXECUTABLE_miller "SONIC_X_SHADOW_GENERATIONS.exe")
set(GAME_WORKING_DIRECTORY_miller "")
set(MODS_SUBFOLDER_miller "mods_shadow")
set(UPDATE_SERVER_NAMESPACE_miller "miller")
set(FRIENDLY_GAME_NAME_miller "Shadow Generations")

if(INSTALL_TO_UPDATE_SERVER)
    cmake_path(SET UPDATE_SERVER_PATH_NORMALIZED NORMALIZE ${UPDATE_SERVER_PATH})
    message("Install to update server enabled. Update server path is ${UPDATE_SERVER_PATH}")
endif()

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/archives")

add_subdirectory(vendor EXCLUDE_FROM_ALL)

foreach(TARGET_SDK wars rangers miller)
    if(DEFINED ENV{${SDK_VAR_${TARGET_SDK}}})
        message("${SDK_VAR_${TARGET_SDK}} environment variable found. Using this version instead of the local submodule.")
        cmake_path(SET SDK_PATH NORMALIZE $ENV{${SDK_VAR_${TARGET_SDK}}})
    else()
        set(SDK_PATH sdks/${TARGET_SDK}-sdk)
    endif()

    add_subdirectory(${SDK_PATH} ${TARGET_SDK}-sdk EXCLUDE_FROM_ALL)
    add_subdirectory(mod ${TARGET_SDK}/mod)
endforeach()
