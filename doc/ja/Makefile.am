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
             whoreirc.1.ja.adoc \
             whoreirc-headless.1.ja.adoc \
             cmdline_options.ja.adoc \
             cmdline_options_debug.ja.adoc \
             whoreirc_user.ja.adoc \
             whoreirc_plugin_api.ja.adoc \
             whoreirc_scripting.ja.adoc \
             whoreirc_faq.ja.adoc \
             whoreirc_quickstart.ja.adoc \
             whoreirc_tester.ja.adoc \
             whoreirc_relay_protocol.ja.adoc \
             whoreirc_dev.ja.adoc \
             $(wildcard autogen/user/*.adoc) \
             $(wildcard autogen/plugin_api/*.adoc)

if MAN
  man_targets = whoreirc.1 \
                whoreirc-headless.1
  man_install = install-man
  man_uninstall = uninstall-man
endif
if DOC
  doc_targets = whoreirc_user.ja.html \
		whoreirc_plugin_api.ja.html \
		whoreirc_scripting.ja.html \
		whoreirc_faq.ja.html \
		whoreirc_quickstart.ja.html \
		whoreirc_tester.ja.html \
		whoreirc_relay_protocol.ja.html \
		whoreirc_dev.ja.html
  doc_install = install-doc
  doc_uninstall = uninstall-doc
endif
all-local: $(man_targets) $(doc_targets)

# man pages
whoreirc.1: whoreirc.1.ja.adoc cmdline_options.ja.adoc cmdline_options_debug.ja.adoc
	$(ASCIIDOCTOR) -a revision="WhoreIRC $(VERSION)" -b manpage -o whoreirc.1 $(abs_top_srcdir)/doc/ja/whoreirc.1.ja.adoc
whoreirc-headless.1: whoreirc-headless.1.ja.adoc cmdline_options.ja.adoc cmdline_options_debug.ja.adoc
	$(ASCIIDOCTOR) -a revision="WhoreIRC $(VERSION)" -b manpage -o whoreirc-headless.1 $(abs_top_srcdir)/doc/ja/whoreirc-headless.1.ja.adoc

# user's guide
whoreirc_user.ja.html: whoreirc_user.ja.adoc cmdline_options.ja.adoc cmdline_options_debug.ja.adoc $(wildcard autogen/user/*.adoc) $(abs_top_srcdir)/doc/docinfo.html
	$(ASCIIDOCTOR) -a revision="$(VERSION)" -a sectanchors -a source-highlighter=prettify -o whoreirc_user.ja.html $(abs_top_srcdir)/doc/ja/whoreirc_user.ja.adoc

# plugin API reference
whoreirc_plugin_api.ja.html: whoreirc_plugin_api.ja.adoc $(wildcard autogen/plugin_api/*.adoc) $(abs_top_srcdir)/doc/docinfo.html
	$(ASCIIDOCTOR) -a revision="$(VERSION)" -a sectanchors -a source-highlighter=prettify -o whoreirc_plugin_api.ja.html $(abs_top_srcdir)/doc/ja/whoreirc_plugin_api.ja.adoc

# scripting guide
whoreirc_scripting.ja.html: whoreirc_scripting.ja.adoc $(abs_top_srcdir)/doc/docinfo.html
	$(ASCIIDOCTOR) -a revision="$(VERSION)" -a sectanchors -a source-highlighter=prettify -o whoreirc_scripting.ja.html $(abs_top_srcdir)/doc/ja/whoreirc_scripting.ja.adoc

# FAQ
whoreirc_faq.ja.html: whoreirc_faq.ja.adoc $(abs_top_srcdir)/doc/docinfo.html
	$(ASCIIDOCTOR) -a revision="$(VERSION)" -a sectanchors -a source-highlighter=prettify -o whoreirc_faq.ja.html $(abs_top_srcdir)/doc/ja/whoreirc_faq.ja.adoc

# quickstart
whoreirc_quickstart.ja.html: whoreirc_quickstart.ja.adoc $(abs_top_srcdir)/doc/docinfo.html
	$(ASCIIDOCTOR) -a revision="$(VERSION)" -a sectanchors -a source-highlighter=prettify -o whoreirc_quickstart.ja.html $(abs_top_srcdir)/doc/ja/whoreirc_quickstart.ja.adoc

# tester's guide
whoreirc_tester.ja.html: whoreirc_tester.ja.adoc $(abs_top_srcdir)/doc/docinfo.html
	$(ASCIIDOCTOR) -a revision="$(VERSION)" -a sectanchors -a source-highlighter=prettify -o whoreirc_tester.ja.html $(abs_top_srcdir)/doc/ja/whoreirc_tester.ja.adoc

# relay protocol
whoreirc_relay_protocol.ja.html: whoreirc_relay_protocol.ja.adoc $(abs_top_srcdir)/doc/docinfo.html
	$(ASCIIDOCTOR) -a revision="$(VERSION)" -a sectanchors -a source-highlighter=prettify -o whoreirc_relay_protocol.ja.html $(abs_top_srcdir)/doc/ja/whoreirc_relay_protocol.ja.adoc

# developer's guide
whoreirc_dev.ja.html: whoreirc_dev.ja.adoc $(abs_top_srcdir)/doc/docinfo.html
	$(ASCIIDOCTOR) -a revision="$(VERSION)" -a sectanchors -a source-highlighter=prettify -o whoreirc_dev.ja.html $(abs_top_srcdir)/doc/ja/whoreirc_dev.ja.adoc

# install man/docs

install-data-hook: $(man_install) $(doc_install)

install-man:
	$(mkinstalldirs) $(DESTDIR)$(mandir)/ja/man1/
	$(INSTALL_DATA) *.1 $(DESTDIR)$(mandir)/ja/man1/

install-doc:
	$(mkinstalldirs) $(DESTDIR)$(docdir)/
	$(INSTALL_DATA) *.html $(DESTDIR)$(docdir)/

# uninstall man/docs

uninstall-hook: $(man_uninstall) $(doc_uninstall)

uninstall-man:
	$(RM) $(DESTDIR)$(mandir)/ja/man1/whoreirc.1
	$(RM) $(DESTDIR)$(mandir)/ja/man1/whoreirc-headless.1
	-rmdir $(DESTDIR)$(mandir)/ja/man1

uninstall-doc:
	$(RM) $(DESTDIR)$(docdir)/*.ja.html
	-rmdir $(DESTDIR)$(docdir)

# clean

clean-local:
	-$(RM) whoreirc.1 whoreirc-headless.1 whoreirc_*.html
