CXXFLAGS = -I../include -g `wx-config --cxxflags` -Wall -Woverloaded-virtual -DLINUX -fprofile-arcs -ftest-coverage

LIBS = `wx-config --libs` -Lstrategic -lstrategic -Lgui -lgui -Lships -lships \
       -Ltactical -ltactical -Lweapons -lweapons -Lcore -lcore

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

OBJ_LIBS = libcore.a libgui.a libships.a libstrategic.a libtactical.a libweapons.a

TARGET = SSW

all: $(OBJ_LIBS) $(OBJS)
	rm *.gcda
	rm *.gcov
	rm *.gcno
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

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
	
objs: $(OBJS)

.PHONY:
all_clean:
	@ cd core; ${MAKE} clean
	@ cd gui; ${MAKE} clean
	@ cd ships; ${MAKE} clean
	@ cd strategic; ${MAKE} clean
	@ cd tactical; ${MAKE} clean
	@ cd weapons; ${MAKE} clean
	rm $(OBJS)
	rm $(TARGET)
	
clean:
	rm $(OBJS)
	rm $(TARGET)

# DO NOT DELETE

FApp.o: ../include/FApp.h ../include/core/FObject.h ../include/FMainFrame.h
FApp.o: ../include/strategic/FGame.h ../include/core/FPObject.h
FApp.o: ../include/core/FObject.h ../include/strategic/FPlayer.h
FApp.o: ../include/strategic/FFleet.h ../include/ships/FVehicle.h
FApp.o: ../include/weapons/FWeapon.h ../include/strategic/FMap.h
FApp.o: ../include/strategic/FSystem.h ../include/strategic/FPlanet.h
FApp.o: ../include/strategic/FAstroBody.h ../include/strategic/FJumpRoute.h
FMainFrame.o: ../include/FMainFrame.h ../include/strategic/FGame.h
FMainFrame.o: ../include/core/FPObject.h ../include/core/FObject.h
FMainFrame.o: ../include/strategic/FPlayer.h ../include/strategic/FFleet.h
FMainFrame.o: ../include/ships/FVehicle.h ../include/weapons/FWeapon.h
FMainFrame.o: ../include/strategic/FMap.h ../include/strategic/FSystem.h
FMainFrame.o: ../include/strategic/FPlanet.h
FMainFrame.o: ../include/strategic/FAstroBody.h
FMainFrame.o: ../include/strategic/FJumpRoute.h ../include/Frontier.h
FMainFrame.o: ../include/wxWidgets.h ../include/tactical/FBattleScreen.h
FMainFrame.o: ../include/tactical/FBattleBoard.h
FMainFrame.o: ../include/strategic/FFleet.h
FMainFrame.o: ../include/tactical/FBattleDisplay.h
Frontier.o: ../include/FApp.h ../include/core/FObject.h
Frontier.o: ../include/FMainFrame.h ../include/strategic/FGame.h
Frontier.o: ../include/core/FPObject.h ../include/core/FObject.h
Frontier.o: ../include/strategic/FPlayer.h ../include/strategic/FFleet.h
Frontier.o: ../include/ships/FVehicle.h ../include/weapons/FWeapon.h
Frontier.o: ../include/strategic/FMap.h ../include/strategic/FSystem.h
Frontier.o: ../include/strategic/FPlanet.h ../include/strategic/FAstroBody.h
Frontier.o: ../include/strategic/FJumpRoute.h
Frontier.o: ../include/tactical/FBattleScreen.h ../include/Frontier.h
Frontier.o: ../include/wxWidgets.h ../include/tactical/FBattleBoard.h
Frontier.o: ../include/strategic/FFleet.h
Frontier.o: ../include/tactical/FBattleDisplay.h
