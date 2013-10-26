add_executable(2.a.out NonRecursiveMutex.cc)
target_link_libraries(2.a.out base)

add_executable(2.b.out SelfDeadLock.cc)
target_link_libraries(2.b.out base)

add_executable(2.c.out MutualDeadLock.cc)
target_link_libraries(2.c.out base)

add_executable(2.d.out BlockingQueueTest.cc)
target_link_libraries(2.d.out base)
