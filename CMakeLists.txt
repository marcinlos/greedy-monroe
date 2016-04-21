project("Greedy Monroe" CXX)

set(CMAKE_CXX_FLAGS "-g -DNDEBUG -std=c++14")

add_executable(monroe
  monroe.cpp
)

