# ###########################################################################
# ADOBE CONFIDENTIAL
# ___________________
# Copyright 2022 Adobe
# All Rights Reserved.
# * NOTICE:  All information contained herein is, and remains
# the property of Adobe and its suppliers, if any. The intellectual
# and technical concepts contained herein are proprietary to Adobe
# and its suppliers and are protected by all applicable intellectual
# property laws, including trade secret and copyright laws.
# Dissemination of this information or reproduction of this material
# is strictly forbidden unless prior written permission is obtained
# from Adobe.
# ###########################################################################

#
# Copyright 2021 Adobe. All rights reserved.
# This file is licensed to you under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License. You may obtain a copy
# of the License at http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software distributed under
# the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS
# OF ANY KIND, either express or implied. See the License for the specific language
# governing permissions and limitations under the License.
#
if(TARGET miniz::miniz)
    return()
endif()

message(STATUS "Third-party (external): creating target 'miniz::miniz'")

include(FetchContent)
FetchContent_Declare(
    miniz
    URL https://github.com/richgel999/miniz/releases/download/2.2.0/miniz-2.2.0.zip
    URL_MD5 bc866f2def5214188cd6481e2694bd3c
)
FetchContent_MakeAvailable(miniz)

add_library(miniz ${miniz_SOURCE_DIR}/miniz.c)
add_library(miniz::miniz ALIAS miniz)

include(GNUInstallDirs)
target_include_directories(miniz PUBLIC
    $<BUILD_INTERFACE:${miniz_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set_target_properties(miniz PROPERTIES FOLDER third_party)

target_include_directories(miniz PUBLIC
    $<BUILD_INTERFACE:${miniz_BINARY_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Install rules
set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME miniz)
install(FILES ${miniz_SOURCE_DIR}/miniz.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(TARGETS miniz EXPORT Miniz_Targets)
install(EXPORT Miniz_Targets DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/miniz NAMESPACE miniz::)
