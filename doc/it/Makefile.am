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

docdir = $(datadir)/doc/$(PACKAGE)

EXTRA_DIST = CMakeLists.txt \
             docinfo.html \
             whoreirc.1.it.adoc \
             whoreirc-headless.1.it.adoc \
             cmdline_options.it.adoc \
             cmdline_options_debug.it.adoc \
             whoreirc_user.it.adoc \
             whoreirc_plugin_api.it.adoc \
             whoreirc_scripting.it.adoc \
             whoreirc_faq.it.adoc \
             whoreirc_quickstart.it.adoc \
             whoreirc_tester.it.adoc \
             $(wildcard autogen/user/*.adoc) \
             $(wildcard autogen/plugin_api/*.adoc)

if MAN
  man_targets = whoreirc.1 \
                whoreirc-headless.1
  man_install = install-man
  man_uninstall = uninstall-man
endif
if DOC
  doc_targets = whoreirc_user.it.html \
		whoreirc_plugin_api.it.html \
		whoreirc_scripting.it.html \
		whoreirc_faq.it.html \
		whoreirc_quickstart.it.html \
		whoreirc_tester.it.html
  doc_install = install-doc
  doc_uninstall = uninstall-doc
endif
all-local: $(man_targets) $(doc_targets)

# man pages
whoreirc.1: whoreirc.1.it.adoc cmdline_options.it.adoc cmdline_options_debug.it.adoc
	$(ASCIIDOCTOR) -a revision="WhoreIRC $(VERSION)" -b manpage -o whoreirc.1 $(abs_top_srcdir)/doc/it/whoreirc.1.it.adoc
whoreirc-headless.1: whoreirc-headless.1.it.adoc cmdline_options.it.adoc cmdline_options_debug.it.adoc
	$(ASCIIDOCTOR) -a revision="WhoreIRC $(VERSION)" -b manpage -o whoreirc-headless.1 $(abs_top_srcdir)/doc/it/whoreirc-headless.1.it.adoc

# user's guide
whoreirc_user.it.html: whoreirc_user.it.adoc cmdline_options.it.adoc cmdline_options_debug.it.adoc $(wildcard autogen/user/*.adoc) $(abs_top_srcdir)/doc/docinfo.html
	$(ASCIIDOCTOR) -a revision="$(VERSION)" -a sectanchors -a source-highlighter=prettify -o whoreirc_user.it.html $(abs_top_srcdir)/doc/it/whoreirc_user.it.adoc

# plugin API reference
whoreirc_plugin_api.it.html: whoreirc_plugin_api.it.adoc $(wildcard autogen/plugin_api/*.adoc) $(abs_top_srcdir)/doc/docinfo.html
	$(ASCIIDOCTOR) -a revision="$(VERSION)" -a sectanchors -a source-highlighter=prettify -o whoreirc_plugin_api.it.html $(abs_top_srcdir)/doc/it/whoreirc_plugin_api.it.adoc

# scripting guide
whoreirc_scripting.it.html: whoreirc_scripting.it.adoc $(abs_top_srcdir)/doc/docinfo.html
	$(ASCIIDOCTOR) -a revision="$(VERSION)" -a sectanchors -a source-highlighter=prettify -o whoreirc_scripting.it.html $(abs_top_srcdir)/doc/it/whoreirc_scripting.it.adoc

# FAQ
whoreirc_faq.it.html: whoreirc_faq.it.adoc $(abs_top_srcdir)/doc/docinfo.html
	$(ASCIIDOCTOR) -a revision="$(VERSION)" -a sectanchors -a source-highlighter=prettify -o whoreirc_faq.it.html $(abs_top_srcdir)/doc/it/whoreirc_faq.it.adoc

# quickstart
whoreirc_quickstart.it.html: whoreirc_quickstart.it.adoc $(abs_top_srcdir)/doc/docinfo.html
	$(ASCIIDOCTOR) -a revision="$(VERSION)" -a sectanchors -a source-highlighter=prettify -o whoreirc_quickstart.it.html $(abs_top_srcdir)/doc/it/whoreirc_quickstart.it.adoc

# tester's guide
whoreirc_tester.it.html: whoreirc_tester.it.adoc $(abs_top_srcdir)/doc/docinfo.html
	$(ASCIIDOCTOR) -a revision="$(VERSION)" -a sectanchors -a source-highlighter=prettify -o whoreirc_tester.it.html $(abs_top_srcdir)/doc/it/whoreirc_tester.it.adoc

# install man/docs

install-data-hook: $(man_install) $(doc_install)

install-man:
	$(mkinstalldirs) $(DESTDIR)$(mandir)/it/man1/
	$(INSTALL_DATA) *.1 $(DESTDIR)$(mandir)/it/man1/

install-doc:
	$(mkinstalldirs) $(DESTDIR)$(docdir)/
	$(INSTALL_DATA) *.html $(DESTDIR)$(docdir)/

# uninstall man/docs

uninstall-hook: $(man_uninstall) $(doc_uninstall)

uninstall-man:
	$(RM) $(DESTDIR)$(mandir)/it/man1/whoreirc.1
	$(RM) $(DESTDIR)$(mandir)/it/man1/whoreirc-headless.1
	-rmdir $(DESTDIR)$(mandir)/it/man1

uninstall-doc:
	$(RM) $(DESTDIR)$(docdir)/*.it.html
	-rmdir $(DESTDIR)$(docdir)

# clean

clean-local:
	-$(RM) whoreirc.1 whoreirc-headless.1 whoreirc_*.html
