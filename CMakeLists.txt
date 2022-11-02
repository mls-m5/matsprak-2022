
cmake_minimum_required(VERSION 3.23)
project(matsprak-2022)

add_executable(
    matsprak-2022
    src/main.cpp
    src/token.cpp
    src/parse/parsemodule.cpp
    src/parse/state.cpp
    src/codegen/il/codegenil.cpp
    src/codegen/cpp/codegencpp.cpp
    )

target_include_directories(
    matsprak-2022
    PRIVATE
    src
)

target_compile_features(
    matsprak-2022
    PRIVATE
    cxx_std_17
    )

find_package(Threads)
target_link_libraries(
    matsprak-2022
    PRIVATE
    ${CMAKE_THREAD_LIBS_INIT}
    )

file(
    COPY
    demo
    DESTINATION
    .
)
