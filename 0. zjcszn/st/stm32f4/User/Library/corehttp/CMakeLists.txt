cmake_minimum_required(VERSION 3.21)

include(httpFilePaths.cmake)

add_library(corehttp STATIC ${HTTP_SOURCES})
