#include "utility.h"
#include "data_structures.h"
#include "data_io.h"
#include "priority_flood.hpp"
#include <cstdio>
#include <string>
#include <sys/time.h>
#include <iostream>
using namespace std;

int main(int argc, char **argv){
	if(argc!=3){
		cout<<argv[0]<<" <ALGORITHM> <INPUT DEM>"<<endl;
		cout<<"Algorithms:"<<endl;
    cout<<"\t1: Original Priority Flood"<<endl;
    cout<<"\t2: Improved Priority Flood"<<endl;
    cout<<"\t3: Priority Flood+Epsilon"<<endl;
    cout<<"\t4: Priority Flood+FlowDirs"<<endl;
    cout<<"\t5: Priority Flood+Watershed Labels"<<endl;
		return -1;
	}
  Timer overall,algtimer;

  overall.start();

  char_2d flowdirs;
  int_2d pit_mask;

	float_2d elevations;
	load_ascii_data(argv[2],elevations);

  switch(argv[1][0]){
    case '1':
      algtimer.start();
      original_priority_flood(elevations);
      algtimer.stop();

      output_ascii_data("out-pf-original",elevations,8);
      overall.stop();

      cout<<"Algorithm 1: Original Priority-Flood took ";
      cout<<algtimer.accumulated()<<"s to run."<<endl;
      cout<<"Overall run-time: "<<overall.accumulated()<<endl;
      return 0;
    case '2':
      algtimer.start();
      improved_priority_flood(elevations);
      algtimer.stop();

      output_ascii_data("out-pf-improved",elevations,8);
      overall.stop();

      cout<<"Algorithm 2: Improved Priority-Flood took ";
      cout<<algtimer.accumulated()<<"s to run."<<endl;
      cout<<"Overall run-time: "<<overall.accumulated()<<endl;
      return 0;
    case '3':
      algtimer.start();
      priority_flood_epsilon(elevations);
      algtimer.stop();

      output_ascii_data("out-pf-epsilon",elevations,8);
      overall.stop();

      cout<<"Algorithm 3: Priority-Flood+Epsilon took ";
      cout<<algtimer.accumulated()<<"s to run."<<endl;
      cout<<"Overall run-time: "<<overall.accumulated()<<endl;
      return 0;
    case '4':
      algtimer.start();
      priority_flood_flowdirs(elevations,flowdirs);
      algtimer.stop();

      output_ascii_data("out-pf-flowdirs",flowdirs,0);
      overall.stop();

      cout<<"Algorithm 4: Priority-Flood+FlowDirs took ";
      cout<<algtimer.accumulated()<<"s to run."<<endl;
      cout<<"Overall run-time: "<<overall.accumulated()<<endl;
      return 0;
    case '5':
      algtimer.start();
      priority_flood_watersheds(elevations,pit_mask,false);
      algtimer.stop();

      output_ascii_data("out-pf-wlabels",pit_mask,0);
      overall.stop();

      cout<<"Algorithm 5: Priority-Flood+Watershed Labels took ";
      cout<<algtimer.accumulated()<<"s to run."<<endl;
      cout<<"Overall run-time: "<<overall.accumulated()<<endl;
      return 0;
  }

	return 0;
}
