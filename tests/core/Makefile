include ../../common.mk

CXXFLAGS += `wx-config --cxxflags` -DCORE_TEST_REPO_ROOT=\"$(CURDIR)/../..\"
LIBS = `wx-config --libs` -lcppunit

############################# Paths #################################

SRCS = $(filter-out test_template.cpp,$(wildcard *.cpp))
OBJS = $(SRCS:.cpp=.o)
DEPS = $(OBJS:.o=.d)

TARGET = libcoreTests.a

$(OBJS): Makefile ../../common.mk

all $(TARGET): $(COVERAGE_PREP) $(OBJS)
	ar rcs $(TARGET) $(OBJS)

-include $(DEPS)
