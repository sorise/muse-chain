cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

add_executable(transaction_ut db/transaction_test.cpp)
add_executable(affair_ut db/affair_test.cpp)
add_executable(base_binary_ut db/base_binary_test.cpp)
add_executable(merkle_tree_ut db/merkle_tree_test.cpp)

target_link_libraries(transaction_ut
        PUBLIC chain rpc
)

target_link_libraries(affair_ut
        PUBLIC chain rpc
)

target_link_libraries(base_binary_ut
        PUBLIC chain rpc
)

target_link_libraries(merkle_tree_ut
        PUBLIC chain serializer
)

add_test( NAME "transaction_ut" COMMAND transaction_ut )
add_test( NAME "affair_ut" COMMAND affair_ut )
add_test( NAME "base_binary_ut" COMMAND base_binary_ut )
add_test( NAME "merkle_tree_ut" COMMAND merkle_tree_ut )
