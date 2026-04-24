cmake_minimum_required(VERSION 3.16)

project(computorv2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SFML 2.5.1 COMPONENTS graphics window system QUIET)

if (SFML_FOUND)
	message(STATUS "✅ SFML 2.5 trouvée sur le système (Mode École)")
else()
	message(STATUS "⚠️ SFML 2.5 non trouvée. Téléchargement en cours... (Mode Mac)")
	include(FetchContent)
	FetchContent_Declare(SFML
    	GIT_REPOSITORY https://github.com/SFML/SFML.git
   		GIT_TAG 2.5.1
	)
	set(SFML_BUILD_AUDIO OFF CACHE BOOL "Ne pas compiler l'audio" FORCE)
	set(SFML_BUILD_NETWORK OFF CACHE BOOL "Ne pas compiler le réseau" FORCE)
	FetchContent_MakeAvailable(SFML)
	set(SFML_LIBRARIES sfml-graphics sfml-window sfml-system)
	set(SFML_INCLUDE_DIR ${sfml_SOURCE_DIR}/include)
endif()

add_executable(computorv2
    srcs/main.cpp
    srcs/Parser.cpp
    srcs/Lexer.cpp
    srcs/Plotter.cpp
    srcs/Fraction.cpp

    srcs/Interpreter/Interpreter.cpp
    srcs/Interpreter/InterpreterEvaluator.cpp
    srcs/Interpreter/InterpreterCommandHandler.cpp
    srcs/Interpreter/TokenPreprocessor.cpp

    srcs/Types/Complex/Complex_core.cpp
    srcs/Types/Complex/Complex_free_ops.cpp
    srcs/Types/Complex/Complex_io.cpp
    srcs/Types/Complex/Complex_ops_addsub.cpp
    srcs/Types/Complex/Complex_ops_divmod.cpp
    srcs/Types/Complex/Complex_ops_mul.cpp
    srcs/Types/Complex/Complex_pow.cpp

    srcs/Types/Real.cpp

    srcs/Types/Polynomial/Polynomial_core.cpp
    srcs/Types/Polynomial/Polynomial_eval.cpp
    srcs/Types/Polynomial/Polynomial_free_ops.cpp
    srcs/Types/Polynomial/Polynomial_io.cpp
    srcs/Types/Polynomial/Polynomial_ops_addsub.cpp
    srcs/Types/Polynomial/Polynomial_ops_divmod.cpp
    srcs/Types/Polynomial/Polynomial_ops_mul.cpp
    srcs/Types/Polynomial/Polynomial_pow.cpp
    srcs/Types/Polynomial/Polynomial_solve.cpp

    srcs/Visitors/BinaryOpVisitor.cpp
    srcs/Visitors/UnaryOpVisitor.cpp

    srcs/Nodes/BinaryOpNode.cpp
    srcs/Nodes/UnaryOpNode.cpp
    srcs/Nodes/MatrixNode.cpp
    srcs/Nodes/ImaginaryNode.cpp
    srcs/Nodes/VariableNode.cpp
    srcs/Nodes/NumberNode.cpp
    srcs/Nodes/FunctionCallNode.cpp
    srcs/Nodes/BuiltinFunctionNode.cpp

    srcs/Maths/Maths_arithmetic.cpp
    srcs/Maths/Maths_exponential.cpp
    srcs/Maths/Maths_trig.cpp
)

target_include_directories(computorv2 PRIVATE
    includes
    includes/Types
    includes/Nodes
    includes/Visitors
    includes/Interpreter
	${SFML_INCLUDE_DIR}
)

target_compile_options(computorv2 PRIVATE
    -Wall
    -Wextra
    -Werror
)

target_link_libraries(computorv2 PRIVATE
	readline
	${SFML_LIBRARIES}
)
