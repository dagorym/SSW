CXXFLAGS = -I../include -g `wx-config --cxxflags` -Wall -Woverloaded-virtual -DLINUX -fprofile-arcs -ftest-coverage

LIBS = `wx-config --libs` -Lstrategic -lstrategic -Lships -lships -Ltactical -ltactical \
       -Lweapons -lweapons -Ldefenses -ldefenses -Lcore -lcore -Lgui -lgui

BS_LIBS = `wx-config --libs` -LbattleSim -lbattleSim -Lstrategic -lstrategic -Lships -lships -Ltactical -ltactical \
       -Lweapons -lweapons -Ldefenses -ldefenses -Lcore -lcore -Lgui -lgui

#################### the implicit rules #############################

# NB: we use the old style suffix rules (.c.o) and not the GNU make
#     implicit rules (%.o: %.c) because they are more portable, in
#     particular the BSD make understands the former but not the
#     latter
.SUFFIXES: .o .c .cpp .cxx .rsrc .r

.c.o:
	$(CC) -c $(CFLAGS) $(PICFLAGS) -o $@ $<

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(PICFLAGS) -o $@ $<

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(PICFLAGS) -o $@ $<

.rsrc.r:
	$(DEREZ) $^ Carbon.r -useDF > $@

############################# Paths #################################

OBJS = FApp.o Frontier.o FMainFrame.o

OBJ_LIBS = libcore.a libgui.a libstrategic.a libtactical.a libweapons.a libdefenses.a libships.a

BS_OBJS = FBattleSimApp.o BattleSim.o 

BS_OBJ_LIBS = libcore.a libgui.a libstrategic.a libtactical.a libweapons.a libdefenses.a libships.a libbattleSim.a

TARGET = SSW

BS_TARGET=BattleSim

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
	rm $(OBJS)
	rm $(TARGET)
	rm $(BS_OBJS)
	rm $(BS_TARGET)
	
clean:
	rm $(OBJS)
	rm $(TARGET)
	rm $(BS_OBJS)
	rm $(BS_TARGET)

# DO NOT DELETE

BattleSim.o: ../include/FBattleSimApp.h ../include/core/FObject.h
BattleSim.o: ../include/BattleSim.h ../include/wxWidgets.h
BattleSim.o: ../include/battleSim/BattleSimFrame.h
FApp.o: ../include/FApp.h ../include/core/FObject.h ../include/FMainFrame.h
FApp.o: ../include/strategic/FGame.h ../include/core/FPObject.h
FApp.o: ../include/core/FObject.h ../include/strategic/FPlayer.h
FApp.o: ../include/strategic/FFleet.h ../include/ships/FVehicle.h
FApp.o: ../include/core/FGameConfig.h ../include/weapons/FWeapon.h
FApp.o: ../include/defenses/FDefense.h ../include/strategic/FMap.h
FApp.o: ../include/strategic/FSystem.h ../include/strategic/FPlanet.h
FApp.o: ../include/strategic/FAstroBody.h ../include/strategic/FJumpRoute.h
FBattleSimApp.o: ../include/FBattleSimApp.h ../include/core/FObject.h
FBattleSimApp.o: ../include/battleSim/BattleSimFrame.h
FBattleSimApp.o: ../include/core/FGameConfig.h ../include/core/FPObject.h
FBattleSimApp.o: ../include/core/FObject.h
FMainBattleSimFrame.o: ../include/tactical/FBattleScreen.h
FMainBattleSimFrame.o: ../include/Frontier.h ../include/wxWidgets.h
FMainBattleSimFrame.o: ../include/tactical/FBattleBoard.h
FMainBattleSimFrame.o: ../include/strategic/FFleet.h
FMainBattleSimFrame.o: ../include/core/FPObject.h ../include/core/FObject.h
FMainBattleSimFrame.o: ../include/ships/FVehicle.h
FMainBattleSimFrame.o: ../include/core/FGameConfig.h
FMainBattleSimFrame.o: ../include/weapons/FWeapon.h
FMainBattleSimFrame.o: ../include/defenses/FDefense.h
FMainBattleSimFrame.o: ../include/core/FPoint.h
FMainBattleSimFrame.o: ../include/tactical/FBattleDisplay.h
FMainFrame.o: ../include/FMainFrame.h ../include/strategic/FGame.h
FMainFrame.o: ../include/core/FPObject.h ../include/core/FObject.h
FMainFrame.o: ../include/strategic/FPlayer.h ../include/strategic/FFleet.h
FMainFrame.o: ../include/ships/FVehicle.h ../include/core/FGameConfig.h
FMainFrame.o: ../include/weapons/FWeapon.h ../include/defenses/FDefense.h
FMainFrame.o: ../include/strategic/FMap.h ../include/strategic/FSystem.h
FMainFrame.o: ../include/strategic/FPlanet.h
FMainFrame.o: ../include/strategic/FAstroBody.h
FMainFrame.o: ../include/strategic/FJumpRoute.h ../include/Frontier.h
FMainFrame.o: ../include/wxWidgets.h ../include/tactical/FBattleScreen.h
FMainFrame.o: ../include/tactical/FBattleBoard.h
FMainFrame.o: ../include/strategic/FFleet.h ../include/core/FPoint.h
FMainFrame.o: ../include/tactical/FBattleDisplay.h
Frontier.o: ../include/FApp.h ../include/core/FObject.h
Frontier.o: ../include/FMainFrame.h ../include/strategic/FGame.h
Frontier.o: ../include/core/FPObject.h ../include/core/FObject.h
Frontier.o: ../include/strategic/FPlayer.h ../include/strategic/FFleet.h
Frontier.o: ../include/ships/FVehicle.h ../include/core/FGameConfig.h
Frontier.o: ../include/weapons/FWeapon.h ../include/defenses/FDefense.h
Frontier.o: ../include/strategic/FMap.h ../include/strategic/FSystem.h
Frontier.o: ../include/strategic/FPlanet.h ../include/strategic/FAstroBody.h
Frontier.o: ../include/strategic/FJumpRoute.h ../include/Frontier.h
Frontier.o: ../include/wxWidgets.h
