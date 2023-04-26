cmake_minimum_required(VERSION 3.18)

project(slook_test VERSION 0.1.0)

include(cmake_helpers/BuildOptions.cmake)
include(deps.cmake)

add_executable(slook_test src/main.cpp)

add_subdirectory(slook)

target_add_default_build_options(slook_test PUBLIC)

target_link_libraries(slook_test PUBLIC Threads::Threads fmt::fmt CLI11::CLI11
                                        slook::slook ${boost_targets})
