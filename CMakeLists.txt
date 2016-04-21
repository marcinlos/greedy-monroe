project("Greedy Monroe" CXX)

set(CMAKE_CXX_FLAGS "-O2 -std=c++14")

add_executable(monroe
  monroe.cpp)

add_executable(generator
  generator.cpp)



