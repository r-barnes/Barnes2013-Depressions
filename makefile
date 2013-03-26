CC = g++
CFLAGS = -O3

default:
	$(MAKE) --directory=src/
	mv src/priority_flood.exe ./
