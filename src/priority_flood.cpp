#include "utility.h"
#include "data_structures.h"
#include "data_io.h"
#include "pit_fill.h"
#include <cstdio>
#include <string>
#include <sys/time.h>

int main(int argc, char **argv){
	if(argc!=2){
		printf("%s <INPUT DEM>\n",argv[0]);
		return -1;
	}
  Timer overall,flat_resolution_timer;
  overall.start();

	float_2d elevations;

	load_ascii_data(argv[1],elevations);

	char_2d flowdirs;
	d8_flow_directions(elevations,flowdirs);

  flat_resolution_timer.start();
	int_2d flat_resolution_mask, labels;
	resolve_flats_barnes(elevations,flowdirs,flat_resolution_mask,labels);
  flat_resolution_timer.stop();

  //If you want to leave the DEM unaltered, use this command
	d8_flow_flats(flat_resolution_mask,labels,flowdirs);

  //If you want to alter the DEM to enforce drainage, use these commands instead
/*
  flowdirs.init(-4);
  d8_flats_alter_dem(flat_resolution_mask, labels, elevations);
  d8_flow_directions(elevations,flowdirs);
*/

	output_ascii_data("out_barnes",flowdirs,0);
  write_arrows("out_barnes_arrows",flowdirs);

  overall.stop();
  printf("Barnes algorithm took %f seconds overall.\n",overall.accumulated());
  printf("%f seconds was used to resolve flats.\n",flat_resolution_timer.accumulated());

	return 0;
}
