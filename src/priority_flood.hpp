#ifndef pit_fill_include
#define pit_fill_include
#include "data_structures.h"
#include <queue>
#include <limits>

//barnes_priority_flood
/**
  @brief  Fills all pits and removes all digital dams from a DEM
  @author Richard Barnes (rbarnes@umn.edu)

    Priority-Flood starts on the edges of the DEM and then works its way
    inwards using a priority queue to determine the lowest cell which has
    a path to the edge. The neighbours of this cell are added to the priority
    queue if they are higher. If they are lower, they are added to a "pit"
    queue which is used to flood pits. Cells which are higher than a pit being
    filled are added to the priority queue. In this way, pits are filled
    without incurring the expense of the priority queue.

  @param[in,out]  &elevations   A grid of cell elevations
*/
template <class T>
void barnes_priority_flood(array2d<T> &elevations){
  grid_cellz_pq open;
  std::queue<grid_cellz> pit;
  bool_2d closed;
  unsigned long processed_cells=0;
  unsigned long pitc=0;
  ProgressBar progress;

  diagnostic("\n###Barnes Flood\n");
  diagnostic("Setting up boolean flood array matrix...");
  closed.copyprops(elevations);
  closed.init(false);
  diagnostic("succeeded.\n");

  diagnostic_arg("The priority queue will require approximately %ldMB of RAM.\n",(elevations.width()*2+elevations.height()*2)*((long)sizeof(grid_cellz))/1024/1024);
  diagnostic("Adding cells to the priority queue...");
  for(int x=0;x<elevations.width();x++){
    open.push_cell(x,0,elevations(x,0) );
    open.push_cell(x,elevations.height()-1,elevations(x,elevations.height()-1) );
    closed(x,0)=true;
    closed(x,elevations.height()-1)=true;
  }
  for(int y=1;y<elevations.height()-1;y++){
    open.push_cell(0,y,elevations(0,y)  );
    open.push_cell(elevations.width()-1,y,elevations(elevations.width()-1,y) );
    closed(0,y)=true;
    closed(elevations.width()-1,y)=true;
  }
  diagnostic("succeeded.\n");

  diagnostic("%%Performing the Barnes Flood...\n");
  progress.start( elevations.width()*elevations.height() );
  while(open.size()>0 || pit.size()>0){
    grid_cellz c;
    if(pit.size()>0){
      c=pit.front();
      pit.pop();
    } else {
      c=open.top();
      open.pop();
    }
    processed_cells++;

    for(int n=1;n<=8;n++){
      int nx=c.x+dx[n];
      int ny=c.y+dy[n];
      if(!elevations.in_grid(nx,ny)) continue;
      if(closed(nx,ny)) 
        continue;

      closed(nx,ny)=true;
      if(elevations(nx,ny)<=c.z){
        if(elevations(nx,ny)<c.z){
          ++pitc;
          elevations(nx,ny)=c.z;
        }
        pit.push(grid_cellz(nx,ny,c.z));
      } else
        open.push_cell(nx,ny,elevations(nx,ny));
    }
    progress.update(processed_cells);
  }
  diagnostic_arg(SUCCEEDED_IN,progress.stop());
  diagnostic_arg("%ld cells processed. %ld in pits.\n",processed_cells,pitc);
}


