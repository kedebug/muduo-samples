add_executable(1.a.out Observer.cc)
target_link_libraries(1.a.out base)

add_executable(1.b.out ObserverSafe.cc)
target_link_libraries(1.b.out base)

add_executable(1.c.out StockFactory.cc)
target_link_libraries(1.c.out base)
