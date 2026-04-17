ifeq ($(COVERAGE),1)
COVERAGE_FLAGS = -fprofile-arcs -ftest-coverage
else
COVERAGE_FLAGS =
endif

.DEFAULT_GOAL := all

CXXFLAGS = -I../include  -g `wx-config --cxxflags` -Wall -Woverloaded-virtual -DLINUX $(COVERAGE_FLAGS)
DEPFLAGS = -MMD -MP

LIBS = -L./strategic -lstrategicTests -L./ships -lshipsTests \
	-L./tactical -ltacticalTests \
	-L./weapons -lweaponsTests -L./core -lcoreTests \
	-lcppunit -L../src/strategic -lstrategic -L../src/ships -lships -L../src/gui -lgui -L../src/tactical -ltactical \
	-L../src/weapons -lweapons -L../src/defenses -ldefenses \
	-L../src/core -lcore -L../src/gui -lgui \
	`wx-config --libs`
	 #-l wxGuiTesting
       

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

############################# Paths #################################

OBJS = SSWTests.o #FApp.o Frontier.o FMainFrameTest.o 
DEPS = $(OBJS:.o=.d)

OBJ_LIBS = libcoreTests.a libweaponsTests.a libstrategicTests.a libshipsTests.a libtacticalTests.a #libguiTests.a

$(OBJS): Makefile

TARGET = SSWTests

all: model-deps $(OBJ_LIBS) $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

model-deps:
	$(MAKE) -C ../src/core
	$(MAKE) -C ../src/strategic
	$(MAKE) -C ../src/ships
	$(MAKE) -C ../src/defenses
	$(MAKE) -C ../src/weapons
	$(MAKE) -C ../src/tactical
	$(MAKE) -C ../src/gui

gui-tests: model-deps
	@ cd gui; $(MAKE) all

tactical-tests:
	@ cd tactical; $(MAKE) clean TacticalTests

libcoreTests.a:
	@ cd core; $(MAKE) $@
	
libguiTests.a:
	@ cd gui; $(MAKE) $@
	
libshipsTests.a:
	@ cd ships; $(MAKE) $@
	
libstrategicTests.a:
	@ cd strategic; $(MAKE) $@

libtacticalTests.a:
	@ cd tactical; $(MAKE) $@
	
libweaponsTests.a:
	@ cd weapons; $(MAKE) $@
	
objs: $(OBJS)
	rm *.gcda
	rm *.gcov
	rm *.gcno

.PHONY:
all_clean:
	@ cd core; ${MAKE} clean
	@ cd gui; ${MAKE} clean
	@ cd ships; ${MAKE} clean
	@ cd strategic; ${MAKE} clean
	@ cd tactical; ${MAKE} clean
	@ cd weapons; ${MAKE} clean
	rm -f $(OBJS) $(DEPS)
	rm -f $(TARGET)
	
clean:
	rm -f $(OBJS) $(DEPS)
	rm -f $(TARGET)

-include $(DEPS)
