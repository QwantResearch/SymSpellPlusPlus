PROJECT(symspell)

include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
link_directories(/usr/local/lib)
SET(NLP_SRCS
    main.cpp
)


add_executable(autocorrection ${NLP_SRCS})

target_link_libraries(autocorrection symspell)
# install(TARGETS autocorrection DESTINATION bin)
