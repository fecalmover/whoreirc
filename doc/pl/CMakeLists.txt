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
    COMMAND ${ASCIIDOCTOR_EXECUTABLE} ARGS ${ASCIIDOCTOR_ARGS} -b manpage -o whoreirc.1 ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc.1.pl.adoc
    DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc.1.pl.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/cmdline_options.pl.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/cmdline_options_debug.pl.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/man_plugin_options.pl.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/man_files.pl.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/man_copyright.pl.adoc
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building whoreirc.1 (pl)"
  )
  add_custom_target(doc-man-whoreirc-pl ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/whoreirc.1)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/whoreirc.1 DESTINATION ${MANDIR}/pl/man1)

  # man page: whoreirc-headless
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/whoreirc-headless.1
    COMMAND ${ASCIIDOCTOR_EXECUTABLE} ARGS ${ASCIIDOCTOR_ARGS} -b manpage -o whoreirc-headless.1 ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc-headless.1.pl.adoc
    DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc-headless.1.pl.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/cmdline_options.pl.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/cmdline_options_debug.pl.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/man_plugin_options.pl.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/man_files.pl.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/man_copyright.pl.adoc
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building whoreirc-headless.1 (pl)"
  )
  add_custom_target(doc-man-whoreirc-headless-pl ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/whoreirc-headless.1)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/whoreirc-headless.1 DESTINATION ${MANDIR}/pl/man1)
endif()

if(ENABLE_DOC)

  file(GLOB AUTOGEN_USER "${CMAKE_CURRENT_SOURCE_DIR}/autogen/user/*.adoc")

  # user's guide
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_user.pl.html
    COMMAND ${ASCIIDOCTOR_EXECUTABLE} ARGS ${ASCIIDOCTOR_ARGS} -o whoreirc_user.pl.html ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc_user.pl.adoc
    DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/../docinfo.html
    ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc_user.pl.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/cmdline_options.pl.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/cmdline_options_debug.pl.adoc
    ${AUTOGEN_USER}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building whoreirc_user.pl.html"
  )
  add_custom_target(doc-user-pl ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_user.pl.html)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_user.pl.html DESTINATION ${SHAREDIR}/doc/${PROJECT_NAME})

  # scripting guide
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_scripting.pl.html
    COMMAND ${ASCIIDOCTOR_EXECUTABLE} ARGS ${ASCIIDOCTOR_ARGS} -o whoreirc_scripting.pl.html ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc_scripting.pl.adoc
    DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/../docinfo.html
    ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc_scripting.pl.adoc
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building whoreirc_scripting.pl.html"
  )
  add_custom_target(doc-scripting-pl ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_scripting.pl.html)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_scripting.pl.html DESTINATION ${SHAREDIR}/doc/${PROJECT_NAME})

  # FAQ
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_faq.pl.html
    COMMAND ${ASCIIDOCTOR_EXECUTABLE} ARGS ${ASCIIDOCTOR_ARGS} -o whoreirc_faq.pl.html ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc_faq.pl.adoc
    DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/../docinfo.html
    ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc_faq.pl.adoc
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building whoreirc_faq.pl.html"
  )
  add_custom_target(doc-faq-pl ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_faq.pl.html)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_faq.pl.html DESTINATION ${SHAREDIR}/doc/${PROJECT_NAME})

  # quickstart
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_quickstart.pl.html
    COMMAND ${ASCIIDOCTOR_EXECUTABLE} ARGS ${ASCIIDOCTOR_ARGS} -o whoreirc_quickstart.pl.html ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc_quickstart.pl.adoc
    DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/../docinfo.html
    ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc_quickstart.pl.adoc
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building whoreirc_quickstart.pl.html"
  )
  add_custom_target(doc-quickstart-pl ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_quickstart.pl.html)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_quickstart.pl.html DESTINATION ${SHAREDIR}/doc/${PROJECT_NAME})

  # tester's guide
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_tester.pl.html
    COMMAND ${ASCIIDOCTOR_EXECUTABLE} ARGS ${ASCIIDOCTOR_ARGS} -o whoreirc_tester.pl.html ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc_tester.pl.adoc
    DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/../docinfo.html
    ${CMAKE_CURRENT_SOURCE_DIR}/whoreirc_tester.pl.adoc
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building whoreirc_tester.pl.html"
  )
  add_custom_target(doc-tester-pl ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_tester.pl.html)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/whoreirc_tester.pl.html DESTINATION ${SHAREDIR}/doc/${PROJECT_NAME})

endif()
