.RECIPEPREFIX = >
ifeq ($(COVERAGE),1)
COVERAGE_FLAGS = -fprofile-arcs -ftest-coverage
COVERAGE_PREP = coverage-clean
else
COVERAGE_FLAGS =
COVERAGE_PREP =
endif

.DEFAULT_GOAL := all

CXXFLAGS = -I../../include -g `wx-config --cxxflags` -Wall -Woverloaded-virtual -DLINUX $(COVERAGE_FLAGS) -DTACTICAL_TEST_REPO_ROOT=\"$(CURDIR)/../..\"
DEPFLAGS = -MMD -MP

LIBS = `wx-config --libs`
MODEL_LIBS = -L../../src/gui -L../../src/tactical -L../../src/weapons -L../../src/defenses -L../../src/ships -L../../src/strategic -L../../src/core -Wl,--start-group -lgui -ltactical -lweapons -ldefenses -lships -lstrategic -lcore -Wl,--end-group

PICFLAGS = -fPIC -DPIC

.SUFFIXES: .o .c .cpp .cxx .rsrc .r

.c.o:
>$(CC) -c $(CFLAGS) $(PICFLAGS) -o $@ $<

.cpp.o:
>$(CXX) -c $(CXXFLAGS) $(DEPFLAGS) $(PICFLAGS) -o $@ $<

.cxx.o:
>$(CXX) -c $(CXXFLAGS) $(PICFLAGS) -o $@ $<

.rsrc.r:
>$(DEREZ) $^ Carbon.r -useDF > $@

SRCS = $(filter-out test_template.cpp,$(wildcard *.cpp))
OBJS = $(SRCS:.cpp=.o)
DEPS = $(OBJS:.o=.d)
LIB_OBJS = $(filter-out TacticalTests.o,$(OBJS))

TARGET = libtacticalTests.a
RUNNER = TacticalTests

$(OBJS): Makefile

all: $(COVERAGE_PREP) model-deps $(TARGET) $(RUNNER)

model-deps:
ifeq ($(SKIP_MODEL_DEPS),1)
> @true
else
>$(MAKE) -C ../../src/core
>$(MAKE) -C ../../src/strategic
>$(MAKE) -C ../../src/ships
>$(MAKE) -C ../../src/defenses
>$(MAKE) -C ../../src/weapons
>$(MAKE) -C ../../src/tactical
>$(MAKE) -C ../../src/gui
endif

$(TARGET): $(LIB_OBJS)
>ar rcs $(TARGET) $(LIB_OBJS)

$(RUNNER): model-deps $(OBJS)
>$(CXX) $(CXXFLAGS) -o $(RUNNER) $(OBJS) -lcppunit $(MODEL_LIBS) $(LIBS)

objs: $(OBJS)
>rm -f *.gcda
>rm -f *.gcov
>rm -f *.gcno

.PHONY: coverage-clean
coverage-clean:
>rm -f $(OBJS) $(DEPS)
>rm -f *.gcda
>rm -f *.gcov
>rm -f *.gcno

clean:
>rm -f $(OBJS) $(DEPS)
>rm -f $(TARGET)
>rm -f $(RUNNER)

-include $(DEPS)
