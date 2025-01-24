#
# Copyright (C) 2014-2019 Sébastien Helleu <flashcode@flashtux.org>
#
# This file is part of WhoreIRC, the extensible chat client.
#
# WhoreIRC is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# WhoreIRC is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with WhoreIRC.  If not, see <https://www.gnu.org/licenses/>.
#

enable_language(CXX)

remove_definitions(-DHAVE_CONFIG_H)
include_directories(${CPPUTEST_INCLUDE_DIRS} ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR})

# unit tests (core)
set(LIB_WHOREIRC_UNIT_TESTS_CORE_SRC
  unit/test-plugins.cpp
  unit/core/test-core-arraylist.cpp
  unit/core/test-core-eval.cpp
  unit/core/test-core-hashtable.cpp
  unit/core/test-core-hdata.cpp
  unit/core/test-core-hook.cpp
  unit/core/test-core-infolist.cpp
  unit/core/test-core-list.cpp
  unit/core/test-core-secure.cpp
  unit/core/test-core-string.cpp
  unit/core/test-core-url.cpp
  unit/core/test-core-utf8.cpp
  unit/core/test-core-util.cpp
  unit/gui/test-gui-line.cpp
  scripts/test-scripts.cpp
)
add_library(whoreirc_unit_tests_core STATIC ${LIB_WHOREIRC_UNIT_TESTS_CORE_SRC})

# unit tests (plugins)
set(LIB_WHOREIRC_UNIT_TESTS_PLUGINS_SRC
  unit/plugins/irc/test-irc-color.cpp
  unit/plugins/irc/test-irc-config.cpp
  unit/plugins/irc/test-irc-mode.cpp
  unit/plugins/irc/test-irc-protocol.cpp
)
add_library(whoreirc_unit_tests_plugins MODULE ${LIB_WHOREIRC_UNIT_TESTS_PLUGINS_SRC})

if(ICONV_LIBRARY)
  list(APPEND EXTRA_LIBS ${ICONV_LIBRARY})
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
  list(APPEND EXTRA_LIBS "intl")
  if(HAVE_BACKTRACE)
    list(APPEND EXTRA_LIBS "execinfo")
  endif()
endif()

# binary to run tests
set(WHOREIRC_TESTS_SRC tests.cpp tests.h)
add_executable(tests ${WHOREIRC_TESTS_SRC})
target_link_libraries(tests
  whoreirc_core
  whoreirc_plugins
  whoreirc_gui_common
  whoreirc_gui_headless
  whoreirc_ncurses_fake
  whoreirc_unit_tests_core
  # due to circular references, we must link two times with libwhoreirc_core.a
  whoreirc_core
  ${EXTRA_LIBS}
  ${CURL_LIBRARIES}
  ${CPPUTEST_LIBRARIES}
  -rdynamic)
add_dependencies(tests
  whoreirc_core
  whoreirc_plugins
  whoreirc_gui_common
  whoreirc_gui_headless
  whoreirc_ncurses_fake
  whoreirc_unit_tests_core)

# test for cmake (ctest)
add_test(NAME unit
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMAND tests -v)
set_property(TEST unit PROPERTY
  ENVIRONMENT "WHOREIRC_TESTS_ARGS=-p;"
  "WHOREIRC_EXTRA_LIBDIR=${PROJECT_BINARY_DIR}/src;"
  "WHOREIRC_TESTS_SCRIPTS_DIR=${CMAKE_CURRENT_SOURCE_DIR}/scripts/python"
  "WHOREIRC_TESTS_PLUGINS_LIB=${CMAKE_CURRENT_BINARY_DIR}/libwhoreirc_unit_tests_plugins.so")
