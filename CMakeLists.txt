cmake_minimum_required(VERSION 3.5)
project(symspell)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lpthread")
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

include_directories(${PROJECT_SOURCE_DIR}/include/ /usr/local/include/ /usr/)

link_directories(/usr/local/lib/ /usr/lib/)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++11 -lpthread")


SET(SOURCES
#  ${CMAKE_SOURCE_DIR}/src/chunkarray.cpp
  ${CMAKE_SOURCE_DIR}/src/editdistance.cpp
  ${CMAKE_SOURCE_DIR}/src/suggestionstage.cpp
  ${CMAKE_SOURCE_DIR}/src/suggestitem.cpp
  ${CMAKE_SOURCE_DIR}/src/symspell.cpp
  ${CMAKE_SOURCE_DIR}/src/utils.cpp
  ${CMAKE_SOURCE_DIR}/src/wordsegmentationitem.cpp
)

# install(TARGETS url-segmenter DESTINATION bin)
add_library(${PROJECT_NAME} ${SOURCES})
# target_link_libraries(${PROJECT_NAME} ${LIBS})

install(TARGETS ${PROJECT_NAME} DESTINATION lib/)
install(DIRECTORY include/ DESTINATION include/qnlp FILES_MATCHING PATTERN "*.h*")
