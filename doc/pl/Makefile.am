#
# Copyright (C) 2003-2019 Sébastien Helleu <flashcode@flashtux.org>
# Copyright (C) 2006 Julien Louis <ptitlouis@sysif.net>
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

docdir = $(datadir)/doc/$(PACKAGE)

EXTRA_DIST = CMakeLists.txt \
             docinfo.html \
             whoreirc.1.pl.adoc \
             whoreirc-headless.1.pl.adoc \
             cmdline_options.pl.adoc \
             cmdline_options_debug.pl.adoc \
             whoreirc_user.pl.adoc \
             whoreirc_scripting.pl.adoc \
             whoreirc_faq.pl.adoc \
             whoreirc_quickstart.pl.adoc \
             whoreirc_tester.pl.adoc \
             $(wildcard autogen/user/*.adoc)

if MAN
  man_targets = whoreirc.1 \
                whoreirc-headless.1
  man_install = install-man
  man_uninstall = uninstall-man
endif
if DOC
  doc_targets = whoreirc_user.pl.html \
		whoreirc_scripting.pl.html \
		whoreirc_faq.pl.html \
		whoreirc_quickstart.pl.html \
		whoreirc_tester.pl.html
  doc_install = install-doc
  doc_uninstall = uninstall-doc
endif
all-local: $(man_targets) $(doc_targets)

# man pages
whoreirc.1: whoreirc.1.pl.adoc cmdline_options.pl.adoc cmdline_options_debug.pl.adoc
	$(ASCIIDOCTOR) -a revision="WeeChat $(VERSION)" -b manpage -o whoreirc.1 $(abs_top_srcdir)/doc/pl/whoreirc.1.pl.adoc
whoreirc-headless.1: whoreirc-headless.1.pl.adoc cmdline_options.pl.adoc cmdline_options_debug.pl.adoc
	$(ASCIIDOCTOR) -a revision="WeeChat $(VERSION)" -b manpage -o whoreirc-headless.1 $(abs_top_srcdir)/doc/pl/whoreirc-headless.1.pl.adoc

# user's guide
whoreirc_user.pl.html: whoreirc_user.pl.adoc cmdline_options.pl.adoc cmdline_options_debug.pl.adoc $(wildcard autogen/user/*.adoc) $(abs_top_srcdir)/doc/docinfo.html
	$(ASCIIDOCTOR) -a revision="$(VERSION)" -a sectanchors -a source-highlighter=prettify -o whoreirc_user.pl.html $(abs_top_srcdir)/doc/pl/whoreirc_user.pl.adoc

# scripting guide
whoreirc_scripting.pl.html: whoreirc_scripting.pl.adoc $(abs_top_srcdir)/doc/docinfo.html
	$(ASCIIDOCTOR) -a revision="$(VERSION)" -a sectanchors -a source-highlighter=prettify -o whoreirc_scripting.pl.html $(abs_top_srcdir)/doc/pl/whoreirc_scripting.pl.adoc

# FAQ
whoreirc_faq.pl.html: whoreirc_faq.pl.adoc $(abs_top_srcdir)/doc/docinfo.html
	$(ASCIIDOCTOR) -a revision="$(VERSION)" -a sectanchors -a source-highlighter=prettify -o whoreirc_faq.pl.html $(abs_top_srcdir)/doc/pl/whoreirc_faq.pl.adoc

# quickstart
whoreirc_quickstart.pl.html: whoreirc_quickstart.pl.adoc $(abs_top_srcdir)/doc/docinfo.html
	$(ASCIIDOCTOR) -a revision="$(VERSION)" -a sectanchors -a source-highlighter=prettify -o whoreirc_quickstart.pl.html $(abs_top_srcdir)/doc/pl/whoreirc_quickstart.pl.adoc

# tester's guide
whoreirc_tester.pl.html: whoreirc_tester.pl.adoc $(abs_top_srcdir)/doc/docinfo.html
	$(ASCIIDOCTOR) -a revision="$(VERSION)" -a sectanchors -a source-highlighter=prettify -o whoreirc_tester.pl.html $(abs_top_srcdir)/doc/pl/whoreirc_tester.pl.adoc

# install man/docs

install-data-hook: $(man_install) $(doc_install)

install-man:
	$(mkinstalldirs) $(DESTDIR)$(mandir)/pl/man1/
	$(INSTALL_DATA) *.1 $(DESTDIR)$(mandir)/pl/man1/

install-doc:
	$(mkinstalldirs) $(DESTDIR)$(docdir)/
	$(INSTALL_DATA) *.html $(DESTDIR)$(docdir)/

# uninstall man/docs

uninstall-hook: $(man_uninstall) $(doc_uninstall)

uninstall-man:
	$(RM) $(DESTDIR)$(mandir)/pl/man1/whoreirc.1
	$(RM) $(DESTDIR)$(mandir)/pl/man1/whoreirc-headless.1
	-rmdir $(DESTDIR)$(mandir)/pl/man1

uninstall-doc:
	$(RM) $(DESTDIR)$(docdir)/*.pl.html
	-rmdir $(DESTDIR)$(docdir)

# clean

clean-local:
	-$(RM) whoreirc.1 whoreirc-headless.1 whoreirc_*.html
