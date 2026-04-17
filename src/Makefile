ifeq ($(COVERAGE),1)
COVERAGE_FLAGS = -fprofile-arcs -ftest-coverage
else
COVERAGE_FLAGS =
endif

.DEFAULT_GOAL := all

CXXFLAGS = -I../include -g `wx-config --cxxflags` -Wall -Woverloaded-virtual -DLINUX $(COVERAGE_FLAGS)
DEPFLAGS = -MMD -MP

LIBS = -Lstrategic -lstrategic -Lships -lships -Ltactical -ltactical \
	-Lweapons -lweapons -Ldefenses -ldefenses -Lcore -lcore -Lgui -lgui \
	`wx-config --libs`

BS_LIBS = -LbattleSim -lbattleSim -Lstrategic -lstrategic -Lships -lships -Ltactical -ltactical \
	-Lweapons -lweapons -Ldefenses -ldefenses -Lcore -lcore -Lgui -lgui \
	`wx-config --libs`

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

OBJS = FApp.o Frontier.o FMainFrame.o FGamePanel.o
DEPS = $(OBJS:.o=.d) $(BS_OBJS:.o=.d)

OBJ_LIBS = libcore.a libgui.a libstrategic.a libtactical.a libweapons.a libdefenses.a libships.a

BS_OBJS = FBattleSimApp.o BattleSim.o 

BS_OBJ_LIBS = libcore.a libgui.a libstrategic.a libtactical.a libweapons.a libdefenses.a libships.a libbattleSim.a

TARGET = SSW

BS_TARGET=BattleSim

$(OBJS) $(BS_OBJS): Makefile

all: SSW BattleSim

SSW: $(OBJ_LIBS) $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

BattleSim: $(BS_OBJ_LIBS) $(BS_OBJS)
	$(CXX) $(CXXFLAGS) -o $(BS_TARGET) $(BS_OBJS) $(BS_LIBS)

libcore.a:
	@ cd core; $(MAKE) $@
	
libgui.a:
	@ cd gui; $(MAKE) $@
	
libships.a:
	@ cd ships; $(MAKE) $@
	
libstrategic.a:
	@ cd strategic; $(MAKE) $@
	
libtactical.a:
	@ cd tactical; $(MAKE) $@
	
libweapons.a:
	@ cd weapons; $(MAKE) $@
	
libdefenses.a:
	@ cd defenses; $(MAKE) $@
	
libbattleSim.a:
	@ cd battleSim; $(MAKE) $@
	
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
	@ cd defenses; ${MAKE} clean
	@ cd battleSim; ${MAKE} clean
	rm -f $(OBJS) $(DEPS)
	rm -f  $(TARGET)
	rm -f $(BS_OBJS)
	rm -f $(BS_TARGET)
	
clean:
	rm -f $(OBJS) $(DEPS)
	rm -f $(TARGET)
	rm -f $(BS_OBJS)
	rm -f $(BS_TARGET)

-include $(DEPS)
