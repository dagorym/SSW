#CXXFLAGS = -I../include -I/usr/local/lib/wx/include/gtk2-ansi-release-2.8 \
#            -I/usr/local/include/wx-2.8 \
#            -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D__WXGTK__ -pthread -g 

#LIBS = -L/usr/local/lib -pthread -lwx_gtk2_richtext-2.8 -lwx_gtk2_aui-2.8 -lwx_gtk2_xrc-2.8 \
#       -lwx_gtk2_qa-2.8 -lwx_gtk2_html-2.8 -lwx_gtk2_adv-2.8 -lwx_gtk2_core-2.8 \
#       -lwx_base_xml-2.8 -lwx_base_net-2.8 -lwx_base-2.8

CXXFLAGS = -I../../include -g `wx-config --cxxflags` -Wall -Woverloaded-virtual -DLINUX -fprofile-arcs -ftest-coverage

LIBS = `wx-config --libs`

PICFLAGS = -fPIC -DPIC

#CXXFLAGS = -I../include -I/home/tstephen/src/wxGTK-2.8.7/buildgtkm/lib/wx/include/gtk2-ansi-release-static-2.8 -I/home/tstephen/src/wxGTK-2.8.7/include -I/home/tstephen/src/wxGTK-2.8.7/contrib/include -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D__WXGTK__ -pthread -g
#LIBS = -L/home/tstephen/src/wxGTK-2.8.7/buildgtkm/lib -pthread   /home/tstephen/src/wxGTK-2.8.7/buildgtkm/lib/libwx_gtk2-2.8.a -pthread -L/lib -lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lgdk_pixbuf-2.0 -lm -lpango-1.0 -lgobject-2.0 -lgmodule-2.0 -ldl -lgthread-2.0 -lglib-2.0 -lXinerama -lXxf86vm -lSM -lwxregex-2.8 -lwxexpat-2.8 -lwxtiff-2.8 -lwxjpeg-2.8 -lwxpng-2.8 -lwxzlib-2.8 -ldl -lm

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

OBJS = FObject.o FPObject.o FPoint.o
#       FVehicle.o FFleet.o \
#       FPlayer.o FAstroBody.o FPlanet.o FSystem.o FJumpRoute.o FMap.o \
#       FGame.o FApp.o FMainFrame.o Frontier.o SystemDialogGUI.o SelectJumpGUI.o \
#       FAssaultScout.o FAssaultCarrier.o FBattleship.o FDestroyer.o FFighter.o \
#       FFrigate.o FHvCruiser.o FLtCruiser.o FMinelayer.o ViewFleetGUI.o \
#       UPFUnattachedGUI.o SatharFleetsGUI.o FFortress.o FArmedStation.o \
#       FFortifiedStation.o TransferShipsGUI.o  SatharRetreatGUI.o \
#       SelectCombatGUI.o TwoPlanetsGUI.o CombatLocationGUI.o \
#       CombatFleetsGUI.o SelectResolutionGUI.o BattleResultsGUI.o \
#       FBattleBoard.o FBattleScreen.o FBattleDisplay.o FWeapon.o \
#       FLaserBattery.o FLaserCannon.o FProtonBattery.o FElectronBattery.o \
#       FDisruptorCannon.o FRocketBattery.o FAssaultRocket.o FTorpedo.o \
#       FSeekerMissileLauncher.o FMineLauncher.o

#OBJS = FApp.o FMainFrame.o Frontier.o FObject.o FPObject.o

TARGET = libcore.a

all $(TARGET): $(OBJS)
	ar rcsu $(TARGET) $(OBJS)

#	$(CXX) $(CXXFLAGS) -shared -o $(TARGET) $(OBJS)# $(LIBS)
	
objs: $(OBJS)
	rm *.gcda
	rm *.gcov
	rm *.gcno

.PHONY:
clean:
	rm $(OBJS)
	rm $(TARGET)


# DO NOT DELETE

FObject.o: ../../include/core/FObject.h
FPObject.o: ../../include/core/FPObject.h ../../include/core/FObject.h
