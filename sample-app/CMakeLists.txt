cmake_minimum_required(VERSION 3.16.3)

# Find required packages
find_package(Boost REQUIRED COMPONENTS system)
find_package(OpenSSL REQUIRED)

# Add executable
add_executable(sample-app src/main.cpp)

# Link libraries
target_link_libraries(sample-app PRIVATE vss-signal-client)
target_link_libraries(sample-app PRIVATE ${Boost_LIBRARIES})
target_link_libraries(sample-app PRIVATE OpenSSL::SSL OpenSSL::Crypto)

# Include directories
target_include_directories(sample-app PRIVATE ${CMAKE_SOURCE_DIR}/vss-signal-client/inc)
target_include_directories(sample-app PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(sample-app PRIVATE ${OPENSSL_INCLUDE_DIR})

# Add dependency on vss-signal-client
add_dependencies(sample-app vss-signal-client)

# Install binary to out directory
install(TARGETS sample-app
    RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/out
)

# Set rpath to find the shared library
set_target_properties(sample-app PROPERTIES
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "${CMAKE_SOURCE_DIR}/out"
) 