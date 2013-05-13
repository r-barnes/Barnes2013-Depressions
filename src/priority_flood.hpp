#ifndef pit_fill_include
#define pit_fill_include
#include "data_structures.h"
#include <queue>
#include <limits>



//original_priority_flood
/**
  @brief  Fills all pits and removes all digital dams from a DEM
  @author Richard Barnes (rbarnes@umn.edu)

    Priority-Flood starts on the edges of the DEM and then works its way
    inwards using a priority queue to determine the lowest cell which has
    a path to the edge. The neighbours of this cell are added to the priority
    queue. If the neighbours are lower than the cell which is adding them, then
    they are raised to match its elevation; this fills depressions.

  @param[in,out]  &elevations   A grid of cell elevations

  @pre
    1. **elevations** contains the elevations of every cell or a value _NoData_
       for cells not part of the DEM.

  @post
    1. **elevations** contains the elevations of every cell or a value _NoData_
       for cells not part of the DEM.
    2. **elevations** contains no landscape depressions or digital dams.
*/
template <class T>
void original_priority_flood(array2d<T> &elevations){
  grid_cellz_pq open;
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

  diagnostic("%%Performing the original Priority Flood...\n");
  progress.start( elevations.width()*elevations.height() );
  while(open.size()>0){
    grid_cellz c=open.top();
    open.pop();
    processed_cells++;

    for(int n=1;n<=8;n++){
      int nx=c.x+dx[n];
      int ny=c.y+dy[n];
      if(!elevations.in_grid(nx,ny)) continue;
      if(closed(nx,ny)) 
        continue;

      closed(nx,ny)=true;
      if(elevations(nx,ny)<elevations(c.x,c.y)) ++pitc;
      elevations(nx,ny)=std::max(elevations(nx,ny),elevations(c.x,c.y));
      open.push_cell(nx,ny,elevations(nx,ny));
    }
    progress.update(processed_cells);
  }
  diagnostic_arg(SUCCEEDED_IN,progress.stop());
  diagnostic_arg("%ld cells processed. %ld in pits.\n",processed_cells,pitc);
}