//priority_flood_epsilon
/**
  @brief  Assigns every cell an elevation which guarantees drainage
  @author Richard Barnes (rbarnes@umn.edu)

    This version of Priority-Flood starts on the edges of the DEM and then
    works its way inwards using a priority queue to determine the lowest cell
    which has a path to the edge. The neighbours of this cell are added to the
    priority queue if they are higher. If they are lower, then their elevation
    is increased to ensure that they have a drainage path and they are added to
    a "pit" queue which is used to flood pits. Cells which are higher than a
    pit being filled are added to the priority queue. In this way, pits are
    filled without incurring the expense of the priority queue.

  @param[in,out]  &elevations   A grid of cell elevations
*/
template <class T>
void priority_flood_epsilon(array2d<T> &elevations){
  grid_cellz_pq open;
  std::queue<grid_cellz> pit;
  bool_2d closed;
  unsigned long processed_cells=0;
  unsigned long pitc=0;
  ProgressBar progress;

  diagnostic("\n###Barnes Flood\n");
  diagnostic("Setting up boolean flood array matrix...");
  closed.copyprops(elevations);
  closed.init(false);
  diagnostic("succeeded.\n");

  diagnostic_arg("The priority queue will require approximately %ldMB of RAM.\n",(elevations.width()*2+elevations.height()*2)*((long)sizeof(grid_cellz))/1024/1024);
  diagnostic("Adding cells to the priority queue...");
  for(int x=0;x<elevations.width();x++){
    open.push_cell(x,0,elevations(x,0) );
    open.push_cell(x,elevations.height()-1,elevations(x,elevations.height()-1) );
    closed(x,0)=true;
    closed(x,elevations.height()-1)=true;
  }
  for(int y=1;y<elevations.height()-1;y++){
    open.push_cell(0,y,elevations(0,y)  );
    open.push_cell(elevations.width()-1,y,elevations(elevations.width()-1,y) );
    closed(0,y)=true;
    closed(elevations.width()-1,y)=true;
  }
  diagnostic("succeeded.\n");

  diagnostic("%%Performing the Barnes Flood...\n");
  progress.start( elevations.width()*elevations.height() );
  while(open.size()>0 || pit.size()>0){
    grid_cellz c;
    if(pit.size()>0 && open.size()>0 && open.top().z==pit.front().z){
      c=open.top();
      open.pop();
    } else if(pit.size()>0){
      c=pit.front();
      pit.pop();
    } else {
      c=open.top();
      open.pop();
    }
    processed_cells++;

    for(int n=1;n<=8;n++){
      int nx=c.x+dx[n];
      int ny=c.y+dy[n];
      if(!elevations.in_grid(nx,ny)) continue;
      if(closed(nx,ny)) 
        continue;

      closed(nx,ny)=true;
      if(elevations(nx,ny)<nextafterf(c.z,std::numeric_limits<float>::infinity())){
        if(elevations(nx,ny)<c.z){
          ++pitc;
          elevations(nx,ny)=nextafterf(c.z,std::numeric_limits<float>::infinity());
        }
        pit.push(grid_cellz(nx,ny,c.z));
      } else
        open.push_cell(nx,ny,elevations(nx,ny));
    }
    progress.update(processed_cells);
  }
  diagnostic_arg(SUCCEEDED_IN,progress.stop());
  diagnostic_arg("%ld cells processed. %ld in pits.\n",processed_cells,pitc);
}





//d8_edge_flow
/**
  @brief  Helper function which returns a flow direction pointing to an edge if (x,y) is a non-no_data edge cell
  @author Richard Barnes (rbarnes@umn.edu)

  @param[in]  x             x-coordinate of the cell
  @param[in]  y             y-coordinate of the cell
  @param[in]  &elevations   A grid of cell elevations
  @param[in]  &flowdirs     A grid of D8 flow directions

  @pre  (x,y) must be an edge cell or an error will be thrown

  @return If (x,y) is a non-no_data edge cell, then a D8 direction is returned which points to the edge
*/
template <class T>
char d8_edge_flow(int x, int y, const array2d<T> &elevations, const char_2d &flowdirs){
  if(!elevations.edge_grid(x,y))  //NOTE: Shouldn't happen
    throw "Barnes Flood+Flow Directions tried to initialize with a non-edge cell!";
  else if(elevations(x,y)==elevations.no_data)
    return flowdirs.no_data;
  else if(x==0 && y==0)
    return 2;
  else if(x==0 && y==elevations.height()-1)
    return 8;
  else if(x==elevations.width()-1 && y==0)
    return 4;
  else if(x==elevations.width()-1 && y==elevations.height()-1)
    return 6;
  else if(x==0)
    return 1;
  else if(x==elevations.width()-1)
    return 5;
  else if(y==0)
    return 3;
  else if(y==elevations.height()-1)
    return 7;
  else  //NOTE: Avoids a compiler warning. Control can now never reach end of a non-void function.
    throw "Barnes Flood+Flow Directions tried to initialize with a non-edge cell!";
}




