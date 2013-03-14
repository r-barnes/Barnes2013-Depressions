/**
  @file
  Header and templated code for reading and writing data, primarily in the
  ArcGrid ASCII format.

  Richard Barnes (rbarnes@umn.edu), 2012
*/
#ifndef _data_io_included
#define _data_io_included

#include "utility.h"
#include <fstream>
#include <string>
#include <stdlib.h>

int load_ascii_data(const char filename[], float_2d &elevations);
int write_arrows(const char filename[], const char_2d &flowdirs);

//load_ascii_data
/**
  @brief  Writes an ArcGrid ASCII file
  @author Richard Barnes

  @param[in]  &filename     Name of ArcGrid ASCII file to write
  @param[in]  &elevations   DEM object to write
  @param[in]  precision     Floating-point precision of output

  @returns 0 upon success
*/
template <class T>
int output_ascii_data(
  const std::string filename,
  const array2d<T> &output_grid,
  int precision=8
){
  std::ofstream fout;
  std::string outputsep=" ";
  Timer write_time;
  ProgressBar progress;

  write_time.start();

  diagnostic_arg("Opening ASCII output file \"%s\"...",filename.c_str());
  fout.open(filename.c_str());
  if(!fout.is_open()){
    diagnostic("failed!\n");
    exit(-1);  //TODO: Need to make this safer! Don't just close after all that work!
  }
  diagnostic("succeeded.\n");

  diagnostic("Writing ArcGrid ASCII file header...");
  fout<<"ncols\t\t"<<output_grid.width()<<std::endl;
  fout<<"nrows\t\t"<<output_grid.height()<<std::endl;
  fout<<"xllcorner\t"<<std::fixed<<std::setprecision(precision)<<output_grid.xllcorner<<std::endl;
  fout<<"yllcorner\t"<<std::fixed<<std::setprecision(precision)<<output_grid.yllcorner<<std::endl;
  fout<<"cellsize\t"<<std::fixed<<std::setprecision(precision)<<output_grid.cellsize<<std::endl;
  fout<<"NODATA_value\t"<<std::fixed<<std::setprecision(precision);
  if(sizeof(T)==1)  //TODO: Crude way of detecting chars and bools
    fout<<(int)output_grid.no_data<<std::endl;
  else
    fout<<output_grid.no_data<<std::endl;
  diagnostic("succeeded.\n");

  diagnostic("%%Writing ArcGrid ASCII file data...\n");
  progress.start( output_grid.width()*output_grid.height() );
  fout.precision(precision);
  fout.setf(std::ios::fixed);
  for(int y=0;y<output_grid.height();y++){
    progress.update( y*output_grid.width() );
    for(int x=0;x<output_grid.width();x++)
      if(sizeof(T)==1)  //TODO: Crude way of detecting chars and bools
        fout<<(int)output_grid(x,y)<<outputsep;
      else
        fout<<output_grid(x,y)<<outputsep;
    fout<<std::endl;
  }
  diagnostic_arg(SUCCEEDED_IN,progress.stop());

  fout.close();

  write_time.stop();
  diagnostic_arg("Write time was: %lf\n", write_time.accumulated());

  return 0;
}

#endif
