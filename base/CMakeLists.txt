set(LIB_BASE 
    Thread.cc
)
add_library(base ${LIB_BASE})
target_link_libraries(base pthread rt)
install(TARGETS base DESTINATION base)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/base)
