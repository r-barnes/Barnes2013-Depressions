CC = g++
CFLAGS = -O3 -Wall

default:
	./gen_surface.py noshow
	$(MAKE) --directory=../src/
	mv ../src/priority_flood.exe ./

clean:
	rm -f priority_flood.exe