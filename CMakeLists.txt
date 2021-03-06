cmake_minimum_required(VERSION 3.10)
project(arm11_22)

set(CMAKE_CXX_STANDARD 11)

include_directories(src)
include_directories(src/effects)
include_directories(src/opc)
include_directories(src/parson)
include_directories(src/utils)
include_directories(src)
include_directories(src/assemble)
include_directories(src/emulate)
include_directories(src/utils)
include_directories(test/lib/json_pure-1.5.3/ext/json/ext/generator)
include_directories(test/lib/json_pure-1.5.3/ext/json/ext/parser)
include_directories(/usr/local/Cellar//openssl/1.0.2o_2/include)

add_executable(arm11_22
        src/effects/common.c
        src/effects/common.h
        src/effects/image.c
        src/effects/image.h
        src/effects/scroller.c
        src/effects/scroller.h
        src/effects/sunrise.c
        src/effects/sunrise.h
        src/effects/alltest.c
        src/effects/alltest.h
        src/effects/1test.c
        src/effects/1test.h
        src/effects/weather.c
        src/effects/weather.h
        src/effects/python.h
        src/effects/python.c
        src/layout/csv_to_layout.c
        src/opc/opc_client.c
        src/opc/opc_client.h
        src/parson/parson.c
        src/parson/parson.h
        src/utils/csv.c
        src/utils/csv.h
        src/utils/list.c
        src/utils/list.h
        src/ctrlserver.h
        src/ctrlserver.c
        src/apimanager.c
        src/apimanager.h
        src/bare_bones_test.c
        src/effect_runner.c
        src/effect_runner.h
        src/extension.h
        src/pixel.c
        src/pixel.h
        src/projection.c
        src/projection.h
        src/Makefile)
