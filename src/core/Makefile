#CXXFLAGS = -I../include -I/usr/local/lib/wx/include/gtk2-ansi-release-2.8 \
#            -I/usr/local/include/wx-2.8 \
#            -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D__WXGTK__ -pthread -g 

#LIBS = -L/usr/local/lib -pthread -lwx_gtk2_richtext-2.8 -lwx_gtk2_aui-2.8 -lwx_gtk2_xrc-2.8 \
#       -lwx_gtk2_qa-2.8 -lwx_gtk2_html-2.8 -lwx_gtk2_adv-2.8 -lwx_gtk2_core-2.8 \
#       -lwx_base_xml-2.8 -lwx_base_net-2.8 -lwx_base-2.8

CXXFLAGS = -I../../include -g `wx-config-3.0 --cxxflags` -Wall -Woverloaded-virtual -DLINUX -fprofile-arcs -ftest-coverage

LIBS = `wx-config-3.0 --libs`

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

OBJS = FObject.o FPObject.o FPoint.o FGameConfig.o FHexMap.o FHexPath.o

TARGET = libcore.a

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

FGameConfig.o: ../../include/core/FGameConfig.h ../../include/core/FPObject.h
FGameConfig.o: ../../include/core/FObject.h
FHexMap.o: ../../include/Frontier.h ../../include/wxWidgets.h
FHexMap.o: ../../include/core/FHexMap.h ../../include/core/FPObject.h
FHexMap.o: ../../include/core/FObject.h ../../include/core/FPoint.h
FHexMap.o: ../../include/ships/FVehicle.h ../../include/core/FGameConfig.h
FHexMap.o: ../../include/weapons/FWeapon.h ../../include/defenses/FDefense.h
FHexPath.o: ../../include/core/FHexPath.h ../../include/core/FPObject.h
FHexPath.o: ../../include/core/FObject.h ../../include/core/FPoint.h
FHexPath.o: ../../include/core/FHexMap.h ../../include/ships/FVehicle.h
FHexPath.o: ../../include/core/FGameConfig.h ../../include/weapons/FWeapon.h
FHexPath.o: ../../include/defenses/FDefense.h
FObject.o: ../../include/core/FObject.h
FPObject.o: ../../include/core/FPObject.h ../../include/core/FObject.h
FPoint.o: ../../include/core/FPoint.h