//improved_priority_flood
/**
  @brief  Fills all pits and removes all digital dams from a DEM
  @author Richard Barnes (rbarnes@umn.edu)

    Priority-Flood starts on the edges of the DEM and then works its way
    inwards using a priority queue to determine the lowest cell which has
    a path to the edge. The neighbours of this cell are added to the priority
    queue if they are higher. If they are lower, they are raised to the
    elevation of the cell adding them, thereby filling in pits. The neighbors
    are then added to a "pit" queue which is used to flood pits. Cells which
    are higher than a pit being filled are added to the priority queue. In this
    way, pits are filled without incurring the expense of the priority queue.

  @param[in,out]  &elevations   A grid of cell elevations

  @pre
    1. **elevations** contains the elevations of every cell or a value _NoData_
       for cells not part of the DEM.

  @post
    1. **elevations** contains the elevations of every cell or a value _NoData_
       for cells not part of the DEM.
    2. **elevations** contains no landscape depressions or digital dams.
*/
template <class T>
void improved_priority_flood(array2d<T> &elevations){
  grid_cellz_pq open;
  std::queue<grid_cellz> pit;
  bool_2d closed;
  unsigned long processed_cells=0;
  unsigned long pitc=0;
  ProgressBar progress;

  diagnostic("\n###Improved Priority-Flood\n");
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

  diagnostic("%%Performing the improved Priority-Flood...\n");
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
    is increased by a small amount to ensure that they have a drainage path and
    they are added to a "pit" queue which is used to flood pits. Cells which
    are higher than a pit being filled are added to the priority queue. In this
    way, pits are filled without incurring the expense of the priority queue.

  @param[in,out]  &elevations   A grid of cell elevations

  @pre
    1. **elevations** contains the elevations of every cell or a value _NoData_
       for cells not part of the DEM.

  @post
    1. **elevations** contains the elevations of every cell or a value _NoData_
       for cells not part of the DEM.
    2. **elevations** has no landscape depressions, digital dams, or flats.
*/
template <class T>
void priority_flood_epsilon(array2d<T> &elevations){
  grid_cellz_pq open;
  std::queue<grid_cellz> pit;
  bool_2d closed;
  unsigned long processed_cells=0;
  unsigned long pitc=0;
  ProgressBar progress;
  T PitTop=elevations.no_data;
  int false_pit_cells=0;

  diagnostic("\n###Priority-Flood+Epsilon\n");
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

  diagnostic("%%Performing Priority-Flood+Epsilon...\n");
  progress.start( elevations.width()*elevations.height() );
  while(open.size()>0 || pit.size()>0){
    grid_cellz c;
    if(pit.size()>0 && open.size()>0 && open.top().z==pit.front().z){
      c=open.top();
      open.pop();
      PitTop=elevations.no_data;
    } else if(pit.size()>0){
      c=pit.front();
      pit.pop();
      if(PitTop==elevations.no_data)
        PitTop=elevations(c.x,c.y);
    } else {
      c=open.top();
      open.pop();
      PitTop=elevations.no_data;
    }
    processed_cells++;

    for(int n=1;n<=8;n++){
      int nx=c.x+dx[n];
      int ny=c.y+dy[n];

      if(!elevations.in_grid(nx,ny)) continue;

      if(closed(nx,ny)) 
        continue;
      closed(nx,ny)=true;

      if(elevations(nx,ny)==elevations.no_data)
        pit.push(grid_cellz(nx,ny,elevations.no_data));

      else if(elevations(nx,ny)<=nextafterf(c.z,std::numeric_limits<float>::infinity())){
        if(PitTop!=elevations.no_data && PitTop<elevations(nx,ny) && nextafterf(c.z,std::numeric_limits<float>::infinity())>=elevations(nx,ny))
          ++false_pit_cells;
        ++pitc;
        elevations(nx,ny)=nextafterf(c.z,std::numeric_limits<float>::infinity());
        pit.push(grid_cellz(nx,ny,elevations(nx,ny)));
      } else
        open.push_cell(nx,ny,elevations(nx,ny));
    }
    progress.update(processed_cells);
  }
  diagnostic_arg(SUCCEEDED_IN,progress.stop());
  diagnostic_arg("%ld cells processed. %ld in pits.\n",processed_cells,pitc);
  if(false_pit_cells)
    diagnostic_arg("\033[91mIn assigning negligible gradients to depressions, some depressions rose above the surrounding cells. This implies that a larger storage type should be used. The problem occured for %d of %ld cells.\033[39m\n",false_pit_cells,elevations.data_cells);
}






//priority_flood_flowdirs
/**
  @brief  Determines D8 flow directions and implicitly fills pits.
  @author Richard Barnes (rbarnes@umn.edu)

    This version of Priority-Flood starts on the edges of the DEM and then
    works its way inwards using a priority queue to determine the lowest cell
    which has a path to the edge. The neighbours of this cell are given D8 flow
    directions which point to it. All depressions are implicitly filled and
    digital dams removed.

    Based on Metz 2011.

  @param[in]   &elevations  A grid of cell elevations
  @param[out]  &flowdirs    A grid of D8 flow directions

  @pre
    1. **elevations** contains the elevations of every cell or a value _NoData_
       for cells not part of the DEM.

  @post
    1. **flowdirs** contains a D8 flow direction of each cell or a value
       _NO_FLOW_ for those cells which are not part of the DEM.
    2. **flowdirs** has no cells which are not part of a continuous flow
       path leading to the edge of the DEM.
*/
template <class T>
void priority_flood_flowdirs(const array2d<T> &elevations, char_2d &flowdirs){
  grid_cellzk_pq open;
  bool_2d closed;
  unsigned long processed_cells=0;
  ProgressBar progress;

  diagnostic("\n###Priority-Flood+Flow Directions\n");
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
    flowdirs(x,0)=3;
    flowdirs(x,elevations.height()-1)=7;
    closed(x,0)=true;
    closed(x,elevations.height()-1)=true;
  }
  for(int y=1;y<elevations.height()-1;y++){
    open.push_cell(0,y,elevations(0,y) );
    open.push_cell(elevations.width()-1,y,elevations(elevations.width()-1,y) );
    flowdirs(0,y)=1;
    flowdirs(elevations.width()-1,y)=5;
    closed(0,y)=true;
    closed(elevations.width()-1,y)=true;
  }
  diagnostic("succeeded.\n");

  flowdirs(0,0)=2;
  flowdirs(flowdirs.width()-1,0)=4;
  flowdirs(0,flowdirs.height()-1)=8;
  flowdirs(flowdirs.width()-1,flowdirs.height()-1)=6;

  const int d8_order[9]={0,1,3,5,7,2,4,6,8};
  diagnostic("%%Performing Priority-Flood+Flow Directions...\n");
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
  @brief  Indicates which cells are in pits
  @author Richard Barnes (rbarnes@umn.edu)

    This version of Priority-Flood starts on the edges of the DEM and then
    works its way inwards using a priority queue to determine the lowest cell
    which has a path to the edge. If a cell is lower than this cell then it is
    part of a pit and is given a value 1 to indicate this. The result is a grid
    where every cell which is in a pit is labeled.

  @param[in]   &elevations   A grid of cell elevations
  @param[out]  &pit_mask     A grid of indicating which cells are in pits

  @pre
    1. **elevations** contains the elevations of every cell or a value _NoData_
       for cells not part of the DEM.

  @post
    1. **pit_mask** contains a 1 for each cell which is in a pit and a 0 for
       each cell which is not.
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



//priority_flood_watersheds
/**
  @brief  Gives a common label to all cells which drain to a common point
  @author Richard Barnes (rbarnes@umn.edu)

    All the edge cells of the DEM are given unique labels. This version of
    Priority-Flood starts on the edges of the DEM and then works its way
    inwards using a priority queue to determine the lowest cell which has a
    path to the edge. The neighbours of this cell are then given its label. All
    depressions are implicitly filled and digital dams removed. The result is
    a grid of cells where all cells with a common label drain to a common
    point.

  @param[in,out] elevations        A grid of cell elevations
  @param[out]    labels            A grid to hold the watershed labels
  @param[in]     alter_elevations
    If true, then **elevations** is altered as though improved_priority_flood()
    had been applied. The result is that all cells drain to the edges of the
    DEM. Otherwise, **elevations** is not altered.

  @pre
    1. **elevations** contains the elevations of every cell or a value _NoData_
       for cells not part of the DEM.

  @post
    1. **elevations** contains no depressions or digital dams, if
       **alter_elevations** was set.
    2. **labels** contains a label for each cell indicating its membership in a
       given watershed. Cells bearing common labels drain to common points.
*/
void priority_flood_watersheds(
  float_2d &elevations, int_2d &labels, bool alter_elevations
){
  grid_cellz_pq open;
  std::queue<grid_cellz> pit;
  bool_2d closed;
  unsigned long processed_cells=0;
  unsigned long pitc=0,openc=0;
  int clabel=1;  //TODO: Thought this was more clear than zero in the results.
  ProgressBar progress;

  diagnostic("\n###Priority-Flood+Watershed Labels\n");
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

  diagnostic("%%Performing Priority-Flood+Watershed Labels...\n");
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
