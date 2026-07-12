include ../../common.mk

############################# Paths #################################

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)
DEPS = $(OBJS:.o=.d)

TARGET = libdefenses.a

$(OBJS): Makefile ../../common.mk

all $(TARGET): $(COVERAGE_PREP) $(OBJS)
	ar rcs $(TARGET) $(OBJS)

-include $(DEPS)
