add_executable(2.a.out NonRecursiveMutex.cc)
target_link_libraries(2.a.out pthread)

add_executable(2.b.out SelfDeadLock.cc)
target_link_libraries(2.b.out pthread)
