cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(muse-chain
        VERSION         0.0.1
        DESCRIPTION     "This project is used to build a blockchain consensus mechanism experimental testing platform"
        HOMEPAGE_URL    "https://github.com/muse-chain"
        LANGUAGES       C CXX
)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED  ON)

#[[
    OpenSSL_Include_Directory  --- openssl 头文件所在位置
    OpenSSL_Library_Directory  --- openssl 二进制库所在位置
    ZLIB_Library_Directory     --- zlib 库所在位置
]]#
string(TIMESTAMP CURRENT_DATETIME "%Y/%m/%d %H:%M:%S")

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Build in Linux OS on ${CURRENT_DATETIME} !")
    SET(OpenSSL_Include_Directory "/usr/include/openssl")
    SET(OpenSSL_Library_Directory "/usr/lib/x86_64-linux-gnu")
    SET(ZLIB_Library_Directory "/usr/local/lib")
endif ()

include_directories(include)
include_directories(${OpenSSL_Include_Directory})
include_directories(dependency)
include_directories(dependency/catch2)
include_directories(dependency/spdlog)
include_directories(dependency/concurrent_queue)

add_library(serializer SHARED
        ${CMAKE_SOURCE_DIR}/include/serializer/binarySerializer.cpp
        ${CMAKE_SOURCE_DIR}/include/serializer/util.cpp
        ${CMAKE_SOURCE_DIR}/include/serializer/binaryDeserializeView.cpp
)

add_library(timer SHARED
        ${CMAKE_SOURCE_DIR}/include/timer/timer_wheel.cpp
        ${CMAKE_SOURCE_DIR}/include/timer/timer_tree.cpp
)

add_library(museThreads SHARED
        ${CMAKE_SOURCE_DIR}/include/thread_pool/executor.cpp
        ${CMAKE_SOURCE_DIR}/include/thread_pool/conf.cpp
        ${CMAKE_SOURCE_DIR}/include/thread_pool/pool.cpp
        ${CMAKE_SOURCE_DIR}/include/thread_pool/concurrent_pool.cpp
)

add_library(rpc SHARED
        ${CMAKE_SOURCE_DIR}/include/rpc/protocol/protocol.cpp
        ${CMAKE_SOURCE_DIR}/include/rpc/server/reactor.cpp
        ${CMAKE_SOURCE_DIR}/include/rpc/server/request.cpp
        ${CMAKE_SOURCE_DIR}/include/rpc/server/sub_reactor.cpp
        ${CMAKE_SOURCE_DIR}/include/rpc/server/reactor_exception.cpp
        ${CMAKE_SOURCE_DIR}/include/rpc/server/reactor_pool.cpp
        ${CMAKE_SOURCE_DIR}/include/rpc/server/response.cpp
        ${CMAKE_SOURCE_DIR}/include/rpc/server/registry.cpp
        ${CMAKE_SOURCE_DIR}/include/rpc/server/rpc_response_header.cpp
        ${CMAKE_SOURCE_DIR}/include/rpc/server/middleware_channel.cpp
        ${CMAKE_SOURCE_DIR}/include/rpc/server/route_service.cpp
        ${CMAKE_SOURCE_DIR}/include/rpc/server/zlib_service.cpp
        ${CMAKE_SOURCE_DIR}/include/rpc/server/peer.cpp
        ${CMAKE_SOURCE_DIR}/include/rpc/server/synchronous_registry.cpp
        ${CMAKE_SOURCE_DIR}/include/rpc/server/global_entry.cpp
        ${CMAKE_SOURCE_DIR}/include/rpc/server/transmitter_link_reactor.cpp
        ${CMAKE_SOURCE_DIR}/include/rpc/client/response_data.cpp
        ${CMAKE_SOURCE_DIR}/include/rpc/client/invoker.cpp
        ${CMAKE_SOURCE_DIR}/include/rpc/client/client.cpp
        ${CMAKE_SOURCE_DIR}/include/rpc/client/invoker_exception.cpp
        ${CMAKE_SOURCE_DIR}/include/rpc/client/transmitter.cpp
        ${CMAKE_SOURCE_DIR}/include/rpc/client/outcome.cpp
        ${CMAKE_SOURCE_DIR}/include/rpc/client/transmitter_event.cpp
        ${CMAKE_SOURCE_DIR}/include/rpc/client/global_id.cpp
        ${CMAKE_SOURCE_DIR}/include/rpc/memory/conf.cpp
)

add_library(chain SHARED
        ${CMAKE_SOURCE_DIR}/include/encryption/hash_handler.cpp
        ${CMAKE_SOURCE_DIR}/include/encryption/rsa_handler.cpp
        ${CMAKE_SOURCE_DIR}/include/encryption/encryption.cpp
        ${CMAKE_SOURCE_DIR}/include/db/out_entry.cpp
        ${CMAKE_SOURCE_DIR}/include/db/transaction.cpp
        ${CMAKE_SOURCE_DIR}/include/db/block.cpp
        ${CMAKE_SOURCE_DIR}/include/db/account.cpp
        ${CMAKE_SOURCE_DIR}/include/db/affair.cpp
)

#zlib 不在就G
if (Zlib_Find_Result)
    MESSAGE("-- find zlib in ${Zlib_Find_Result} ----- OK!")
else ()
    MESSAGE(FATAL_ERROR "-- didn't find zlib ----- ERROR!")
endif ()

target_link_libraries(rpc
        PUBLIC serializer museThreads timer z
)

target_link_libraries(chain
        PUBLIC ssl crypto
)

#查找需要的库在不在
find_library(Zlib_Find_Result z
        PATHS ${ZLIB_Library_Directory}
        REQUIRED
)

find_library(openssl_Find_Result ssl
        PATHS ${OpenSSL_Library_Directory}
        VERSION 3.0
        REQUIRED
)

find_library(crypto_Find_Result crypto
        PATHS ${OpenSSL_Library_Directory}
        VERSION 3.0
        REQUIRED
)
#openssl 不在就G
if (openssl_Find_Result)
    MESSAGE("-- find openssl-ssl in ${openssl_Find_Result} ----- OK!")
else ()
    MESSAGE(FATAL_ERROR "-- didn't find openssl ----- ERROR!")
endif ()

#crypto 不在就G
if (crypto_Find_Result)
    MESSAGE("-- find openssl-crypto in ${crypto_Find_Result} ----- OK!")
else ()
    MESSAGE(FATAL_ERROR "-- didn't find openssl ----- ERROR!")
endif ()


add_executable(muse main.cpp)

add_executable(client client.cpp)

add_executable(call bind.cpp)

target_link_libraries(call
        PUBLIC z
)

target_link_libraries(muse
        PUBLIC rpc
        PUBLIC chain
)

target_link_libraries(client
        PUBLIC rpc
)

target_link_libraries(call
        PUBLIC rpc
)

#add thread only linux gcc
set(CMAKE_CXX_FLAGS "${CAMKE_CXX_FLAGS} -std=c++17 -pthread -lcrypto -lssl")

# 启动单元测试
enable_testing()

# 进入单元测试
add_subdirectory(tests)
