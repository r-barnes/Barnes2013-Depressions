/**
  @file
  Defines timing functions, progress bars, and the D8 neighbourhood.

  Richard Barnes (rbarnes@umn.edu), 2012
*/
#ifndef _utility_included
#define _utility_included

#include <string>
#include <cmath>
#include "data_structures.h"
#include <sys/time.h>
//Neighbour directions
//234
//105
//876

///Value used to indicate that a flow direction cell has no data
#define d8_NO_DATA    -100
///Value used to indicate that a cell does not have a defined flow direction
#define NO_FLOW       -1
///sqrt(2), used to generate distances from a central cell to its neighbours
#define SQRT2         1.414213562373095048801688724209698078569671875376948

///x offsets of D8 neighbours, from a central cell
const int dx[9]={0,-1,-1,0,1,1,1,0,-1};
///y offsets of D8 neighbours, from a central cell
const int dy[9]={0,0,-1,-1,-1,0,1,1,1};
///Arrows indicating flow directions
const wchar_t fd[9]={L'·',L'←',L'↖',L'↑',L'↗',L'→',L'↘',L'↓',L'↙'};
///Distances from a central cell to each of its 8 neighbours
const double dr[9]={0,1,SQRT2,1,SQRT2,1,SQRT2,1,SQRT2};
///For each D8 direction above, this is the opposite direction
const int inverse_flow[9]={0,5,6,7,8,1,2,3,4}; //Inverse of a given n from chart below

class Timer{
  private:
    timeval start_time; ///<Last time the timer was started
    double accumulated_time; ///<Accumulated running time since creation
    bool running; ///<True when the timer is running
    ///Number of seconds between two time objects
    double timediff(timeval beginning, timeval end){
      long seconds, useconds;
      seconds  = end.tv_sec  - beginning.tv_sec;
      useconds = end.tv_usec - beginning.tv_usec;
      return seconds + useconds/1000000.0;
    }
  public:
    Timer(){
      accumulated_time=0;
      running=false;
    }
    void start(){
      if(running)
        throw "Timer was already started!";
      running=true;
      gettimeofday(&start_time, NULL);
    }
    void stop(){
      if(!running)
        throw "Timer was already stopped!";
      running=false;
      timeval end_time;
      gettimeofday(&end_time, NULL);

      accumulated_time+=timediff(start_time,end_time);
    }
    double accumulated(){
      if(running)
        throw "Timer is still running!";
      return accumulated_time;
    }
    double lap(){
      if(!running)
        throw "Timer was not started!";
      timeval lap_time;
      gettimeofday(&lap_time, NULL);
      return timediff(start_time,lap_time);
    }
};

#ifdef _OPENMP
  #include <omp.h>
#else
  #define omp_get_thread_num()  0
  #define omp_get_num_threads() 1
#endif

#define PROGRESS_BAR "=================================================="

class ProgressBar{
  private:
    long total_work;
    int old_percent;
    Timer timer;
  public:
    void start(long total_work0){
      timer.start();
      total_work=total_work0;
      old_percent=0;
      //This ANSI code clears the entire line
      fprintf(stderr,"\r\033[2K");
    }
    void update(long work_done){
      if(omp_get_thread_num()!=0)
        return;

      int percent;
      percent=(int)(work_done*omp_get_num_threads()*100/total_work);
      if(percent>100)
        percent=100;
      if(percent==old_percent)
        return;
      old_percent=percent;

      fprintf(stderr,"\r\033[2K[%-50.*s] (%d%% - %.1lfs left - %d thread)", percent/2, PROGRESS_BAR, percent, timer.lap()/percent*(100-percent),omp_get_num_threads());
    }
    double stop(){
      //This ANSI code clears the entire line
      fprintf(stderr,"\r\033[2K");

      timer.stop();
      return timer.accumulated();
    }
    double time_it_took(){
      return timer.accumulated();
    }
};

#endif
