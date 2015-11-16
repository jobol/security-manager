# Copyright (c) 2011 - 2014 Samsung Electronics Co., Ltd All Rights Reserved
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#
# @file        CMakeLists.txt
# @author
# @brief
#

############################# Check minimum CMake version #####################

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT("security-manager")

############################# cmake packages ##################################

INCLUDE(FindPkgConfig)

############################# compiler flags ##################################

SET(CMAKE_C_FLAGS_PROFILING    "-g -O0 -pg -Wp,-U_FORTIFY_SOURCE")
SET(CMAKE_CXX_FLAGS_PROFILING  "-g -std=c++0x -O0 -pg -Wp,-U_FORTIFY_SOURCE")
SET(CMAKE_C_FLAGS_DEBUG        "-g -O0 -ggdb -Wp,-U_FORTIFY_SOURCE")
SET(CMAKE_CXX_FLAGS_DEBUG      "-g -std=c++0x -O0 -ggdb -Wp,-U_FORTIFY_SOURCE")
SET(CMAKE_C_FLAGS_RELEASE      "-g -O2")
SET(CMAKE_CXX_FLAGS_RELEASE    "-g -std=c++0x -O2")
SET(CMAKE_C_FLAGS_CCOV         "-g -O2 --coverage")
SET(CMAKE_CXX_FLAGS_CCOV       "-g -std=c++0x -O2 --coverage")

# If supported for the target machine, emit position-independent code,suitable
# for dynamic linking and avoiding any limit on the size of the global offset
# table. This option makes a difference on the m68k, PowerPC and SPARC.
# (BJ: our ARM too?)
ADD_DEFINITIONS("-fPIC")

# Set compiler warning flags
ADD_DEFINITIONS("-Werror")                      # Make all warnings into errors.
ADD_DEFINITIONS("-Wall")                        # Generate all warnings
ADD_DEFINITIONS("-Wextra")                      # Generate even more extra warnings

STRING(REGEX MATCH "([^.]*)" API_VERSION "${VERSION}")
ADD_DEFINITIONS("-DAPI_VERSION=\"${API_VERSION}\"")

ADD_DEFINITIONS("-DSMACK_ENABLED")

IF (CMAKE_BUILD_TYPE MATCHES "DEBUG")
    ADD_DEFINITIONS("-DTIZEN_DEBUG_ENABLE")
    ADD_DEFINITIONS("-DBUILD_TYPE_DEBUG")
ENDIF (CMAKE_BUILD_TYPE MATCHES "DEBUG")

SET(DATADIR        "/usr/share/security-manager" CACHE STRING "path to data directory")
SET(SMACKRULESDIR  "/etc/smack/accesses.d"       CACHE STRING "path to Smack rules directory")
SET(LOCKDIR        "/var/run/lock"               CACHE STRING "path to lock directory")
SET(DB_INSTALL_DIR "/var/db/security-manager"    CACHE STRING "path to database directory")
SET(DB_FILENAME    ".security-manager.db"        CACHE STRING "basename of database")
SET(GLOBALUSER     "userapp"                     CACHE STRING "name of the global user")

ADD_DEFINITIONS("-DDATADIR=\"${DATADIR}\"")
ADD_DEFINITIONS("-DSMACKRULESDIR=\"${SMACKRULESDIR}\"")
ADD_DEFINITIONS("-DLOCKDIR=\"${LOCKDIR}\"")
ADD_DEFINITIONS("-DDB_INSTALL_DIR=\"${DB_INSTALL_DIR}\"")
ADD_DEFINITIONS("-DDB_FILENAME=\"${DB_FILENAME}\"")
ADD_DEFINITIONS("-DGLOBALUSER=\"${GLOBALUSER}\"")

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(pc)
ADD_SUBDIRECTORY(systemd)
ADD_SUBDIRECTORY(db)
ADD_SUBDIRECTORY(policy)
