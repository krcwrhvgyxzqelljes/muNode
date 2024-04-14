cmake_minimum_required(VERSION 3.5)

project(interpreter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories("usr/local/lib")
include_directories("usr/local/include")

add_executable(interpreter
    main.cpp
    munode.cpp
    munode.h
    mutext.cpp
    mutext.h
    )

# Link the muparser library if it's required
target_link_libraries(interpreter muparser -lrt)
