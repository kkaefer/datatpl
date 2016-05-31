set(CMAKE_CXX_FLAGS "-O3 -std=c++14 -Wall -Wextra -fno-rtti")

add_subdirectory(datatpl)

# Example executable that uses libdatatpl:
add_executable(main main.cpp)
target_link_libraries(main datatpl)
