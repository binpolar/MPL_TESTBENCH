cmake_minimum_required(VERSION 3.10.0)
project(mpltestbench VERSION 0.1.0 LANGUAGES C)



file(GLOB MPL
     "MPL/*.c"
)
file(GLOB cplus
     "MPL/cplus/src/*.c"
)
include_directories(MPL/cplus/include)
add_executable(mpltestbench main.c ${MPL} ${cplus})

