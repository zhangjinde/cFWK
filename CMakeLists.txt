cmake_minimum_required(VERSION 2.8.9)
project(cFWK)
set(CMAKE_BUILD_TYPE Debug)
#message($ENV{CXXFLAGS})
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")  
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")  
include_directories(.)
if( DEFINED ENV{CROSS_FILE})
    include($ENV{CROSS_FILE})
endif()
file(GLOB tmp_SRCS "cf_allocator/*.c")
list(APPEND cFWK_SRCS ${tmp_SRCS})
file(GLOB tmp_SRCS "cf_test/*.c")
list(APPEND cFWK_SRCS ${tmp_SRCS})
file(GLOB tmp_SRCS "cf_module/*.c")
list(APPEND cFWK_SRCS ${tmp_SRCS})
file(GLOB tmp_SRCS "cf_collection/*.c")
list(APPEND cFWK_SRCS ${tmp_SRCS})
file(GLOB tmp_SRCS "cf_factory/*.c")
list(APPEND cFWK_SRCS ${tmp_SRCS})
file(GLOB tmp_SRCS "cf_websocket/*.c")
list(APPEND cFWK_SRCS ${tmp_SRCS})
file(GLOB tmp_SRCS "cf_util/*.c")
list(APPEND cFWK_SRCS ${tmp_SRCS})
file(GLOB tmp_SRCS "cf_net/*.c")
list(APPEND cFWK_SRCS ${tmp_SRCS})
message(${cFWK_SRCS})
#set(cFWK_SRCS cf_allocator/cf_allocator_simple.c )
add_library(cFWK STATIC ${cFWK_SRCS})

file(GLOB test_SRCS "cf_test_case/*.c") 
ADD_EXECUTABLE(test_top ${test_SRCS}  )
#message(${CMAKE_CURRENT_SOURCE_DIR})
#message(${CMAKE_CURRENT_BINARY_DIR})
#message(${PROJECT_BINARY_DIR})
set_target_properties(
    cFWK
    PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/lib
    )
set_target_properties(
    test_top
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin
    )


target_link_libraries(test_top cFWK)
install (TARGETS cFWK DESTINATION ${PROJECT_BINARY_DIR}/install/lib)
install (TARGETS test_top DESTINATION ${PROJECT_BINARY_DIR}/install/bin)


file(GLOB tmp_INCS "cf_allocator/*.h")
list(APPEND cFWK_INCS ${tmp_INCS})
install (FILES  ${tmp_INCS} DESTINATION ${PROJECT_BINARY_DIR}/install/include/cf_allocator/ )

