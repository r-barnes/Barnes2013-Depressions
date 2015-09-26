#include "utility.h"
#include "data_structures.h"
#include "priority_flood.hpp"
#include "Array2D.hpp"
#include <string>
#include <sys/time.h>
#include <iostream>
#include <cstdint>
using namespace std;

template<class elev_t>
int PerformAlgorithm(char alg, char *filename){
  Timer overall,algtimer;

  overall.start();


  Array2D<int32_t> pit_mask;
  Array2D<elev_t>  elevations(filename,0,0,0,0);
  Array2D<int8_t>  flowdirs;

  switch(alg){
    case '1':
      algtimer.start();
      original_priority_flood(elevations);
      algtimer.stop();

      elevations.saveGDAL("out-pf-original.tif",filename,0,0);
      overall.stop();

      cout<<"Algorithm 1: Original Priority-Flood took ";
      cout<<algtimer.accumulated()<<"s to run."<<endl;
      cout<<"Overall run-time: "<<overall.accumulated()<<endl;
      return 0;
    case '2':
      algtimer.start();
      improved_priority_flood(elevations);
      algtimer.stop();

      elevations.saveGDAL("out-pf-improved.tif",filename,0,0);
      overall.stop();

      cout<<"Algorithm 2: Improved Priority-Flood took ";
      cout<<algtimer.accumulated()<<"s to run."<<endl;
      cout<<"Overall run-time: "<<overall.accumulated()<<endl;
      return 0;
    case '3':
      algtimer.start();
      priority_flood_epsilon(elevations);
      algtimer.stop();

      elevations.saveGDAL("out-pf-epsilon.tif",filename,0,0);
      overall.stop();

      cout<<"Algorithm 3: Priority-Flood+Epsilon took ";
      cout<<algtimer.accumulated()<<"s to run."<<endl;
      cout<<"Overall run-time: "<<overall.accumulated()<<endl;
      return 0;
    case '4':
      algtimer.start();
      priority_flood_flowdirs(elevations,flowdirs);
      algtimer.stop();

      flowdirs.saveGDAL("out-pf-flowdirs.tif",filename,0,0);
      overall.stop();

      cout<<"Algorithm 4: Priority-Flood+FlowDirs took ";
      cout<<algtimer.accumulated()<<"s to run."<<endl;
      cout<<"Overall run-time: "<<overall.accumulated()<<endl;
      return 0;
    case '5':
      algtimer.start();
      priority_flood_watersheds(elevations,pit_mask,false);
      algtimer.stop();

      pit_mask.saveGDAL("out-pf-wlabels.tif",filename,0,0);
      overall.stop();

      cout<<"Algorithm 5: Priority-Flood+Watershed Labels took ";
      cout<<algtimer.accumulated()<<"s to run."<<endl;
      cout<<"Overall run-time: "<<overall.accumulated()<<endl;
      return 0;
    default:
      cerr<<"Unrecognised algorithm choice!"<<endl;
      return -1;
  }
}

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

  switch(peekGDALType(argv[2])){
    case GDT_Unknown:
      std::cerr<<"Unrecognised data type: "<<GDALGetDataTypeName(peekGDALType(argv[2]))<<std::endl;
      return -1;
    case GDT_Byte:
      return PerformAlgorithm<uint8_t >(argv[1][0],argv[2]);
    case GDT_UInt16:
      return PerformAlgorithm<uint16_t>(argv[1][0],argv[2]);
    case GDT_Int16:
      return PerformAlgorithm<int16_t >(argv[1][0],argv[2]);
    case GDT_UInt32:
      return PerformAlgorithm<uint32_t>(argv[1][0],argv[2]);
    case GDT_Int32:
      return PerformAlgorithm<int32_t >(argv[1][0],argv[2]);
    case GDT_Float32:
      return PerformAlgorithm<float   >(argv[1][0],argv[2]);
    case GDT_Float64:
      return PerformAlgorithm<double  >(argv[1][0],argv[2]);
    case GDT_CInt16:
    case GDT_CInt32:
    case GDT_CFloat32:
    case GDT_CFloat64:
      std::cerr<<"Complex types are unsupported. Sorry!"<<std::endl;
      return -1;
    default:
      std::cerr<<"Unrecognised data type: "<<GDALGetDataTypeName(peekGDALType(argv[2]))<<std::endl;
      return -1;
  }

	return 0;
}
