CC = g++
GDAL_LIBS = `gdal-config --libs`
GDAL_FLAGS = `gdal-config --cflags`
RICHDEM_COMPILE_TIME=`date -u +'%Y-%m-%d %H:%M:%S UTC'`
RICHDEM_FLAGS = -Irichdem/include -DRICHDEM_COMPILE_TIME="\"$(RICHDEM_COMPILE_TIME)\""
CFLAGS = -g -O3 -Wall --std=c++11 $(GDAL_FLAGS) $(RICHDEM_FLAGS)
LIBS = $(GDAL_LIBS)

default:
	$(CC) $(CFLAGS) -o priority_flood.exe main.cpp $(GDAL_LIBS)

clean:
	rm -f *exe
