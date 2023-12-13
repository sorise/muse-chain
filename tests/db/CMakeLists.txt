cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

add_executable(merkle_tree_ut merkle_tree_test.cpp)

add_executable(patricia_trie_ut patricia_trie_test.cpp)

add_executable(merkle_patricia_trie_ut merkle_patricia_trie_test.cpp)

add_executable(connector_account_ut connector_account_test.cpp)

add_executable(receipt_ut receipt_test.cpp)

add_executable(insert_block_ut insert_block_ut.cpp)

target_link_libraries(insert_block_ut
        PUBLIC chain serializer
)

target_link_libraries(merkle_tree_ut
        PUBLIC chain serializer
)

target_link_libraries(patricia_trie_ut
        PUBLIC chain serializer
)

target_link_libraries(merkle_patricia_trie_ut
        PUBLIC chain serializer
)

target_link_libraries(connector_account_ut
        PUBLIC chain serializer
)

target_link_libraries(receipt_ut
        PUBLIC chain serializer
)

add_test( NAME "merkle_tree_ut" COMMAND merkle_tree_ut )

add_test( NAME "patricia_trie_ut" COMMAND patricia_trie_ut )

add_test( NAME "merkle_patricia_trie_ut" COMMAND merkle_patricia_trie_ut )

add_test( NAME "connector_account_ut" COMMAND connector_account_ut )

add_test( NAME "receipt_ut" COMMAND receipt_ut )

add_test( NAME "insert_block_ut" COMMAND insert_block_ut )