//barnes_flood_flowdirs
/**
  @brief  Determines D8 flow directions by flooding inwards, pits are implicitly carved to drainage points. Based on Metz 2011.
  @author Richard Barnes (rbarnes@umn.edu)

  @param[in]   &elevations  A grid of cell elevations
  @param[out]  &flowdirs    A grid of D8 flow directions

  @post \pname{flowdirs} takes the properties and dimensions of \pname{elevations}
*/
template <class T>
void barnes_flood_flowdirs(const array2d<T> &elevations, char_2d &flowdirs){
  grid_cellzk_pq open;
  bool_2d closed;
  unsigned long processed_cells=0;
  ProgressBar progress;

  diagnostic("\n###Barnes Flood+Flow Directions\n");
  diagnostic("Setting up boolean flood array matrix...");
  closed.copyprops(elevations);
  closed.init(false);
  diagnostic("succeeded.\n");

  diagnostic("Setting up the flowdirs matrix...");
  flowdirs.copyprops(elevations);
  flowdirs.no_data=NO_FLOW;
  diagnostic("succeeded.\n");

  diagnostic_arg("The priority queue will require approximately %ldMB of RAM.\n",(elevations.width()*2+elevations.height()*2)*((long)sizeof(grid_cellz))/1024/1024);
  diagnostic("Adding cells to the priority queue...");
  for(int x=0;x<elevations.width();x++){
    open.push_cell(x,0,elevations(x,0));
    open.push_cell(x,elevations.height()-1,elevations(x,elevations.height()-1));
    flowdirs(x,0)=d8_edge_flow(x,0,elevations,flowdirs);
    flowdirs(x,elevations.height()-1)=d8_edge_flow(x, elevations.height()-1, elevations, flowdirs);
    closed(x,0)=true;
    closed(x,elevations.height()-1)=true;
  }
  for(int y=1;y<elevations.height()-1;y++){
    open.push_cell(0,y,elevations(0,y) );
    open.push_cell(elevations.width()-1,y,elevations(elevations.width()-1,y) );
    flowdirs(0,y)=d8_edge_flow(0,y,elevations,flowdirs);
    flowdirs(elevations.width()-1,y)=d8_edge_flow(elevations.width()-1, y, elevations, flowdirs);
    closed(0,y)=true;
    closed(elevations.width()-1,y)=true;
  }
  diagnostic("succeeded.\n");

  const int d8_order[9]={0,1,3,5,7,2,4,6,8};
  diagnostic("%%Performing the Barnes Flood+Flow Directions...\n");
  progress.start( elevations.width()*elevations.height() );
  while(open.size()>0){
    grid_cellz c=open.top();
    open.pop();
    processed_cells++;

    for(int no=1;no<=8;no++){
      int n=d8_order[no];
      int nx=c.x+dx[n];
      int ny=c.y+dy[n];
      if(!elevations.in_grid(nx,ny)) continue;
      if(closed(nx,ny)) 
        continue;

      closed(nx,ny)=true;

      if(elevations(nx,ny)==elevations.no_data)
        flowdirs(nx,ny)=flowdirs.no_data;
      else
        flowdirs(nx,ny)=inverse_flow[n];

      open.push_cell(nx,ny,elevations(nx,ny));
    }
    progress.update(processed_cells);
  }
  diagnostic_arg(SUCCEEDED_IN,progress.stop());
  diagnostic_arg("%ld cells processed.\n",processed_cells);
}











//pit_mask
/**
  @brief  TODO
  @author Richard Barnes (rbarnes@umn.edu)

    TODO

  @param[in,out]  &elevations   A grid of cell elevations
*/
template <class T>
void pit_mask(const array2d<T> &elevations, int_2d &pit_mask){
  grid_cellz_pq open;
  std::queue<grid_cellz> pit;
  bool_2d closed;
  unsigned long processed_cells=0;
  unsigned long pitc=0;
  ProgressBar progress;

  diagnostic("\n###Pit Mask\n");
  diagnostic("Setting up boolean flood array matrix...");
  closed.copyprops(elevations);
  closed.init(false);
  diagnostic("succeeded.\n");

  diagnostic("Setting up the pit mask matrix...");
  pit_mask.copyprops(elevations);
  pit_mask.no_data=3;
  diagnostic("succeeded.\n");

  diagnostic_arg("The priority queue will require approximately %ldMB of RAM.\n",(elevations.width()*2+elevations.height()*2)*((long)sizeof(grid_cellz))/1024/1024);
  diagnostic("Adding cells to the priority queue...");
  for(int x=0;x<elevations.width();x++){
    open.push_cell(x,0,elevations(x,0) );
    open.push_cell(x,elevations.height()-1,elevations(x,elevations.height()-1) );
    closed(x,0)=true;
    closed(x,elevations.height()-1)=true;
  }
  for(int y=1;y<elevations.height()-1;y++){
    open.push_cell(0,y,elevations(0,y)  );
    open.push_cell(elevations.width()-1,y,elevations(elevations.width()-1,y) );
    closed(0,y)=true;
    closed(elevations.width()-1,y)=true;
  }
  diagnostic("succeeded.\n");

  diagnostic("%%Performing the pit mask...\n");
  progress.start( elevations.width()*elevations.height() );
  while(open.size()>0 || pit.size()>0){
    grid_cellz c;
    if(pit.size()>0){
      c=pit.front();
      pit.pop();
    } else {
      c=open.top();
      open.pop();
    }
    processed_cells++;

    for(int n=1;n<=8;n++){
      int nx=c.x+dx[n];
      int ny=c.y+dy[n];
      if(!elevations.in_grid(nx,ny)) continue;
      if(closed(nx,ny)) 
        continue;

      closed(nx,ny)=true;
      if(elevations(nx,ny)<=c.z){
        if(elevations(nx,ny)<c.z)
          pit_mask(nx,ny)=1;
        pit.push(grid_cellz(nx,ny,c.z));
      } else{
        pit_mask(nx,ny)=0;
        open.push_cell(nx,ny,elevations(nx,ny));
      }
    }

    if(elevations(c.x,c.y)==elevations.no_data)
      pit_mask(c.x,c.y)=pit_mask.no_data;

    progress.update(processed_cells);
  }
  diagnostic_arg(SUCCEEDED_IN,progress.stop());
  diagnostic_arg("%ld cells processed. %ld in pits.\n",processed_cells,pitc);
}



