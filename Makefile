all: src/apps/cainteoir/cainteoir

LIBCAINTEOIR_src = $(wildcard src/libcainteoir/*.cpp)

CAINTEOIR_src = $(LIBCAINTEOIR_src) src/apps/cainteoir/main.cpp

src/apps/cainteoir/cainteoir: $(CAINTEOIR_src)
	g++ -Isrc/include -o $@ $(CAINTEOIR_src)

