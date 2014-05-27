set(TEST_SRCS
test.cpp
)
add_executable(run_tests ${TEST_SRCS})
target_link_libraries(run_tests cserial)
