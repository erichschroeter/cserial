set(RX_SRCS
rx.c
)
add_executable(rx ${RX_SRCS})
target_link_libraries(rx cserial rt)

set(TX_SRCS
tx.c
)
add_executable(tx ${TX_SRCS})
target_link_libraries(tx cserial rt)

set(TEST_SRCS
test.cpp
)
add_executable(run_tests ${TEST_SRCS})
target_link_libraries(run_tests cserial)
