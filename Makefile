all: src/apps/cainteoir/cainteoir

src/apps/cainteoir/cainteoir: src/apps/cainteoir/main.cpp
	g++ -Isrc/include -o $@ $<

