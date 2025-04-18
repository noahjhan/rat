cmake_minimum_required(VERSION 3.10)
project(Rat CXX)

# set(CMAKE_GENERATOR "Ninja")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER "clang++")

# execute_process(
#     COMMAND 'llvm-config --cxxflags --includedir --ldflags --system-libs --libs'
#     RESULT_VARIABLE llvm_config_result
#     OUTPUT_VARIABLE llvm_config_flags
#     ERROR_VARIABLE llvm_config_error
# )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -O0")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -O3 ${llvm_config_flags}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")

set(SRC_DIR src)
set(INC_DIR includes)
set(TEST_DIR tests)
set(BIN_DIR bin)
set(DATA_DIR data)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/${BIN_DIR})
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/${DATA_DIR})

add_custom_target(data ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/${DATA_DIR} ${CMAKE_BINARY_DIR}/${DATA_DIR}
)

# Source Files
set(SOURCES
    ${SRC_DIR}/rat_source.cpp
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/lexer.cpp
    ${SRC_DIR}/parser.cpp
    ${SRC_DIR}/symbol_table.cpp
    ${SRC_DIR}/assembly.cpp
    ${SRC_DIR}/semantic.cpp
    ${TEST_DIR}/test.cpp
)

include_directories(${INC_DIR})

set(EXECUTABLE_NAME rat)
set(EXECUTABLE ${BIN_DIR}/${EXECUTABLE_NAME})

add_executable(${EXECUTABLE_NAME} ${SOURCES})
add_dependencies(${EXECUTABLE_NAME} data)
