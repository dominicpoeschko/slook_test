include(cmake_helpers/FindOrFetch.cmake)
include(FetchContent)

find_package(Threads REQUIRED)

find_package(Boost 1.81 QUIET COMPONENTS system)

if(NOT Boost_FOUND)
  set(BOOST_INCLUDE_LIBRARIES system asio)
  set(BOOST_ENABLE_CMAKE ON)
  FetchContent_Declare(
    Boost
    GIT_REPOSITORY https://github.com/boostorg/boost.git
    GIT_TAG boost-1.81.0
    GIT_SHALLOW TRUE)
  FetchContent_MakeAvailable(Boost)
  set(boost_targets Boost::system)
else()
  set(boost_targets Boost::system)
endif()

find_or_fetch_package(fmt 9 GIT_REPOSITORY https://github.com/fmtlib/fmt.git
                      GIT_TAG master)

find_or_fetch_package(CLI11 2 GIT_REPOSITORY https://github.com/CLIUtils/CLI11
                      GIT_TAG main)
