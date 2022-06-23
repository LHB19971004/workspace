cmake_minimum_required(VERSION 2.8)
MESSAGE(STATUS "operation system is ${CMAKE_SYSTEM}")
project (e6_809_client)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED program_options filesystem serialization thread locale)
include_directories( ${Boost_INCLUDE_DIR} )
include_directories( $ENV{E6GPS_DLL_SRC} )
include_directories( $ENV{E6GPS_DLL_SRC}/third_party/jsoncpp_master/include )
include_directories( ${PROJECT_SOURCE_DIR}/include )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/hiredis )
include_directories( $ENV{E6GPS_DLL_SRC}/third_party/SimpleAmqpClient-master/src/ )
include_directories( /root/librdkafka-master/src-cpp )
link_directories( ${CMAKE_CURRENT_SOURCE_DIR}/hiredis )
link_directories(
                $ENV{E6GPS_DLL_SRC}/bin
                $ENV{E6GPS_DLL_SRC}/bin/rabbitmq_writer
                $ENV{E6GPS_DLL_SRC}/bin/gzip_codec
                $ENV{E6GPS_DLL_SRC}/bin/common_fun
                $ENV{E6GPS_DLL_SRC}/third_party/jsoncpp_master/bin/src/lib_json
                $ENV{E6GPS_DLL_SRC}/third_party/SimpleAmqpClient-master/bin/
                $ENV{E6GPS_DLL_SRC}/third_party/rabbitmq-c-master/bin/librabbitmq
                ${PROJECT_SOURCE_DIR}/lib
                /root/librdkafka-master/src
                /root/librdkafka-master/src-cpp
        )


add_definitions(-std=c++1y)
add_definitions(-DELPP_NO_DEFAULT_LOG_FILE)
add_definitions(-DELPP_THREAD_SAFE)
add_definitions(-DELPP_FORCE_USE_STD_THREAD)
add_definitions(-DBOOST_UUID_RANDOM_PROVIDER_FORCE_POSIX)
add_definitions(-Wno-reserved-user-defined-literal)
add_definitions(-Wno-register)
add_definitions(-Wno-literal-suffix)

file(GLOB ALL_SRC
                *.h
                *.cpp
        )

aux_source_directory(. ALL_SRC)
add_executable(e6_809_client ${ALL_SRC})

target_link_libraries(e6_809_client 
                curl
                -lz
                pthread
                -static-libgcc -ldl -static-libstdc++
                -lcrypto -lssl
                librabbitmq_writer.a
                libjsoncpp.a
                libSimpleAmqpClient.a
                librabbitmq.so
                libgzip_codec.a
                libcommon_fun.a
                librdkafka++.a
                librdkafka.a
                ${Boost_LIBRARIES})

