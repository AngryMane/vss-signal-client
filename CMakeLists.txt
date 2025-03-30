cmake_minimum_required(VERSION 3.16.3)
include(ExternalProject)

# Find Boost
find_package(Boost REQUIRED COMPONENTS system)

set(REPOSITORY_INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/out)
set(REPOSITORY_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external_modules/include)

ExternalProject_add(
    websocketpp    
    GIT_REPOSITORY  https://github.com/zaphoyd/websocketpp.git   
    GIT_TAG         0.8.2
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_SOURCE_DIR}/external_modules
    DEPENDS ${Boost_LIBRARIES}
)

# vss-signal-clientのビルド
add_subdirectory(vss-signal-client)

# sample-appのビルド
add_subdirectory(sample-app)