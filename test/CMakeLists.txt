add_library(testutils testutils.cpp wrapper.c)
target_include_directories(testutils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(test_kompress test_kompress.cpp)
target_link_libraries(test_kompress kompresslib testutils gtest_main)
#add_test(NAME test_kompress COMMAND test_kompress)

add_executable(test_mmap test_mmap.cpp)
target_link_libraries(test_mmap testutils gtest_main)

include(GoogleTest)
gtest_discover_tests(test_kompress)
