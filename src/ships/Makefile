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

OBJS = FVehicle.o \
       FAssaultScout.o FAssaultCarrier.o FBattleship.o FDestroyer.o FFighter.o \
       FFrigate.o FHvCruiser.o FLtCruiser.o FMinelayer.o \
       FFortress.o FArmedStation.o \
       FFortifiedStation.o 
       
TARGET = libships.a

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

FArmedStation.o: ../../include/ships/FArmedStation.h
FArmedStation.o: ../../include/ships/FVehicle.h ../../include/core/FPObject.h
FArmedStation.o: ../../include/core/FObject.h
FArmedStation.o: ../../include/core/FGameConfig.h
FArmedStation.o: ../../include/weapons/FWeapon.h
FArmedStation.o: ../../include/defenses/FDefense.h
FAssaultCarrier.o: ../../include/ships/FAssaultCarrier.h
FAssaultCarrier.o: ../../include/ships/FVehicle.h
FAssaultCarrier.o: ../../include/core/FPObject.h ../../include/core/FObject.h
FAssaultCarrier.o: ../../include/core/FGameConfig.h
FAssaultCarrier.o: ../../include/weapons/FWeapon.h
FAssaultCarrier.o: ../../include/defenses/FDefense.h
FAssaultScout.o: ../../include/ships/FAssaultScout.h
FAssaultScout.o: ../../include/ships/FVehicle.h ../../include/core/FPObject.h
FAssaultScout.o: ../../include/core/FObject.h
FAssaultScout.o: ../../include/core/FGameConfig.h
FAssaultScout.o: ../../include/weapons/FWeapon.h
FAssaultScout.o: ../../include/defenses/FDefense.h
FBattleship.o: ../../include/ships/FBattleship.h
FBattleship.o: ../../include/ships/FVehicle.h ../../include/core/FPObject.h
FBattleship.o: ../../include/core/FObject.h ../../include/core/FGameConfig.h
FBattleship.o: ../../include/weapons/FWeapon.h
FBattleship.o: ../../include/defenses/FDefense.h
FDestroyer.o: ../../include/ships/FDestroyer.h ../../include/ships/FVehicle.h
FDestroyer.o: ../../include/core/FPObject.h ../../include/core/FObject.h
FDestroyer.o: ../../include/core/FGameConfig.h
FDestroyer.o: ../../include/weapons/FWeapon.h
FDestroyer.o: ../../include/defenses/FDefense.h
FFighter.o: ../../include/ships/FFighter.h ../../include/ships/FVehicle.h
FFighter.o: ../../include/core/FPObject.h ../../include/core/FObject.h
FFighter.o: ../../include/core/FGameConfig.h ../../include/weapons/FWeapon.h
FFighter.o: ../../include/defenses/FDefense.h
FFortifiedStation.o: ../../include/ships/FFortifiedStation.h
FFortifiedStation.o: ../../include/ships/FVehicle.h
FFortifiedStation.o: ../../include/core/FPObject.h
FFortifiedStation.o: ../../include/core/FObject.h
FFortifiedStation.o: ../../include/core/FGameConfig.h
FFortifiedStation.o: ../../include/weapons/FWeapon.h
FFortifiedStation.o: ../../include/defenses/FDefense.h
FFortress.o: ../../include/ships/FFortress.h ../../include/ships/FVehicle.h
FFortress.o: ../../include/core/FPObject.h ../../include/core/FObject.h
FFortress.o: ../../include/core/FGameConfig.h ../../include/weapons/FWeapon.h
FFortress.o: ../../include/defenses/FDefense.h
FFrigate.o: ../../include/ships/FFrigate.h ../../include/ships/FVehicle.h
FFrigate.o: ../../include/core/FPObject.h ../../include/core/FObject.h
FFrigate.o: ../../include/core/FGameConfig.h ../../include/weapons/FWeapon.h
FFrigate.o: ../../include/defenses/FDefense.h
FHvCruiser.o: ../../include/ships/FHvCruiser.h ../../include/ships/FVehicle.h
FHvCruiser.o: ../../include/core/FPObject.h ../../include/core/FObject.h
FHvCruiser.o: ../../include/core/FGameConfig.h
FHvCruiser.o: ../../include/weapons/FWeapon.h
FHvCruiser.o: ../../include/defenses/FDefense.h
FLtCruiser.o: ../../include/ships/FLtCruiser.h ../../include/ships/FVehicle.h
FLtCruiser.o: ../../include/core/FPObject.h ../../include/core/FObject.h
FLtCruiser.o: ../../include/core/FGameConfig.h
FLtCruiser.o: ../../include/weapons/FWeapon.h
FLtCruiser.o: ../../include/defenses/FDefense.h
FMinelayer.o: ../../include/ships/FMinelayer.h ../../include/ships/FVehicle.h
FMinelayer.o: ../../include/core/FPObject.h ../../include/core/FObject.h
FMinelayer.o: ../../include/core/FGameConfig.h
FMinelayer.o: ../../include/weapons/FWeapon.h
FMinelayer.o: ../../include/defenses/FDefense.h
FVehicle.o: ../../include/ships/FVehicle.h ../../include/core/FPObject.h
FVehicle.o: ../../include/core/FObject.h ../../include/core/FGameConfig.h
FVehicle.o: ../../include/weapons/FWeapon.h ../../include/defenses/FDefense.h
FVehicle.o: ../../include/defenses/FNone.h ../../include/ships/ships.h
FVehicle.o: ../../include/ships/FFighter.h ../../include/ships/FVehicle.h
FVehicle.o: ../../include/ships/FAssaultScout.h
FVehicle.o: ../../include/ships/FFrigate.h ../../include/ships/FDestroyer.h
FVehicle.o: ../../include/ships/FMinelayer.h ../../include/ships/FLtCruiser.h
FVehicle.o: ../../include/ships/FHvCruiser.h
FVehicle.o: ../../include/ships/FAssaultCarrier.h
FVehicle.o: ../../include/ships/FBattleship.h ../../include/ships/FFortress.h
FVehicle.o: ../../include/ships/FFortifiedStation.h
FVehicle.o: ../../include/ships/FArmedStation.h ../../include/Frontier.h
FVehicle.o: ../../include/wxWidgets.h