//find_watersheds
/**
  @brief  Labels watershed drainage areas, working inwards from the edges of the DEM
  @author Richard Barnes (rbarnes@umn.edu)

  Same as #barnes_flood. \pname{labels} starts out as no_data. If it is found
  that a no_data labels cell coincides with a data_cell in \pname{elevations},
  then this is the beginning of a new watershed. Cells which are flooded from
  a labeled cell take on that cell's label

  @param[in,out] elevations        A grid of cell elevations
  @param[out]    labels            A grid to hold the watershed labels
  @param[in]     alter_elevations
    If true, then \pname{elevations} is altered as though barnes_flood() had
    been applied. The result is that all cells drain to the edges of the DEM. 
    Otherwise, \pname{elevations} is not altered.

  @post \pname{labels} takes the properties and dimensions of \pname{elevations}
*/
void find_watersheds(
  float_2d &elevations, int_2d &labels, bool alter_elevations
){
  grid_cellz_pq open;
  std::queue<grid_cellz> pit;
  bool_2d closed;
  unsigned long processed_cells=0;
  unsigned long pitc=0,openc=0;
  int clabel=1;  //TODO: Thought this was more clear than zero in the results.
  ProgressBar progress;

  diagnostic("\n###Barnes Flood+Watershed Labels\n");
  diagnostic("Setting up boolean flood array matrix...");
  closed.copyprops(elevations);
  closed.init(false);
  diagnostic("succeeded.\n");

  diagnostic("Setting up boolean flood array matrix...");
  labels.copyprops(elevations);
  labels.no_data=-1;
  labels.init(labels.no_data);
  diagnostic("succeeded.\n");

  diagnostic_arg("The priority queue will require approximately %ldMB of RAM.\n",(elevations.width()*2+elevations.height()*2)*((long)sizeof(grid_cellz))/1024/1024);
  diagnostic("Adding cells to the priority queue...");
  for(int x=0;x<elevations.width();x++){
    open.push_cell(x,0,elevations(x,0) );
    open.push_cell(x,elevations.height()-1,elevations(x,elevations.height()-1) );
    closed(x,0)=true;
    closed(x,elevations.height()-1)=true;
  }
  for(int y=1;y<elevations.height()-1;y++){
    open.push_cell(0,y,elevations(0,y)  );
    open.push_cell(elevations.width()-1,y,elevations(elevations.width()-1,y) );
    closed(0,y)=true;
    closed(elevations.width()-1,y)=true;
  }
  diagnostic("succeeded.\n");

  diagnostic("%%Performing the Barnes Flood+Watershed Labels...\n");
  progress.start( elevations.width()*elevations.height() );
  while(open.size()>0 || pit.size()>0){
    grid_cellz c;
    if(pit.size()>0){
      c=pit.front();
      pit.pop();
      pitc++;
    } else {
      c=open.top();
      open.pop();
      openc++;
    }
    processed_cells++;

    if(labels(c.x,c.y)==labels.no_data && elevations(c.x,c.y)!=elevations.no_data)  //Implies a cell without a label which borders the edge of the DEM or a region of no_data
      labels(c.x,c.y)=clabel++;

    for(int n=1;n<=8;n++){
      int nx=c.x+dx[n];
      int ny=c.y+dy[n];
      if(!elevations.in_grid(nx,ny)) continue;
      if(closed(nx,ny)) 
        continue;

      labels(nx,ny)=labels(c.x,c.y);

      closed(nx,ny)=true;
      if(elevations(nx,ny)<=c.z){
        if(alter_elevations)
          elevations(nx,ny)=c.z;
        pit.push(grid_cellz(nx,ny,c.z));
      } else
        open.push(grid_cellz(nx,ny,elevations(nx,ny)));
    }
    progress.update(processed_cells);
  }
  diagnostic_arg("\t\033[96msucceeded in %.2lfs\033[39m\n",progress.stop());
  diagnostic_arg(
    "%ld cells processed. %ld in pits, %ld not in pits.\n",
    processed_cells,pitc,openc
  );
}



#endif