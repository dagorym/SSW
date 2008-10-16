#CXXFLAGS = -I../include -I/usr/local/lib/wx/include/gtk2-ansi-release-2.8 \
#            -I/usr/local/include/wx-2.8 \
#            -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D__WXGTK__ -pthread -g 

#LIBS = -L/usr/local/lib -pthread -lwx_gtk2_richtext-2.8 -lwx_gtk2_aui-2.8 -lwx_gtk2_xrc-2.8 \
#       -lwx_gtk2_qa-2.8 -lwx_gtk2_html-2.8 -lwx_gtk2_adv-2.8 -lwx_gtk2_core-2.8 \
#       -lwx_base_xml-2.8 -lwx_base_net-2.8 -lwx_base-2.8

CXXFLAGS = -I../include -g `wx-config --cxxflags` -Wall -Woverloaded-virtual 

LIBS = `wx-config --libs`

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

OBJS = FObject.o FPObject.o FVehicle.o FFleet.o \
       FPlayer.o FAstroBody.o FPlanet.o FSystem.o FJumpRoute.o FMap.o \
       FGame.o FApp.o FMainFrame.o Frontier.o SystemDialogGUI.o SelectJumpGUI.o \
       FAssaultScout.o FAssaultCarrier.o FBattleship.o FDestroyer.o FFighter.o \
       FFrigate.o FHvCruiser.o FLtCruiser.o FMinelayer.o ViewFleetGUI.o \
       UPFUnattachedGUI.o SatharFleetsGUI.o FFortress.o FArmedStation.o \
       FFortifiedStation.o TransferShipsGUI.o  SatharRetreatGUI.o \
       SelectCombatGUI.o TwoPlanetsGUI.o CombatLocationGUI.o \
       CombatFleetsGUI.o SelectResolutionGUI.o BattleResultsGUI.o \
       FBattleBoard.o FBattleScreen.o FBattleDisplay.o

#OBJS = FApp.o FMainFrame.o Frontier.o FObject.o FPObject.o

TARGET = SSW

all: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

.PHONY:
clean:
	rm $(OBJS)
	rm $(TARGET)


# DO NOT DELETE

