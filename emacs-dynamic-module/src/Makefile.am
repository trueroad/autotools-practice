noinst_LTLIBRARIES = hello-module.la
hello_module_la_SOURCES = hello-module.c

pkglisp_DATA = $(objdir)/hello-module$(SHREXT)

$(objdir)/hello-module$(SHREXT): hello-module.la

hello_module_la_LDFLAGS = -no-undefined
hello_module_la_LDFLAGS += -module
hello_module_la_LDFLAGS += -avoid-version
hello_module_la_LDFLAGS += -rpath $(pkglispdir)
