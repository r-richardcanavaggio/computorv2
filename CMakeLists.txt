cmake_minimum_required(VERSION 3.12)

project(computorv2
    VERSION 1.0
    DESCRIPTION "Mathematical Interpreter & Computer Algebra Engine"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(computorv2
    srcs/main.cpp
    srcs/Types/Real.cpp
    srcs/Types/Complex.cpp
    srcs/Types/Polynomial.cpp
    srcs/Parser/Nodes.cpp
    srcs/Parser/ParserPrimary.cpp
    srcs/Parser/Parser.cpp
    srcs/Parser/TokenStream.cpp
    srcs/Interpreter/Commands.cpp
    srcs/Interpreter/Evaluator.cpp
    srcs/Interpreter/EvaluatorNormalize.cpp
    srcs/Interpreter/Interpreter.cpp
)

target_link_libraries(computorv2 PRIVATE readline)

target_include_directories(computorv2
    PUBLIC
        includes
        srcs/Types/
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(computorv2 PRIVATE -g -O0 -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(computorv2 PRIVATE -fsanitize=address,undefined)
endif()
