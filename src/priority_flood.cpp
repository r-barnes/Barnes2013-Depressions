#include "utility.h"
#include "data_structures.h"
#include "data_io.h"
#include "pit_fill.h"
#include <cstdio>
#include <string>
#include <sys/time.h>

int main(int argc, char **argv){
	if(argc!=3){
		printf("%s <INPUT DEM>\n",argv[0]);
		return -1;
	}
  Timer overall;
  overall.start();

	float_2d elevations;

	load_ascii_data(argv[1],elevations);

	int_2d pmask;
  pit_mask(elevations,pmask);

	output_ascii_data(argv[2],pmask,0);

  overall.stop();
  printf("Barnes algorithm took %f seconds overall.\n",overall.accumulated());

	return 0;
}
