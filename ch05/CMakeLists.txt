set(src_ch05
    LogStream.cc
    LogFile.cc
    AsyncLogging.cc
)
add_library(ch05 ${src_ch05})
target_link_libraries(ch05 base)
install(TARGETS ch05 DESTINATION ch05)
