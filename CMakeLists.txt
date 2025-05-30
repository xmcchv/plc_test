cmake_minimum_required(VERSION 3.0.2)
project(plctest)

###########
## CMAKE ##
###########
## Compile as C++14, supported in ROS Noetic
# add_compile_options(-std=c++11)
set(CMAKE_BUILD_TYPE RELEASE) #DEBUG  RELEASE
add_definitions(-w)
set(CMAKE_CXX_STANDARD 17)

## define build type to control gdb tool and DEBUG_MODE macro definition
if(CMAKE_BUILD_TYPE STREQUAL "DEBUG")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall  -g -ggdb -std=c++17")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall  -g -ggdb -std=c++17")
    message(STATUS "CMAKE_BUILD_TYPE IS DEBUG")
    add_definitions(-DDEBUG_MODE)
    set(USE_PANGOLIN OFF)
    if (USE_PANGOLIN)
        add_definitions(-DUSE_PANGOLIN)
    endif ()
endif ()
if (CMAKE_BUILD_TYPE STREQUAL "RELEASE")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall  -std=c++17")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++17")
    message(STATUS "CMAKE_BUILD_TYPE IS RELEASE")
endif ()

######################
## macro definition ##
######################


##########################
## catkin and libraries ##
##########################
#========yaml-cpp=================
set(YAML_CPP_INCLUDE_DIR "/usr/local/include/yaml-cpp")
set(YAML_CPP_LIBRARIES "/usr/local/lib/libyaml-cpp.a")


###########
## Build ##
###########
#========output lib path===================
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
# 添加头文件路径
include_directories(
        ${catkin_INCLUDE_DIRS}
        ${PROJECT_SOURCE_DIR}/include/plc
        ${PROJECT_SOURCE_DIR}/include

        ${YAML_CPP_INCLUDE_DIR}
)

# 添加本地CPP文件路径
add_library(${PROJECT_NAME}
        # plc
        ${PROJECT_SOURCE_DIR}/src/plc/snap7.cpp
        ${PROJECT_SOURCE_DIR}/src/plc/plc_reader.cpp
)

# 链接依赖库
target_link_libraries(${PROJECT_NAME}
        ${PROJECT_SOURCE_DIR}/lib/libsnap7.so
        ${YAML_CPP_LIBRARIES}
        -lboost_system 
        -lpthread
)

##############################
## Declare a C++ executable ##
##############################
add_executable(${PROJECT_NAME}_node main_node.cpp)
target_link_libraries(${PROJECT_NAME}_node
        ${PROJECT_NAME}
)



#############
## Install ##
#############
#install(DIRECTORY launch/
#        DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/launch
#)
#
#install(DIRECTORY src/autounloadlog/
#        DESTINATION ${CATKIN_PACKAGE_PYTHON_DESTINATION}
#)