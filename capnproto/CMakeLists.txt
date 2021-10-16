set(capnproto_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external/capnproto)
set(capnproto_INCLUDE ${capnproto_PREFIX}/include)

set(capnproto_CONFIGURE_COMMAND_EXTRA "")

#ExternalProject_Add(
#        capnproto_dep
#        PREFIX ${capnproto_PREFIX}
#        URL "https://github.com/capnproto/capnproto/archive/refs/tags/v0.9.1.tar.gz"
#        URL_MD5 "3ba09702784e9ae1d53a5d9de23c6dd7"
#        CONFIGURE_COMMAND ${CMAKE_COMMAND} -DCMAKE_INSTALL_PREFIX=${capnproto_PREFIX}
#            -DCMAKE_BUILD_TYPE=Release 
#            <SOURCE_DIR>/cmake
#        LOG_UPDATE ON
#        LOG_CONFIGURE ON
#        LOG_BUILD ON
#)

set(CAPNPROTO_LIBRARIES ${capnproto_PREFIX}/lib/libcapnp.a ${capnproto_PREFIX}/lib/libkj.a)
set(CAPNPROTO_GENERATOR ${capnproto_PREFIX}/bin/capnp)
set(CAPNPROTO_CPP_GENERATOR ${capnproto_PREFIX}/bin/capnpc-c++)


set(capnproto_GENERATOR ${capnproto_PREFIX}/bin/protoc)
#set(capnproto_LIB ${capnproto_PREFIX}/build/c++/src/capnp/libcapnp$<$<CONFIG:Debug>:d>${CMAKE_STATIC_LIBRARY_SUFFIX} ${capnproto_PREFIX}/build/c++/src/capnp/libkj$<$<CONFIG:Debug>:d>${CMAKE_STATIC_LIBRARY_SUFFIX})
set(capnproto_LIB ${capnproto_PREFIX}/build/c++/src/capnp/libcapnp${CMAKE_STATIC_LIBRARY_SUFFIX} ${capnproto_PREFIX}/build/c++/src/capnp/libkj${CMAKE_STATIC_LIBRARY_SUFFIX})
set(capnproto_SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/../../capnproto)
set(capnproto_SCHEMA_FILE ${capnproto_SOURCE_DIR}/monster.capnp)
set(capnproto_GENERATED_FILES ${capnproto_SOURCE_DIR}/monster.capnp.h ${capnproto_SOURCE_DIR}/monster.capnp.c++)

file(GLOB ExampleFiles ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
list(REMOVE_ITEM ExampleFiles ${capnproto_GENERATED_FILES})
foreach(ExampleFile ${ExampleFiles})
    get_filename_component(ExampleName ${ExampleFile} NAME_WE)

    add_executable(${ExampleName} ${ExampleFile} ${capnproto_GENERATED_FILES})
    add_dependencies(${ExampleName} capnproto)
    target_include_directories(${ExampleName} PUBLIC ${capnproto_INCLUDE})
    target_link_libraries(${ExampleName} PRIVATE Testing::core ${capnproto_LIB})
    add_test(NAME test_${ExampleName} COMMAND ${ExampleName})
endforeach()
