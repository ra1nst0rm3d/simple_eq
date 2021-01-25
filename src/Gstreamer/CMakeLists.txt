cmake_minimum_required(VERSION 3.10)
project(gst-plugin-simpleeq)

include_directories(.)
find_package(PkgConfig REQUIRED)

pkg_check_modules(GST REQUIRED
        gstreamer-1.0>=1.0
        gstreamer-base-1.0>=1.0
        gstreamer-controller-1.0>=1.0
        gstreamer-audio-1.0>=1.0)

pkg_check_modules(GLIB REQUIRED
        glib-2.0
        gio-unix-2.0)

file(GLOB SOURCES
        *.h
        *.c)

add_library(gst-plugin-simpleeq SHARED ${SOURCES})

target_include_directories(gst-plugin-simpleeq PUBLIC ${GST_INCLUDE_DIRS} ${GLIB_INCLUDE_DIRS})
target_compile_options(gst-plugin-simpleeq PUBLIC ${GST_CFLAGS} ${GLIB_CFLAGS})
target_link_libraries(gst-plugin-simpleeq ${GST_LIBRARIES} ${GLIB_LIBRARIES})

set_target_properties(gst-plugin-simpleeq PROPERTIES OUTPUT_NAME "gstsimpleeq")
