EXAMPLES       = pr01_open.cpp pr02_HwInfo.cpp pr03_scanning.cpp pr04_analyze.cpp pr05_doctype.cpp pr06_ecard.cpp
TARGETS        = $(EXAMPLES:%.cpp=%.out)
INCL           = -I/usr/include/gx
CFLAGS         = -DLINUX -Wall
LKLIB          = -ldl

.PHONY: all
all: $(TARGETS)
	@echo Done.

.PHONY: clean
clean:
	rm -f $(TARGETS)

%.out : %.cpp
	g++ $(INCL) $(CFLAGS) -o $@ $< $(LKLIB)
