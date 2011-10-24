.PHONY : all clean

CFLAGS := -Wall -L/usr/lib -lrt
CPPFLAGS := -Wall -L/usr/lib -lrt -D_GNU_SOURCE=1 -D_REENTRANT
DIRS := $(shell find . -type d)
FILES := $(foreach dir, $(DIRS), $(wildcard $(dir)/*.c) \
		                         $(wildcard $(dir)/*.cpp))
OBJS := $(addsuffix .o, $(basename $(FILES)))
APP := Golay_Binary

all: $(OBJS)
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $(OBJS) -o $(APP)

clean:
	rm -f $(OBJS) $(APP) $(OBJS:.o=.d)

%.d: %.cpp
	$(CXX) $(CPPFLAGS) -MM -MF $@ $<

%.d: %.c
	$(CC) $(CFLAGS) -MM -MF $@ $<

-include $(OBJS:.o=.d)
