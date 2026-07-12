# common.mk — shared Make fragment for SSW's non-GUI module Makefiles.
#
# Included (via `include ../../common.mk`) by:
#   - the six non-GUI src module Makefiles: src/core, src/strategic,
#     src/ships, src/weapons, src/defenses, src/tactical
#   - the four simple test-lib Makefiles: tests/core, tests/weapons,
#     tests/strategic, tests/ships
#
# Each including Makefile must still define, after the `include` line:
#   - TARGET                      (the .a library name)
#   - SRCS / OBJS / DEPS          (from $(wildcard *.cpp), or the
#                                  filter-out test_template.cpp glob for
#                                  the test-lib Makefiles)
#   - the `all $(TARGET): $(COVERAGE_PREP) $(OBJS)` -> `ar rcs` rule
#   - `-include $(DEPS)` at the end of the file
#   - any module-specific CXXFLAGS/LIBS additions via `+=` AFTER the
#     include (e.g. `` `wx-config --cxxflags` `` and -lcppunit for the
#     test-lib Makefiles, or -D<MODULE>_TEST_REPO_ROOT defines)
#
# WX_CXXFLAGS below is a LAZY (recursively expanded, `=`) variable, not a
# `:=` (immediate) one. GNU make only invokes $(shell ...) inside a
# recursively-expanded variable when that variable is actually referenced
# by a rule/recipe that runs. Because the base CXXFLAGS in this file does
# NOT reference WX_CXXFLAGS, the five pure modules (core, strategic,
# ships, weapons, defenses) that include this file never expand
# WX_CXXFLAGS and therefore never shell out to wx-config or pkg-config.
# src/tactical is the sole approved exception: its own Makefile adds
# `CXXFLAGS += $(WX_CXXFLAGS)` after including this file, reproducing its
# previous hardcoded wx+gtk include set via wx-config/pkg-config instead
# of a machine-specific path. (See AGENTS.md module-boundary carve-out.)

ifeq ($(COVERAGE),1)
COVERAGE_FLAGS = -fprofile-arcs -ftest-coverage
COVERAGE_PREP = coverage-clean
else
COVERAGE_FLAGS =
COVERAGE_PREP =
endif

.DEFAULT_GOAL := all

CXXFLAGS = -I../../include -g -Wall -Woverloaded-virtual -DLINUX -std=c++17 $(COVERAGE_FLAGS)
DEPFLAGS = -MMD -MP

PICFLAGS = -fPIC -DPIC

# wx-config/pkg-config-derived flags — lazy on purpose, see header note.
# Only src/tactical's Makefile references this.
WX_CXXFLAGS = $(shell wx-config --cxxflags) $(shell pkg-config --cflags gtk+-3.0)

#################### the implicit rules #############################

# NB: we use the old style suffix rules (.c.o) and not the GNU make
#     implicit rules (%.o: %.c) because they are more portable, in
#     particular the BSD make understands the former but not the
#     latter
.SUFFIXES: .o .c .cpp .cxx .rsrc .r

.c.o:
	$(CC) -c $(CFLAGS) $(PICFLAGS) -o $@ $<

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(DEPFLAGS) $(PICFLAGS) -o $@ $<

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(PICFLAGS) -o $@ $<

.rsrc.r:
	$(DEREZ) $^ Carbon.r -useDF > $@

############################# Shared targets #########################

objs: $(OBJS)
	rm -f *.gcda
	rm -f *.gcov
	rm -f *.gcno

.PHONY: coverage-clean
coverage-clean:
	rm -f $(OBJS) $(DEPS)
	rm -f *.gcda
	rm -f *.gcov
	rm -f *.gcno

clean:
	rm -f $(OBJS) $(DEPS)
	rm -f $(TARGET)
