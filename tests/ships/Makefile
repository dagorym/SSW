include ../../common.mk

CXXFLAGS += `wx-config --cxxflags`
LIBS = `wx-config --libs`

############################# Paths #################################

SRCS = $(filter-out test_template.cpp,$(wildcard *.cpp))
OBJS = $(SRCS:.cpp=.o)
DEPS = $(OBJS:.o=.d)

TARGET = libshipsTests.a

$(OBJS): Makefile ../../common.mk

all $(TARGET): $(COVERAGE_PREP) $(OBJS)
	ar rcs $(TARGET) $(OBJS)

-include $(DEPS)
