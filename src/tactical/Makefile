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

OBJS = FBattleBoard.o FBattleScreen.o FBattleDisplay.o

TARGET = libtactical.a

all $(TARGET): $(OBJS)
	ar rcsu $(TARGET) $(OBJS)

#	$(CXX) $(CXXFLAGS) -shared -o $(TARGET) $(OBJS)# $(LIBS)
	
objs: $(OBJS)

.PHONY:
clean:
	rm $(OBJS)
	rm $(TARGET)


# DO NOT DELETE

FBattleBoard.o: ../../include/Frontier.h ../../include/wxWidgets.h
FBattleBoard.o: ../../include/tactical/FBattleScreen.h
FBattleBoard.o: ../../include/tactical/FBattleBoard.h
FBattleBoard.o: ../../include/strategic/FFleet.h
FBattleBoard.o: ../../include/core/FPObject.h ../../include/core/FObject.h
FBattleBoard.o: ../../include/ships/FVehicle.h
FBattleBoard.o: ../../include/weapons/FWeapon.h
FBattleBoard.o: ../../include/tactical/FBattleDisplay.h
FBattleDisplay.o: ../../include/Frontier.h ../../include/wxWidgets.h
FBattleDisplay.o: ../../include/tactical/FBattleScreen.h
FBattleDisplay.o: ../../include/tactical/FBattleBoard.h
FBattleDisplay.o: ../../include/strategic/FFleet.h
FBattleDisplay.o: ../../include/core/FPObject.h ../../include/core/FObject.h
FBattleDisplay.o: ../../include/ships/FVehicle.h
FBattleDisplay.o: ../../include/weapons/FWeapon.h
FBattleDisplay.o: ../../include/tactical/FBattleDisplay.h
FBattleScreen.o: ../../include/tactical/FBattleScreen.h
FBattleScreen.o: ../../include/Frontier.h ../../include/wxWidgets.h
FBattleScreen.o: ../../include/tactical/FBattleBoard.h
FBattleScreen.o: ../../include/strategic/FFleet.h
FBattleScreen.o: ../../include/core/FPObject.h ../../include/core/FObject.h
FBattleScreen.o: ../../include/ships/FVehicle.h
FBattleScreen.o: ../../include/weapons/FWeapon.h
FBattleScreen.o: ../../include/tactical/FBattleDisplay.h
