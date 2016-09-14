#include "richdem/common/Array2D.hpp"
#include "richdem/common/timer.hpp"
#include "richdem/depressions/priority_flood.hpp"
#include "richdem/common/version.hpp"
#include "richdem/include/richdem/depressions/Zhou2016pf.hpp"
#include <string>
#include <iostream>
#include <cstdint>
using namespace std;

template<class elev_t>
int PerformAlgorithm(char alg, std::string filename, std::string output_name, std::string analysis){
  Timer overall,algtimer;

  overall.start();

  Array2D<elev_t> elevations(filename,false);
  
  switch(alg){
    case '1':
      algtimer.start();
      original_priority_flood(elevations);
      algtimer.stop();

      elevations.saveGDAL(output_name,analysis);
      overall.stop();

      cout<<"t Run-time = "<<algtimer.accumulated()<<" s"<<endl;
      cout<<"t Wall-time = "<<overall.accumulated()<<" s"<<endl;
      return 0;


    case '2':
      algtimer.start();
      improved_priority_flood(elevations);
      algtimer.stop();

      elevations.saveGDAL(output_name,analysis);
      overall.stop();

      cout<<"t Run-time = "<<algtimer.accumulated()<<" s"<<endl;
      cout<<"t Wall-time = "<<overall.accumulated()<<" s"<<endl;
      return 0;


    case '3':
      algtimer.start();
      priority_flood_epsilon(elevations);
      algtimer.stop();

      elevations.saveGDAL(output_name,analysis);
      overall.stop();

      cout<<"t Run-time = "<<algtimer.accumulated()<<" s"<<endl;
      cout<<"t Wall-time = "<<overall.accumulated()<<" s"<<endl;
      return 0;


    case '4': {
      Array2D<d8_flowdir_t>  flowdirs;
      algtimer.start();
      priority_flood_flowdirs(elevations,flowdirs);
      algtimer.stop();

      flowdirs.saveGDAL(output_name,analysis);
      overall.stop();

      cout<<"t Run-time = "<<algtimer.accumulated()<<" s"<<endl;
      cout<<"t Wall-time = "<<overall.accumulated()<<" s"<<endl;
      return 0;
    }


    case '5': {
      Array2D<int32_t> pit_mask;
      algtimer.start();
      priority_flood_watersheds(elevations,pit_mask,false);
      algtimer.stop();

      pit_mask.saveGDAL(output_name,analysis);
      overall.stop();

      cout<<"t Run-time = "<<algtimer.accumulated()<<" s"<<endl;
      cout<<"t Wall-time = "<<overall.accumulated()<<" s"<<endl;
      return 0;
    }

    case '6':
      algtimer.start();
      Zhou2016(elevations);
      algtimer.stop();

      elevations.saveGDAL(output_name,analysis);
      overall.stop();

      cout<<"t Run-time = "<<algtimer.accumulated()<<" s"<<endl;
      cout<<"t Wall-time = "<<overall.accumulated()<<" s"<<endl;
      return 0;


    default:
      cerr<<"Unrecognised algorithm choice!"<<endl;
      return -1;
  }
}

template< typename... Arguments >
int Router(std::string inputfile, Arguments ... args){
  switch(peekGDALType(inputfile)){
    case GDT_Byte:
      return PerformAlgorithm<uint8_t >(args...);
    case GDT_UInt16:
      return PerformAlgorithm<uint16_t>(args...);
    case GDT_Int16:
      return PerformAlgorithm<int16_t >(args...);
    case GDT_UInt32:
      return PerformAlgorithm<uint32_t>(args...);
    case GDT_Int32:
      return PerformAlgorithm<int32_t >(args...);
    case GDT_Float32:
      return PerformAlgorithm<float   >(args...);
    case GDT_Float64:
      return PerformAlgorithm<double  >(args...);
    case GDT_CInt16:
    case GDT_CInt32:
    case GDT_CFloat32:
    case GDT_CFloat64:
      std::cerr<<"Complex types are unsupported. Sorry!"<<std::endl;
      return -1;
    case GDT_Unknown:
    default:
      std::cerr<<"Unrecognised data type: "<<GDALGetDataTypeName(peekGDALType(inputfile))<<std::endl;
      return -1;
  }
}

int main(int argc, char **argv){
  std::string analysis = PrintRichdemHeader(argc, argv);
  
  if(argc!=4){
    cout<<argv[0]<<" <ALGORITHM> <INPUT DEM> <OUTPUT NAME>"<<endl;
    cout<<"Algorithms:"<<endl;
    cout<<"\t1: Original Priority Flood"<<endl;
    cout<<"\t2: Improved Priority Flood"<<endl;
    cout<<"\t3: Priority Flood+Epsilon"<<endl;
    cout<<"\t4: Priority Flood+FlowDirs"<<endl;
    cout<<"\t5: Priority Flood+Watershed Labels"<<endl;
    cout<<"\t6: Zhou 2016 Priority-Flood"<<endl;
    return -1;
  }

  std::string inputname  = argv[2];
  std::string outputname = argv[3];

  Router(inputname,argv[1][0],inputname,outputname,analysis);

  return 0;
}
