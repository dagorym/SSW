.RECIPEPREFIX = >
ifeq ($(COVERAGE),1)
COVERAGE_FLAGS = -fprofile-arcs -ftest-coverage
else
COVERAGE_FLAGS =
endif

CXXFLAGS = -I../../include -g `wx-config --cxxflags` -Wall -Woverloaded-virtual -DLINUX $(COVERAGE_FLAGS)

LIBS = `wx-config --libs`
MODEL_LIBS = -L../../src/battleSim -L../../src/gui -L../../src/tactical -L../../src/weapons -L../../src/defenses -L../../src/ships -L../../src/strategic -L../../src/core -Wl,--start-group -lbattleSim -lgui -ltactical -lweapons -ldefenses -lships -lstrategic -lcore -Wl,--end-group

PICFLAGS = -fPIC -DPIC

.SUFFIXES: .o .c .cpp .cxx .rsrc .r

.c.o:
>$(CC) -c $(CFLAGS) $(PICFLAGS) -o $@ $<

.cpp.o:
>$(CXX) -c $(CXXFLAGS) $(PICFLAGS) -o $@ $<

.cxx.o:
>$(CXX) -c $(CXXFLAGS) $(PICFLAGS) -o $@ $<

.rsrc.r:
>$(DEREZ) $^ Carbon.r -useDF > $@

OBJS = WXGuiTestHarness.o GuiHarnessTest.o GuiTests.o
TARGET = libguiTests.a
RUNNER = GuiTests

all: model-deps $(TARGET) $(RUNNER)

model-deps:
>$(MAKE) -C ../../src/core
>$(MAKE) -C ../../src/strategic
>$(MAKE) -C ../../src/ships
>$(MAKE) -C ../../src/defenses
>$(MAKE) -C ../../src/weapons
>$(MAKE) -C ../../src/tactical
>$(MAKE) -C ../../src/gui
>$(MAKE) -C ../../src/battleSim

$(TARGET): WXGuiTestHarness.o GuiHarnessTest.o
>ar rcs $(TARGET) WXGuiTestHarness.o GuiHarnessTest.o

$(RUNNER): model-deps $(OBJS)
>$(CXX) $(CXXFLAGS) -o $(RUNNER) $(OBJS) -lcppunit $(MODEL_LIBS) $(LIBS)

objs: $(OBJS)
>rm -f *.gcda
>rm -f *.gcov
>rm -f *.gcno

.PHONY:
clean:
>rm -f $(OBJS)
>rm -f $(TARGET)
>rm -f $(RUNNER)
