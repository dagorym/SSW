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

OBJS = SystemDialogGUI.o SelectJumpGUI.o ViewFleetGUI.o \
       UPFUnattachedGUI.o SatharFleetsGUI.o \
       TransferShipsGUI.o  SatharRetreatGUI.o \
       SelectCombatGUI.o TwoPlanetsGUI.o CombatLocationGUI.o \
       CombatFleetsGUI.o SelectResolutionGUI.o BattleResultsGUI.o \
       WXMapDisplay.o WXPlayerDisplay.o WXGameDisplay.o \
       ICMSelectionGUI.o AssignedICMData.o

TARGET = libgui.a

all $(TARGET): $(OBJS)
	ar rcsu $(TARGET) $(OBJS)

#	$(CXX) $(CXXFLAGS) -shared -o $(TARGET) $(OBJS)# $(LIBS)
	
objs: $(OBJS)
	rm -f *.gcda
	rm -f *.gcov
	rm -f *.gcno

.PHONY:
clean:
	rm -f $(OBJS)
	rm -f $(TARGET)


# DO NOT DELETE

AssignedICMData.o: ../../include/gui/AssignedICMData.h
AssignedICMData.o: ../../include/ships/FVehicle.h
AssignedICMData.o: ../../include/core/FPObject.h ../../include/core/FObject.h
AssignedICMData.o: ../../include/core/FGameConfig.h
AssignedICMData.o: ../../include/weapons/FWeapon.h
AssignedICMData.o: ../../include/defenses/FDefense.h
BattleResultsGUI.o: ../../include/gui/BattleResultsGUI.h
BattleResultsGUI.o: ../../include/strategic/FFleet.h
BattleResultsGUI.o: ../../include/core/FPObject.h
BattleResultsGUI.o: ../../include/core/FObject.h
BattleResultsGUI.o: ../../include/ships/FVehicle.h
BattleResultsGUI.o: ../../include/core/FGameConfig.h
BattleResultsGUI.o: ../../include/weapons/FWeapon.h
BattleResultsGUI.o: ../../include/defenses/FDefense.h
BattleResultsGUI.o: ../../include/strategic/FPlayer.h
BattleResultsGUI.o: ../../include/strategic/FFleet.h
BattleResultsGUI.o: ../../include/strategic/FMap.h
BattleResultsGUI.o: ../../include/strategic/FSystem.h
BattleResultsGUI.o: ../../include/strategic/FPlanet.h
BattleResultsGUI.o: ../../include/strategic/FAstroBody.h
BattleResultsGUI.o: ../../include/strategic/FJumpRoute.h
BattleResultsGUI.o: ../../include/strategic/FMap.h
BattleResultsGUI.o: ../../include/strategic/FGame.h
BattleResultsGUI.o: ../../include/strategic/FPlayer.h
CombatFleetsGUI.o: ../../include/gui/CombatFleetsGUI.h
CombatFleetsGUI.o: ../../include/strategic/FSystem.h
CombatFleetsGUI.o: ../../include/core/FPObject.h ../../include/core/FObject.h
CombatFleetsGUI.o: ../../include/strategic/FPlanet.h
CombatFleetsGUI.o: ../../include/strategic/FAstroBody.h
CombatFleetsGUI.o: ../../include/ships/FVehicle.h
CombatFleetsGUI.o: ../../include/core/FGameConfig.h
CombatFleetsGUI.o: ../../include/weapons/FWeapon.h
CombatFleetsGUI.o: ../../include/defenses/FDefense.h
CombatFleetsGUI.o: ../../include/strategic/FFleet.h
CombatFleetsGUI.o: ../../include/gui/ViewFleetGUI.h
CombatFleetsGUI.o: ../../include/strategic/FFleet.h
CombatLocationGUI.o: ../../include/gui/CombatLocationGUI.h
CombatLocationGUI.o: ../../include/strategic/FPlanet.h
CombatLocationGUI.o: ../../include/strategic/FAstroBody.h
CombatLocationGUI.o: ../../include/core/FPObject.h
CombatLocationGUI.o: ../../include/core/FObject.h
CombatLocationGUI.o: ../../include/ships/FVehicle.h
CombatLocationGUI.o: ../../include/core/FGameConfig.h
CombatLocationGUI.o: ../../include/weapons/FWeapon.h
CombatLocationGUI.o: ../../include/defenses/FDefense.h
ICMSelectionGUI.o: ../../include/gui/ICMSelectionGUI.h
ICMSelectionGUI.o: ../../include/ships/FVehicle.h
ICMSelectionGUI.o: ../../include/core/FPObject.h ../../include/core/FObject.h
ICMSelectionGUI.o: ../../include/core/FGameConfig.h
ICMSelectionGUI.o: ../../include/weapons/FWeapon.h
ICMSelectionGUI.o: ../../include/defenses/FDefense.h
ICMSelectionGUI.o: ../../include/gui/AssignedICMData.h
SatharFleetsGUI.o: ../../include/gui/SatharFleetsGUI.h
SatharFleetsGUI.o: ../../include/strategic/FPlayer.h
SatharFleetsGUI.o: ../../include/core/FPObject.h ../../include/core/FObject.h
SatharFleetsGUI.o: ../../include/strategic/FFleet.h
SatharFleetsGUI.o: ../../include/strategic/FMap.h
SatharFleetsGUI.o: ../../include/strategic/FSystem.h
SatharFleetsGUI.o: ../../include/strategic/FPlanet.h
SatharFleetsGUI.o: ../../include/strategic/FAstroBody.h
SatharFleetsGUI.o: ../../include/ships/FVehicle.h
SatharFleetsGUI.o: ../../include/core/FGameConfig.h
SatharFleetsGUI.o: ../../include/weapons/FWeapon.h
SatharFleetsGUI.o: ../../include/defenses/FDefense.h
SatharFleetsGUI.o: ../../include/strategic/FJumpRoute.h
SatharFleetsGUI.o: ../../include/strategic/FMap.h
SatharRetreatGUI.o: ../../include/gui/SatharRetreatGUI.h
SelectCombatGUI.o: ../../include/gui/SelectCombatGUI.h
SelectCombatGUI.o: ../../include/strategic/FSystem.h
SelectCombatGUI.o: ../../include/core/FPObject.h ../../include/core/FObject.h
SelectCombatGUI.o: ../../include/strategic/FPlanet.h
SelectCombatGUI.o: ../../include/strategic/FAstroBody.h
SelectCombatGUI.o: ../../include/ships/FVehicle.h
SelectCombatGUI.o: ../../include/core/FGameConfig.h
SelectCombatGUI.o: ../../include/weapons/FWeapon.h
SelectCombatGUI.o: ../../include/defenses/FDefense.h
SelectCombatGUI.o: ../../include/strategic/FFleet.h
SelectCombatGUI.o: ../../include/strategic/FPlayer.h
SelectCombatGUI.o: ../../include/strategic/FMap.h
SelectCombatGUI.o: ../../include/strategic/FSystem.h
SelectCombatGUI.o: ../../include/strategic/FJumpRoute.h
SelectCombatGUI.o: ../../include/gui/ViewFleetGUI.h
SelectCombatGUI.o: ../../include/strategic/FFleet.h
SelectCombatGUI.o: ../../include/gui/TwoPlanetsGUI.h
SelectCombatGUI.o: ../../include/gui/CombatLocationGUI.h
SelectCombatGUI.o: ../../include/strategic/FPlanet.h
SelectCombatGUI.o: ../../include/gui/CombatFleetsGUI.h
SelectCombatGUI.o: ../../include/gui/SelectResolutionGUI.h
SelectCombatGUI.o: ../../include/gui/BattleResultsGUI.h
SelectCombatGUI.o: ../../include/tactical/FBattleScreen.h
SelectCombatGUI.o: ../../include/Frontier.h ../../include/wxWidgets.h
SelectCombatGUI.o: ../../include/tactical/FBattleBoard.h
SelectCombatGUI.o: ../../include/core/FPoint.h ../../include/core/FHexPath.h
SelectCombatGUI.o: ../../include/core/FHexMap.h
SelectCombatGUI.o: ../../include/tactical/FBattleDisplay.h
SelectJumpGUI.o: ../../include/gui/SelectJumpGUI.h
SelectJumpGUI.o: ../../include/strategic/FMap.h
SelectResolutionGUI.o: ../../include/gui/SelectResolutionGUI.h
SelectResolutionGUI.o: ../../include/strategic/FFleet.h
SelectResolutionGUI.o: ../../include/core/FPObject.h
SelectResolutionGUI.o: ../../include/core/FObject.h
SelectResolutionGUI.o: ../../include/ships/FVehicle.h
SelectResolutionGUI.o: ../../include/core/FGameConfig.h
SelectResolutionGUI.o: ../../include/weapons/FWeapon.h
SelectResolutionGUI.o: ../../include/defenses/FDefense.h
SystemDialogGUI.o: ../../include/gui/SystemDialogGUI.h
SystemDialogGUI.o: ../../include/strategic/FMap.h
SystemDialogGUI.o: ../../include/strategic/FPlayer.h
SystemDialogGUI.o: ../../include/core/FPObject.h ../../include/core/FObject.h
SystemDialogGUI.o: ../../include/strategic/FFleet.h
SystemDialogGUI.o: ../../include/strategic/FMap.h
SystemDialogGUI.o: ../../include/strategic/FSystem.h
SystemDialogGUI.o: ../../include/strategic/FPlanet.h
SystemDialogGUI.o: ../../include/strategic/FAstroBody.h
SystemDialogGUI.o: ../../include/ships/FVehicle.h
SystemDialogGUI.o: ../../include/core/FGameConfig.h
SystemDialogGUI.o: ../../include/weapons/FWeapon.h
SystemDialogGUI.o: ../../include/defenses/FDefense.h
SystemDialogGUI.o: ../../include/strategic/FJumpRoute.h
SystemDialogGUI.o: ../../include/gui/SelectJumpGUI.h
SystemDialogGUI.o: ../../include/gui/ViewFleetGUI.h
SystemDialogGUI.o: ../../include/strategic/FFleet.h
SystemDialogGUI.o: ../../include/strategic/FSystem.h
SystemDialogGUI.o: ../../include/gui/TransferShipsGUI.h
TransferShipsGUI.o: ../../include/gui/TransferShipsGUI.h
TransferShipsGUI.o: ../../include/strategic/FPlayer.h
TransferShipsGUI.o: ../../include/core/FPObject.h
TransferShipsGUI.o: ../../include/core/FObject.h
TransferShipsGUI.o: ../../include/strategic/FFleet.h
TransferShipsGUI.o: ../../include/strategic/FMap.h
TransferShipsGUI.o: ../../include/strategic/FSystem.h
TransferShipsGUI.o: ../../include/strategic/FPlanet.h
TransferShipsGUI.o: ../../include/strategic/FAstroBody.h
TransferShipsGUI.o: ../../include/ships/FVehicle.h
TransferShipsGUI.o: ../../include/core/FGameConfig.h
TransferShipsGUI.o: ../../include/weapons/FWeapon.h
TransferShipsGUI.o: ../../include/defenses/FDefense.h
TransferShipsGUI.o: ../../include/strategic/FJumpRoute.h
TransferShipsGUI.o: ../../include/strategic/FSystem.h
TwoPlanetsGUI.o: ../../include/gui/TwoPlanetsGUI.h
TwoPlanetsGUI.o: ../../include/strategic/FSystem.h
TwoPlanetsGUI.o: ../../include/core/FPObject.h ../../include/core/FObject.h
TwoPlanetsGUI.o: ../../include/strategic/FPlanet.h
TwoPlanetsGUI.o: ../../include/strategic/FAstroBody.h
TwoPlanetsGUI.o: ../../include/ships/FVehicle.h
TwoPlanetsGUI.o: ../../include/core/FGameConfig.h
TwoPlanetsGUI.o: ../../include/weapons/FWeapon.h
TwoPlanetsGUI.o: ../../include/defenses/FDefense.h
TwoPlanetsGUI.o: ../../include/strategic/FFleet.h
UPFUnattachedGUI.o: ../../include/gui/UPFUnattachedGUI.h
UPFUnattachedGUI.o: ../../include/strategic/FPlayer.h
UPFUnattachedGUI.o: ../../include/core/FPObject.h
UPFUnattachedGUI.o: ../../include/core/FObject.h
UPFUnattachedGUI.o: ../../include/strategic/FFleet.h
UPFUnattachedGUI.o: ../../include/strategic/FMap.h
UPFUnattachedGUI.o: ../../include/strategic/FSystem.h
UPFUnattachedGUI.o: ../../include/strategic/FPlanet.h
UPFUnattachedGUI.o: ../../include/strategic/FAstroBody.h
UPFUnattachedGUI.o: ../../include/ships/FVehicle.h
UPFUnattachedGUI.o: ../../include/core/FGameConfig.h
UPFUnattachedGUI.o: ../../include/weapons/FWeapon.h
UPFUnattachedGUI.o: ../../include/defenses/FDefense.h
UPFUnattachedGUI.o: ../../include/strategic/FJumpRoute.h
UPFUnattachedGUI.o: ../../include/strategic/FMap.h
ViewFleetGUI.o: ../../include/gui/ViewFleetGUI.h
ViewFleetGUI.o: ../../include/strategic/FFleet.h
ViewFleetGUI.o: ../../include/core/FPObject.h ../../include/core/FObject.h
ViewFleetGUI.o: ../../include/ships/FVehicle.h
ViewFleetGUI.o: ../../include/core/FGameConfig.h
ViewFleetGUI.o: ../../include/weapons/FWeapon.h
ViewFleetGUI.o: ../../include/defenses/FDefense.h
ViewFleetGUI.o: ../../include/strategic/FSystem.h
ViewFleetGUI.o: ../../include/strategic/FPlanet.h
ViewFleetGUI.o: ../../include/strategic/FAstroBody.h
ViewFleetGUI.o: ../../include/strategic/FFleet.h
WXGameDisplay.o: ../../include/gui/WXGameDisplay.h
WXGameDisplay.o: ../../include/gui/WXMapDisplay.h
WXGameDisplay.o: ../../include/gui/WXPlayerDisplay.h
WXGameDisplay.o: ../../include/strategic/FPlayer.h
WXGameDisplay.o: ../../include/core/FPObject.h ../../include/core/FObject.h
WXGameDisplay.o: ../../include/strategic/FFleet.h
WXGameDisplay.o: ../../include/strategic/FMap.h
WXGameDisplay.o: ../../include/strategic/FSystem.h
WXGameDisplay.o: ../../include/strategic/FPlanet.h
WXGameDisplay.o: ../../include/strategic/FAstroBody.h
WXGameDisplay.o: ../../include/ships/FVehicle.h
WXGameDisplay.o: ../../include/core/FGameConfig.h
WXGameDisplay.o: ../../include/weapons/FWeapon.h
WXGameDisplay.o: ../../include/defenses/FDefense.h
WXGameDisplay.o: ../../include/strategic/FJumpRoute.h
WXGameDisplay.o: ../../include/strategic/FGame.h
WXGameDisplay.o: ../../include/strategic/FPlayer.h
WXMapDisplay.o: ../../include/gui/WXMapDisplay.h
WXMapDisplay.o: ../../include/strategic/FMap.h
WXPlayerDisplay.o: ../../include/gui/WXPlayerDisplay.h
WXPlayerDisplay.o: ../../include/strategic/FPlayer.h
WXPlayerDisplay.o: ../../include/core/FPObject.h ../../include/core/FObject.h
WXPlayerDisplay.o: ../../include/strategic/FFleet.h
WXPlayerDisplay.o: ../../include/strategic/FMap.h
WXPlayerDisplay.o: ../../include/strategic/FSystem.h
WXPlayerDisplay.o: ../../include/strategic/FPlanet.h
WXPlayerDisplay.o: ../../include/strategic/FAstroBody.h
WXPlayerDisplay.o: ../../include/ships/FVehicle.h
WXPlayerDisplay.o: ../../include/core/FGameConfig.h
WXPlayerDisplay.o: ../../include/weapons/FWeapon.h
WXPlayerDisplay.o: ../../include/defenses/FDefense.h
WXPlayerDisplay.o: ../../include/strategic/FJumpRoute.h
WXPlayerDisplay.o: ../../include/gui/WXMapDisplay.h
