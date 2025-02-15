#
# Copyright (C) 2003-2019 Sébastien Helleu <flashcode@flashtux.org>
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

if(ENABLE_MAN)
  # man page: whoreirc
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/whoreirc.1
    COMMAND ${ASCIIDOCTOR_EXECUTABLE} ARGS ${ASCIIDOCTOR_ARGS} -b manpage -o whoreirc.1 ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc.1.ja.adoc
    DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc.1.ja.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/cmdline_options.ja.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/cmdline_options_debug.ja.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/man_plugin_options.ja.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/man_files.ja.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/man_copyright.ja.adoc
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building whoreirc.1 (ja)"
  )
  add_custom_target(doc-man-whoreirc-ja ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/whoreirc.1)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/whoreirc.1 DESTINATION ${MANDIR}/ja/man1)

  # man page: whoreirc-headless
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/whoreirc-headless.1
    COMMAND ${ASCIIDOCTOR_EXECUTABLE} ARGS ${ASCIIDOCTOR_ARGS} -b manpage -o whoreirc-headless.1 ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc-headless.1.ja.adoc
    DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc-headless.1.ja.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/cmdline_options.ja.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/cmdline_options_debug.ja.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/man_plugin_options.ja.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/man_files.ja.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/man_copyright.ja.adoc
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building whoreirc-headless.1 (ja)"
  )
  add_custom_target(doc-man-whoreirc-headless-ja ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/whoreirc-headless.1)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/whoreirc-headless.1 DESTINATION ${MANDIR}/ja/man1)
endif()

if(ENABLE_DOC)

  file(GLOB AUTOGEN_USER "${CMAKE_CURRENT_SOURCE_DIR}/autogen/user/*.adoc")
  file(GLOB AUTOGEN_PLUGIN "${CMAKE_CURRENT_SOURCE_DIR}/autogen/plugin_api/*.adoc")

  # user's guide
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_user.ja.html
    COMMAND ${ASCIIDOCTOR_EXECUTABLE} ARGS ${ASCIIDOCTOR_ARGS} -o whoreirc_user.ja.html ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc_user.ja.adoc
    DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/../docinfo.html
    ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc_user.ja.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/cmdline_options.ja.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/cmdline_options_debug.ja.adoc
    ${AUTOGEN_USER}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building whoreirc_user.ja.html"
  )
  add_custom_target(doc-user-ja ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_user.ja.html)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_user.ja.html DESTINATION ${SHAREDIR}/doc/${PROJECT_NAME})

  # plugin API reference
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_plugin_api.ja.html
    COMMAND ${ASCIIDOCTOR_EXECUTABLE} ARGS ${ASCIIDOCTOR_ARGS} -o whoreirc_plugin_api.ja.html ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc_plugin_api.ja.adoc
    DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/../docinfo.html
    ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc_plugin_api.ja.adoc
    ${AUTOGEN_PLUGIN}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building whoreirc_plugin_api.ja.html"
  )
  add_custom_target(doc-plugin-api-ja ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_plugin_api.ja.html)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_plugin_api.ja.html DESTINATION ${SHAREDIR}/doc/${PROJECT_NAME})

  # scripting guide
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_scripting.ja.html
    COMMAND ${ASCIIDOCTOR_EXECUTABLE} ARGS ${ASCIIDOCTOR_ARGS} -o whoreirc_scripting.ja.html ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc_scripting.ja.adoc
    DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/../docinfo.html
    ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc_scripting.ja.adoc
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building whoreirc_scripting.ja.html"
  )
  add_custom_target(doc-scripting-ja ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_scripting.ja.html)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_scripting.ja.html DESTINATION ${SHAREDIR}/doc/${PROJECT_NAME})

  # FAQ
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_faq.ja.html
    COMMAND ${ASCIIDOCTOR_EXECUTABLE} ARGS ${ASCIIDOCTOR_ARGS} -o whoreirc_faq.ja.html ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc_faq.ja.adoc
    DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/../docinfo.html
    ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc_faq.ja.adoc
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building whoreirc_faq.ja.html"
  )
  add_custom_target(doc-faq-ja ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_faq.ja.html)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_faq.ja.html DESTINATION ${SHAREDIR}/doc/${PROJECT_NAME})

  # quickstart
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_quickstart.ja.html
    COMMAND ${ASCIIDOCTOR_EXECUTABLE} ARGS ${ASCIIDOCTOR_ARGS} -o whoreirc_quickstart.ja.html ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc_quickstart.ja.adoc
    DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/../docinfo.html
    ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc_quickstart.ja.adoc
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building whoreirc_quickstart.ja.html"
  )
  add_custom_target(doc-quickstart-ja ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_quickstart.ja.html)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_quickstart.ja.html DESTINATION ${SHAREDIR}/doc/${PROJECT_NAME})

  # tester's guide
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_tester.ja.html
    COMMAND ${ASCIIDOCTOR_EXECUTABLE} ARGS ${ASCIIDOCTOR_ARGS} -o whoreirc_tester.ja.html ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc_tester.ja.adoc
    DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/../docinfo.html
    ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc_tester.ja.adoc
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building whoreirc_tester.ja.html"
  )
  add_custom_target(doc-tester-ja ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_tester.ja.html)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_tester.ja.html DESTINATION ${SHAREDIR}/doc/${PROJECT_NAME})

  # relay protocol
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_relay_protocol.ja.html
    COMMAND ${ASCIIDOCTOR_EXECUTABLE} ARGS ${ASCIIDOCTOR_ARGS} -o whoreirc_relay_protocol.ja.html ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc_relay_protocol.ja.adoc
    DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/../docinfo.html
    ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc_relay_protocol.ja.adoc
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building whoreirc_relay_protocol.ja.html"
  )
  add_custom_target(doc-relay-protocol-ja ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_relay_protocol.ja.html)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_relay_protocol.ja.html DESTINATION ${SHAREDIR}/doc/${PROJECT_NAME})

  # developer's guide
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_dev.ja.html
    COMMAND ${ASCIIDOCTOR_EXECUTABLE} ARGS ${ASCIIDOCTOR_ARGS} -o whoreirc_dev.ja.html ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc_dev.ja.adoc
    DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/../docinfo.html
    ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc_dev.ja.adoc
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building whoreirc_dev.ja.html"
  )
  add_custom_target(doc-dev-ja ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_dev.ja.html)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_dev.ja.html DESTINATION ${SHAREDIR}/doc/${PROJECT_NAME})

endif()
