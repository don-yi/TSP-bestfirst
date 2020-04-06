PRG=gcc0.exe

GCC=g++
GCCFLAGS=-O2 -Wall -Wextra -std=c++11 -pedantic -Weffc++ -Wold-style-cast -Woverloaded-virtual -Wsign-promo  -Wctor-dtor-privacy -Wnon-virtual-dtor -Wreorder

VALGRIND_OPTIONS=-q --leak-check=full
DIFFLAGS=--strip-trailing-cr -y --suppress-common-lines

OBJECTS0=tsp.cpp
DRIVER0=driver.cpp

OSTYPE := $(shell uname)
ifeq (,$(findstring CYGWIN,$(OSTYPE)))
CYGWIN=
else
CYGWIN=-Wl,--enable-auto-import
endif

gcc0:
	$(GCC) -o $(PRG) $(CYGWIN) $(DRIVER0) $(OBJECTS0) $(GCCFLAGS)
0 1 2 3 4 5 6 7 8:
	@echo "running test$@"
	watchdog 200 ./$(PRG) $@ >studentout$@
	diff out$@ studentout$@ $(DIFFLAGS) > difference$@
9 10:
	@echo "running test$@"
	watchdog 600 ./$(PRG) $@ >studentout$@
	diff out$@ studentout$@ $(DIFFLAGS) > difference$@
mem6:
	@echo "running memory test $@"
	watchdog 2000 valgrind $(VALGRIND_OPTIONS) ./$(PRG) $(subst mem,,$@) 1>/dev/null 2>difference$@
clean:
	rm -f *.exe *.o difference*