BattleResultsGUI.o: ../include/BattleResultsGUI.h ../include/FFleet.h
BattleResultsGUI.o: ../include/FPObject.h ../include/FObject.h
BattleResultsGUI.o: ../include/FVehicle.h ../include/FPlayer.h
BattleResultsGUI.o: ../include/FMap.h ../include/FSystem.h
BattleResultsGUI.o: ../include/FPlanet.h ../include/FAstroBody.h
BattleResultsGUI.o: ../include/FJumpRoute.h ../include/FGame.h
CombatFleetsGUI.o: ../include/CombatFleetsGUI.h ../include/FSystem.h
CombatFleetsGUI.o: ../include/FPObject.h ../include/FObject.h
CombatFleetsGUI.o: ../include/FPlanet.h ../include/FAstroBody.h
CombatFleetsGUI.o: ../include/FVehicle.h ../include/FFleet.h
CombatFleetsGUI.o: ../include/ViewFleetGUI.h
CombatLocationGUI.o: ../include/CombatLocationGUI.h ../include/FPlanet.h
CombatLocationGUI.o: ../include/FAstroBody.h ../include/FPObject.h
CombatLocationGUI.o: ../include/FObject.h ../include/FVehicle.h
FApp.o: ../include/FApp.h ../include/FObject.h ../include/FMainFrame.h
FApp.o: ../include/FGame.h ../include/FPObject.h ../include/FPlayer.h
FApp.o: ../include/FFleet.h ../include/FVehicle.h ../include/FMap.h
FApp.o: ../include/FSystem.h ../include/FPlanet.h ../include/FAstroBody.h
FApp.o: ../include/FJumpRoute.h
FArmedStation.o: ../include/FArmedStation.h ../include/FVehicle.h
FArmedStation.o: ../include/FPObject.h ../include/FObject.h
FAssaultCarrier.o: ../include/FAssaultCarrier.h ../include/FVehicle.h
FAssaultCarrier.o: ../include/FPObject.h ../include/FObject.h
FAssaultScout.o: ../include/FAssaultScout.h ../include/FVehicle.h
FAssaultScout.o: ../include/FPObject.h ../include/FObject.h
FAstroBody.o: ../include/FAstroBody.h ../include/FPObject.h
FAstroBody.o: ../include/FObject.h
FBattleBoard.o: ../include/FBattleBoard.h ../include/Frontier.h
FBattleBoard.o: ../include/wxWidgets.h ../include/FVehicle.h
FBattleBoard.o: ../include/FPObject.h ../include/FObject.h
FBattleDisplay.o: ../include/FBattleDisplay.h ../include/Frontier.h
FBattleDisplay.o: ../include/wxWidgets.h ../include/FVehicle.h
FBattleDisplay.o: ../include/FPObject.h ../include/FObject.h
FBattleScreen.o: ../include/FBattleScreen.h ../include/FBattleBoard.h
FBattleScreen.o: ../include/FBattleDisplay.h ../include/Frontier.h
FBattleScreen.o: ../include/wxWidgets.h ../include/FVehicle.h
FBattleScreen.o: ../include/FPObject.h ../include/FObject.h
FBattleship.o: ../include/FBattleship.h ../include/FVehicle.h
FBattleship.o: ../include/FPObject.h ../include/FObject.h
FDestroyer.o: ../include/FDestroyer.h ../include/FVehicle.h
FDestroyer.o: ../include/FPObject.h ../include/FObject.h
FFighter.o: ../include/FFighter.h ../include/FVehicle.h ../include/FPObject.h
FFighter.o: ../include/FObject.h
FFleet.o: ../include/FFleet.h ../include/FPObject.h ../include/FObject.h
FFleet.o: ../include/FVehicle.h ../include/Frontier.h ../include/wxWidgets.h
FFleet.o: ../include/FJumpRoute.h ../include/FSystem.h ../include/FPlanet.h
FFleet.o: ../include/FAstroBody.h
FFortifiedStation.o: ../include/FFortifiedStation.h ../include/FVehicle.h
FFortifiedStation.o: ../include/FPObject.h ../include/FObject.h
FFortress.o: ../include/FFortress.h ../include/FVehicle.h
FFortress.o: ../include/FPObject.h ../include/FObject.h
FFrigate.o: ../include/FFrigate.h ../include/FVehicle.h ../include/FPObject.h
FFrigate.o: ../include/FObject.h
FGame.o: ../include/FGame.h ../include/FPObject.h ../include/FObject.h
FGame.o: ../include/FPlayer.h ../include/FFleet.h ../include/FVehicle.h
FGame.o: ../include/FMap.h ../include/FSystem.h ../include/FPlanet.h
FGame.o: ../include/FAstroBody.h ../include/FJumpRoute.h
FGame.o: ../include/SystemDialogGUI.h ../include/Frontier.h
FGame.o: ../include/wxWidgets.h ../include/UPFUnattachedGUI.h
FGame.o: ../include/SatharFleetsGUI.h ../include/ships.h
FGame.o: ../include/FFighter.h ../include/FAssaultScout.h
FGame.o: ../include/FFrigate.h ../include/FDestroyer.h
FGame.o: ../include/FMinelayer.h ../include/FLtCruiser.h
FGame.o: ../include/FHvCruiser.h ../include/FAssaultCarrier.h
FGame.o: ../include/FBattleship.h ../include/FFortress.h
FGame.o: ../include/FFortifiedStation.h ../include/FArmedStation.h
FGame.o: ../include/ViewFleetGUI.h ../include/SatharRetreatGUI.h
FGame.o: ../include/SelectCombatGUI.h
FHvCruiser.o: ../include/FHvCruiser.h ../include/FVehicle.h
FHvCruiser.o: ../include/FPObject.h ../include/FObject.h
FJumpRoute.o: ../include/FJumpRoute.h ../include/FPObject.h
FJumpRoute.o: ../include/FObject.h ../include/FSystem.h ../include/FPlanet.h
FJumpRoute.o: ../include/FAstroBody.h ../include/FVehicle.h
FJumpRoute.o: ../include/FFleet.h
FLtCruiser.o: ../include/FLtCruiser.h ../include/FVehicle.h
FLtCruiser.o: ../include/FPObject.h ../include/FObject.h
FMainFrame.o: ../include/FMainFrame.h ../include/FGame.h
FMainFrame.o: ../include/FPObject.h ../include/FObject.h ../include/FPlayer.h
FMainFrame.o: ../include/FFleet.h ../include/FVehicle.h ../include/FMap.h
FMainFrame.o: ../include/FSystem.h ../include/FPlanet.h
FMainFrame.o: ../include/FAstroBody.h ../include/FJumpRoute.h
FMainFrame.o: ../include/Frontier.h ../include/wxWidgets.h
FMainFrame.o: ../include/FBattleScreen.h ../include/FBattleBoard.h
FMainFrame.o: ../include/FBattleDisplay.h
FMap.o: ../include/FMap.h ../include/FPObject.h ../include/FObject.h
FMap.o: ../include/FSystem.h ../include/FPlanet.h ../include/FAstroBody.h
FMap.o: ../include/FVehicle.h ../include/FFleet.h ../include/FJumpRoute.h
FMinelayer.o: ../include/FMinelayer.h ../include/FVehicle.h
FMinelayer.o: ../include/FPObject.h ../include/FObject.h
FObject.o: ../include/FObject.h
FPlanet.o: ../include/FPlanet.h ../include/FAstroBody.h ../include/FPObject.h
FPlanet.o: ../include/FObject.h ../include/FVehicle.h ../include/Frontier.h
FPlanet.o: ../include/wxWidgets.h
FPlayer.o: ../include/FPlayer.h ../include/FPObject.h ../include/FObject.h
FPlayer.o: ../include/FFleet.h ../include/FVehicle.h ../include/FMap.h
FPlayer.o: ../include/FSystem.h ../include/FPlanet.h ../include/FAstroBody.h
FPlayer.o: ../include/FJumpRoute.h ../include/Frontier.h
FPlayer.o: ../include/wxWidgets.h ../include/ships.h ../include/FFighter.h
FPlayer.o: ../include/FAssaultScout.h ../include/FFrigate.h
FPlayer.o: ../include/FDestroyer.h ../include/FMinelayer.h
FPlayer.o: ../include/FLtCruiser.h ../include/FHvCruiser.h
FPlayer.o: ../include/FAssaultCarrier.h ../include/FBattleship.h
FPlayer.o: ../include/FFortress.h ../include/FFortifiedStation.h
FPlayer.o: ../include/FArmedStation.h
FPObject.o: ../include/FPObject.h ../include/FObject.h
Frontier.o: ../include/FApp.h ../include/FObject.h ../include/FMainFrame.h
Frontier.o: ../include/FGame.h ../include/FPObject.h ../include/FPlayer.h
Frontier.o: ../include/FFleet.h ../include/FVehicle.h ../include/FMap.h
Frontier.o: ../include/FSystem.h ../include/FPlanet.h ../include/FAstroBody.h
Frontier.o: ../include/FJumpRoute.h ../include/Frontier.h
Frontier.o: ../include/wxWidgets.h ../include/ships.h ../include/FFighter.h
Frontier.o: ../include/FAssaultScout.h ../include/FFrigate.h
Frontier.o: ../include/FDestroyer.h ../include/FMinelayer.h
Frontier.o: ../include/FLtCruiser.h ../include/FHvCruiser.h
Frontier.o: ../include/FAssaultCarrier.h ../include/FBattleship.h
Frontier.o: ../include/FFortress.h ../include/FFortifiedStation.h
Frontier.o: ../include/FArmedStation.h
FSystem.o: ../include/FSystem.h ../include/FPObject.h ../include/FObject.h
FSystem.o: ../include/FPlanet.h ../include/FAstroBody.h ../include/FVehicle.h
FSystem.o: ../include/FFleet.h
FVehicle.o: ../include/FVehicle.h ../include/FPObject.h ../include/FObject.h
SatharFleetsGUI.o: ../include/SatharFleetsGUI.h ../include/FPlayer.h
SatharFleetsGUI.o: ../include/FPObject.h ../include/FObject.h
SatharFleetsGUI.o: ../include/FFleet.h ../include/FVehicle.h
SatharFleetsGUI.o: ../include/FMap.h ../include/FSystem.h
SatharFleetsGUI.o: ../include/FPlanet.h ../include/FAstroBody.h
SatharFleetsGUI.o: ../include/FJumpRoute.h
SatharRetreatGUI.o: ../include/SatharRetreatGUI.h
SelectCombatGUI.o: ../include/SelectCombatGUI.h ../include/FSystem.h
SelectCombatGUI.o: ../include/FPObject.h ../include/FObject.h
SelectCombatGUI.o: ../include/FPlanet.h ../include/FAstroBody.h
SelectCombatGUI.o: ../include/FVehicle.h ../include/FFleet.h
SelectCombatGUI.o: ../include/FPlayer.h ../include/FMap.h
SelectCombatGUI.o: ../include/FJumpRoute.h ../include/ViewFleetGUI.h
SelectCombatGUI.o: ../include/TwoPlanetsGUI.h ../include/CombatLocationGUI.h
SelectCombatGUI.o: ../include/CombatFleetsGUI.h
SelectCombatGUI.o: ../include/SelectResolutionGUI.h
SelectCombatGUI.o: ../include/BattleResultsGUI.h
SelectJumpGUI.o: ../include/SelectJumpGUI.h ../include/FMap.h
SelectJumpGUI.o: ../include/FPObject.h ../include/FObject.h
SelectJumpGUI.o: ../include/FSystem.h ../include/FPlanet.h
SelectJumpGUI.o: ../include/FAstroBody.h ../include/FVehicle.h
SelectJumpGUI.o: ../include/FFleet.h ../include/FJumpRoute.h
SelectResolutionGUI.o: ../include/SelectResolutionGUI.h ../include/FFleet.h
SelectResolutionGUI.o: ../include/FPObject.h ../include/FObject.h
SelectResolutionGUI.o: ../include/FVehicle.h
SystemDialogGUI.o: ../include/SystemDialogGUI.h ../include/FMap.h
SystemDialogGUI.o: ../include/FPObject.h ../include/FObject.h
SystemDialogGUI.o: ../include/FSystem.h ../include/FPlanet.h
SystemDialogGUI.o: ../include/FAstroBody.h ../include/FVehicle.h
SystemDialogGUI.o: ../include/FFleet.h ../include/FJumpRoute.h
SystemDialogGUI.o: ../include/FPlayer.h ../include/SelectJumpGUI.h
SystemDialogGUI.o: ../include/ViewFleetGUI.h ../include/TransferShipsGUI.h
TransferShipsGUI.o: ../include/TransferShipsGUI.h ../include/FPlayer.h
TransferShipsGUI.o: ../include/FPObject.h ../include/FObject.h
TransferShipsGUI.o: ../include/FFleet.h ../include/FVehicle.h
TransferShipsGUI.o: ../include/FMap.h ../include/FSystem.h
TransferShipsGUI.o: ../include/FPlanet.h ../include/FAstroBody.h
TransferShipsGUI.o: ../include/FJumpRoute.h
TwoPlanetsGUI.o: ../include/TwoPlanetsGUI.h ../include/FSystem.h
TwoPlanetsGUI.o: ../include/FPObject.h ../include/FObject.h
TwoPlanetsGUI.o: ../include/FPlanet.h ../include/FAstroBody.h
TwoPlanetsGUI.o: ../include/FVehicle.h ../include/FFleet.h
UPFUnattachedGUI.o: ../include/UPFUnattachedGUI.h ../include/FPlayer.h
UPFUnattachedGUI.o: ../include/FPObject.h ../include/FObject.h
UPFUnattachedGUI.o: ../include/FFleet.h ../include/FVehicle.h
UPFUnattachedGUI.o: ../include/FMap.h ../include/FSystem.h
UPFUnattachedGUI.o: ../include/FPlanet.h ../include/FAstroBody.h
UPFUnattachedGUI.o: ../include/FJumpRoute.h
ViewFleetGUI.o: ../include/ViewFleetGUI.h ../include/FFleet.h
ViewFleetGUI.o: ../include/FPObject.h ../include/FObject.h
ViewFleetGUI.o: ../include/FVehicle.h ../include/FSystem.h
ViewFleetGUI.o: ../include/FPlanet.h ../include/FAstroBody.h
