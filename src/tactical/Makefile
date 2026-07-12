include ../../common.mk

# Approved exception (see AGENTS.md module-boundary carve-out): src/tactical
# still has active wx/GTK render-adjacent code (FBattleBoard/FBattleDisplay/
# FBattleScreen), so it needs wx+gtk include flags. Derive them from the
# shared, wx-config-based WX_CXXFLAGS instead of a machine-specific
# hardcoded path.
CXXFLAGS += $(WX_CXXFLAGS)

############################# Paths #################################

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)
DEPS = $(OBJS:.o=.d)

TARGET = libtactical.a

$(OBJS): Makefile ../../common.mk

all $(TARGET): $(COVERAGE_PREP) $(OBJS)
	ar rcs $(TARGET) $(OBJS)

-include $(DEPS)
