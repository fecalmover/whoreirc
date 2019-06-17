#
# Copyright (C) 2003-2019 Sébastien Helleu <flashcode@flashtux.org>
#
# This file is part of WeeChat, the extensible chat client.
#
# WeeChat is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# WeeChat is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with WeeChat.  If not, see <https://www.gnu.org/licenses/>.
#

if(ENABLE_MAN)
  # man page: whoreirc
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/whoreirc.1
    COMMAND ${ASCIIDOCTOR_EXECUTABLE} ARGS ${ASCIIDOCTOR_ARGS} -b manpage -o whoreirc.1 ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc.1.it.adoc
    DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc.1.it.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/cmdline_options.it.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/cmdline_options_debug.it.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/man_plugin_options.it.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/man_files.it.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/man_copyright.it.adoc
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building whoreirc.1 (it)"
  )
  add_custom_target(doc-man-whoreirc-it ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/whoreirc.1)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/whoreirc.1 DESTINATION ${MANDIR}/it/man1)

  # man page: whoreirc-headless
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/whoreirc-headless.1
    COMMAND ${ASCIIDOCTOR_EXECUTABLE} ARGS ${ASCIIDOCTOR_ARGS} -b manpage -o whoreirc-headless.1 ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc-headless.1.it.adoc
    DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc-headless.1.it.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/cmdline_options.it.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/cmdline_options_debug.it.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/man_plugin_options.it.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/man_files.it.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/man_copyright.it.adoc
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building whoreirc-headless.1 (it)"
  )
  add_custom_target(doc-man-whoreirc-headless-it ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/whoreirc-headless.1)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/whoreirc-headless.1 DESTINATION ${MANDIR}/it/man1)
endif()

if(ENABLE_DOC)

  file(GLOB AUTOGEN_USER "${CMAKE_CURRENT_SOURCE_DIR}/autogen/user/*.adoc")
  file(GLOB AUTOGEN_PLUGIN "${CMAKE_CURRENT_SOURCE_DIR}/autogen/plugin_api/*.adoc")

  # user's guide
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_user.it.html
    COMMAND ${ASCIIDOCTOR_EXECUTABLE} ARGS ${ASCIIDOCTOR_ARGS} -o whoreirc_user.it.html ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc_user.it.adoc
    DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/../docinfo.html
    ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc_user.it.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/cmdline_options.it.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/cmdline_options_debug.it.adoc
    ${AUTOGEN_USER}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building whoreirc_user.it.html"
  )
  add_custom_target(doc-user-it ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_user.it.html)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_user.it.html DESTINATION ${SHAREDIR}/doc/${PROJECT_NAME})

  # plugin API reference
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_plugin_api.it.html
    COMMAND ${ASCIIDOCTOR_EXECUTABLE} ARGS ${ASCIIDOCTOR_ARGS} -o whoreirc_plugin_api.it.html ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc_plugin_api.it.adoc
    DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/../docinfo.html
    ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc_plugin_api.it.adoc
    ${AUTOGEN_PLUGIN}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building whoreirc_plugin_api.it.html"
  )
  add_custom_target(doc-plugin-api-it ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_plugin_api.it.html)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_plugin_api.it.html DESTINATION ${SHAREDIR}/doc/${PROJECT_NAME})

  # scripting guide
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_scripting.it.html
    COMMAND ${ASCIIDOCTOR_EXECUTABLE} ARGS ${ASCIIDOCTOR_ARGS} -o whoreirc_scripting.it.html ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc_scripting.it.adoc
    DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/../docinfo.html
    ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc_scripting.it.adoc
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building whoreirc_scripting.it.html"
  )
  add_custom_target(doc-scripting-it ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_scripting.it.html)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_scripting.it.html DESTINATION ${SHAREDIR}/doc/${PROJECT_NAME})

  # FAQ
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_faq.it.html
    COMMAND ${ASCIIDOCTOR_EXECUTABLE} ARGS ${ASCIIDOCTOR_ARGS} -o whoreirc_faq.it.html ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc_faq.it.adoc
    DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/../docinfo.html
    ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc_faq.it.adoc
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building whoreirc_faq.it.html"
  )
  add_custom_target(doc-faq-it ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_faq.it.html)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_faq.it.html DESTINATION ${SHAREDIR}/doc/${PROJECT_NAME})

  # quickstart
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_quickstart.it.html
    COMMAND ${ASCIIDOCTOR_EXECUTABLE} ARGS ${ASCIIDOCTOR_ARGS} -o whoreirc_quickstart.it.html ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc_quickstart.it.adoc
    DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/../docinfo.html
    ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc_quickstart.it.adoc
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building whoreirc_quickstart.it.html"
  )
  add_custom_target(doc-quickstart-it ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_quickstart.it.html)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_quickstart.it.html DESTINATION ${SHAREDIR}/doc/${PROJECT_NAME})

  # tester's guide
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_tester.it.html
    COMMAND ${ASCIIDOCTOR_EXECUTABLE} ARGS ${ASCIIDOCTOR_ARGS} -o whoreirc_tester.it.html ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc_tester.it.adoc
    DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/../docinfo.html
    ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc_tester.it.adoc
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building whoreirc_tester.it.html"
  )
  add_custom_target(doc-tester-it ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_tester.it.html)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_tester.it.html DESTINATION ${SHAREDIR}/doc/${PROJECT_NAME})

endif()
