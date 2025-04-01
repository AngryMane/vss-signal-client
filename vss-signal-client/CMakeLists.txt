# enable json
include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

# enable uuid
find_package(PkgConfig REQUIRED)
pkg_check_modules(UUID REQUIRED uuid)

# find Boost
find_package(Boost REQUIRED COMPONENTS system)

# find OpenSSL
find_package(OpenSSL REQUIRED)

# declare target library
set(CLIENT_NAME vss-signal-client)
file(GLOB_RECURSE SRC_FILES LIST_DIRECTORIES false src/*)
add_library(${CLIENT_NAME} SHARED ${SRC_FILES})

# configure include relations 
set(LOCAL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_include_directories(${CLIENT_NAME} AFTER PUBLIC ${LOCAL_INCLUDE_DIR})
target_include_directories(${CLIENT_NAME} AFTER PUBLIC ${REPOSITORY_INCLUDE_DIR})
target_include_directories(${CLIENT_NAME} AFTER PUBLIC ${Boost_INCLUDE_DIRS})
target_include_directories(${CLIENT_NAME} AFTER PUBLIC ${WEBSOCKETPP_INCLUDE_DIR})
target_include_directories(${CLIENT_NAME} AFTER PUBLIC ${OPENSSL_INCLUDE_DIR})

# configure compile features
target_compile_features(${CLIENT_NAME} PRIVATE cxx_std_17)
target_compile_definitions(${CLIENT_NAME} PRIVATE INSTALL_DIR=\"${SERVICE_INSTALL_DIR}\")

# configure external dependencies
target_link_libraries(${CLIENT_NAME} PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(${CLIENT_NAME} PRIVATE ${UUID_LIBRARIES})
target_link_libraries(${CLIENT_NAME} PRIVATE ${Boost_LIBRARIES})
target_link_libraries(${CLIENT_NAME} PRIVATE OpenSSL::SSL OpenSSL::Crypto)

# configure public header
set_target_properties(${CLIENT_NAME} PROPERTIES PUBLIC_HEADER "inc/signal-client.h")

# configure install
install(TARGETS ${CLIENT_NAME} 
    LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/out
    ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/out
)
