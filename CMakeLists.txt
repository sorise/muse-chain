cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(muse-chain
        VERSION         0.0.1
        DESCRIPTION     "This project is used to build a blockchain consensus mechanism experimental testing platform"
        HOMEPAGE_URL    "https://github.com/muse-chain"
        LANGUAGES       C CXX
)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED  ON)
set(CMAKE_BUILD_TYPE "Debug")

#解决 visual studio 编码问题
if (WIN32)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif ()

#[[
    OpenSSL_Include_Directory  --- openssl 头文件所在位置
    OpenSSL_Library_Directory  --- openssl 二进制库所在位置
    ZLIB_Library_Directory     --- zlib 库所在位置
]]#
string(TIMESTAMP CURRENT_DATETIME "%Y/%m/%d %H:%M:%S")

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Build in Windows OS on ${CURRENT_DATETIME} !")
    SET(OpenSSL_Include_Directory "C:\\Program Files\\OpenSSL-Win64\\include")
    SET(OpenSSL_Library_Directory "C:\\Program Files\\OpenSSL-Win64\\lib")
    SET(OpenSSL_Link_Directory "C:\\Program Files\\OpenSSL-Win64\\bin")
else(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Build in Linux OS on ${CURRENT_DATETIME} !")
    SET(OpenSSL_Include_Directory "/usr/include/openssl")
    SET(OpenSSL_Library_Directory "/usr/lib/x86_64-linux-gnu")
    SET(ZLIB_Library_Directory "/usr/local/lib")
    #add thread only linux gcc
    set(CMAKE_CXX_FLAGS "${CAMKE_CXX_FLAGS} -std=c++17 -pthread -lcrypto -lssl")
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif ()

#解决 visual studio 编码问题
if (WIN32)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif ()

include_directories(include)
include_directories(${OpenSSL_Include_Directory})
include_directories(dependency)
include_directories(dependency/catch2)
include_directories(dependency/inicpp)
include_directories(dependency/spdlog-1.13.0/include)
include_directories(dependency/leveldb-1.23/include)
include_directories(dependency/concurrent_queue)

link_directories(${OpenSSL_Library_Directory})
MESSAGE(STATUS "OpenSSL Library Directory: ${OpenSSL_Library_Directory}")

link_directories(${ZLIB_Library_Directory})

#levelDB
add_subdirectory(dependency/leveldb-1.23)
add_subdirectory(dependency/spdlog-1.13.0)

add_library(serializer SHARED
    include/serializer/binarySerializer.cpp
    include/serializer/util.cpp
    include/serializer/binaryDeserializeView.cpp
)

add_library(rpc SHARED
    include/rpc/protocol/protocol.cpp
    include/rpc/server/reactor.cpp
    include/rpc/server/request.cpp
    include/rpc/server/sub_reactor.cpp
    include/rpc/server/reactor_exception.cpp
    include/rpc/server/reactor_pool.cpp
    include/rpc/server/response.cpp
    include/rpc/server/registry.cpp
    include/rpc/server/rpc_response_header.cpp
    include/rpc/server/middleware_channel.cpp
    include/rpc/server/route_service.cpp
    include/rpc/server/zlib_service.cpp
    include/rpc/server/peer.cpp
    include/rpc/server/synchronous_registry.cpp
    include/rpc/server/global_entry.cpp
    include/rpc/server/transmitter_link_reactor.cpp
    include/rpc/client/response_data.cpp
    include/rpc/client/invoker.cpp
    include/rpc/client/client.cpp
    include/rpc/client/invoker_exception.cpp
    include/rpc/client/transmitter.cpp
    include/rpc/client/outcome.cpp
    include/rpc/client/transmitter_event.cpp
    include/rpc/client/global_id.cpp
    include/rpc/memory/conf.cpp
    include/thread_pool/executor.cpp
    include/thread_pool/conf.cpp
    include/thread_pool/pool.cpp
    include/thread_pool/concurrent_pool.cpp
    include/timer/timer_wheel.cpp
    include/timer/timer_tree.cpp
    include/rpc/rpc.cpp
)

add_library(chain SHARED
    include/encryption/hash_handler.cpp
    include/encryption/rsa_handler.cpp
    include/encryption/encryption.cpp
    include/encryption/ecc_sm2_handler.cpp
    include/encryption/ecc_secp256k1.cpp
    include/db/out_entry.cpp
    include/db/transaction.cpp
    include/db/block.cpp
    include/db/account.cpp
    include/db/affair.cpp
    include/db/merkle_tree.cpp
    include/db/Trie.cpp
    include/db/patricia_trie.cpp
    include/db/connector_account.cpp
    include/db/bloom_filter.cpp
    include/db/environment_db.cpp
    include/db/chain_db_exception.cpp
    include/db/receipt.cpp
    include/db/rollback_operation.cpp
    include/application/application.cpp
    include/application/application_state.cpp
    include/db/connector_block.cpp
    include/db/leveldb_account_commit.hpp
    include/db/leveldb_account_commit.cpp
    include/db/bloom_filter_store.cpp
    include/db/block_header.cpp
    include/db/block_body.cpp
    include/util/singleton.cpp
    include/util/timebox.cpp
)

#查找需要的库在不在
find_library(Zlib_Find_Result z
    PATHS ${ZLIB_Library_Directory}
    REQUIRED
)

find_library(openssl_Find_Result ssl
    PATHS "${OpenSSL_Library_Directory}"
    REQUIRED
)

find_library(crypto_Find_Result crypto
    PATHS "${OpenSSL_Library_Directory}"
    REQUIRED
)

#zlib 不在就G
if (Zlib_Find_Result)
    MESSAGE("-- find zlib in ${Zlib_Find_Result} ----- OK!")
else ()
    MESSAGE(FATAL_ERROR "-- didn't find zlib ----- ERROR!")
endif ()
#openssl 不在就G
if (openssl_Find_Result)
    MESSAGE("-- find openssl-ssl in ${openssl_Find_Result} ----- OK!")
else ()
    MESSAGE(FATAL_ERROR "-- didn't find openssl-ssl ----- ERROR!")
endif ()

#crypto 不在就G
if (crypto_Find_Result)
    MESSAGE("-- find openssl-crypto in ${crypto_Find_Result} ----- OK!")
else ()
    MESSAGE(FATAL_ERROR "-- didn't find openssl-crypto ----- ERROR!")
endif ()

target_link_libraries(rpc
    PUBLIC serializer z
    PRIVATE spdlog::spdlog
)

target_link_libraries(chain
    PUBLIC ssl crypto rpc leveldb
)

add_executable(muse main.cpp)

add_executable(client client.cpp)

add_executable(call bind.cpp)

target_link_libraries(call
    PUBLIC z
)

target_link_libraries(muse
    PUBLIC chain
)

target_link_libraries(client
    PUBLIC rpc
)

target_link_libraries(call
    PUBLIC rpc
)

# 启动单元测试
enable_testing()

# 进入单元测试
add_subdirectory(tests)