cmake_minimum_required(VERSION 2.6)
PROJECT(steerd)

include_directories(dstructs ubus ${steerd_SOURCE_DIR})

ADD_DEFINITIONS(-Os -Wall -Werror --std=gnu99)

SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")

SET(SOURCES
        main.c

        mac.c
        mac.h
        steerd_debug.c
        steerd_debug.h

        dstructs/aplist.c
        dstructs/aplist.h
        dstructs/stalist.c
        dstructs/stalist.h

        ubus/ubus.c
        ubus/ubus.h)

SET(LIBS
        ubox ubus json-c blobmsg_json uci iwinfo)

ADD_EXECUTABLE(steerd ${SOURCES})

TARGET_LINK_LIBRARIES(steerd ${LIBS})

INSTALL(TARGETS steerd
        RUNTIME DESTINATION /usr/sbin/)