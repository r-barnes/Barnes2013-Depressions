CC = g++
CFLAGS = -O3 -Wall

default:
	./gen_surface.py noshow
	$(MAKE) --directory=../src/
	mv ../src/priority_flood.exe ./
	./priority_flood.exe 6 sin-x-cos-y.asc
	./priority_flood.exe 6 sin-x-cos-y-divided.asc divided
	./show_surface.py sin-x-cos-y.asc out-pf-wlabels-awi.tif
	./show_surface.py sin-x-cos-y-divided.asc divided-pf-wlabels-awi.tif

clean:
	rm -f priority_flood.exe