/**
  @file
  Template code defining data structures used throughout the package.
  Defines 2D arrays and grid cells.

  Richard Barnes (rbarnes@umn.edu), 2012
*/
#ifndef _data_structures_included
#define _data_structures_included

#include <cstdio>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <queue>

/// Stores the (x,y) coordinates of a grid cell
class grid_cell {
  public:
    int x; ///< Grid cell's x-coordinate
    int y; ///< Grid cell's y-coordinate
    /// Initiate the grid cell without coordinates; should generally be avoided
    grid_cell(){}
    /// Initiate the grid cell to the coordinates (x0,y0)
    grid_cell(int x, int y):x(x),y(y){}
};


/// Stores the (x,y,z) coordinates of a grid cell; useful for priority sorting
/// with \ref grid_cellz_compare
/// @todo z-coordinate should be templated
class grid_cellz : public grid_cell {
  public:
    float z;         ///< Grid cell's z-coordinate
    grid_cellz(int x, int y, float z): grid_cell(x,y), z(z) {}
    grid_cellz(){}
    bool operator< (const grid_cellz& a) const { return z< a.z; }
    bool operator> (const grid_cellz& a) const { return z> a.z; }
    bool operator>=(const grid_cellz& a) const { return z>=a.z; }
    bool operator<=(const grid_cellz& a) const { return z<=a.z; }
    bool operator==(const grid_cellz& a) const { return z==a.z; }
    bool operator!=(const grid_cellz& a) const { return !operator==(a); }
};



/// Stores the (x,y,z) coordinates of a grid cell and a priority indicator k;
/// used by grid_cellz_pq
/// @todo z-coordinate should be templated
class grid_cellzk : public grid_cellz {
  public:
    int k;           ///< Used to store an integer to make sorting stable
    grid_cellzk(int x, int y, float z, int k): grid_cellz(x,y,z), k(k) {}
    grid_cellzk(){}
    bool operator< (const grid_cellzk& a) const { return z< a.z || (z==a.z && k<a.k); }
    bool operator> (const grid_cellzk& a) const { return z> a.z || (z==a.z && k>a.k); }
};

///A priority queue of grid_cells, sorted by ascending height
class grid_cellz_pq : public std::priority_queue<grid_cellz, std::vector<grid_cellz>, std::greater<grid_cellz> > {
  public:
    void push_cell(int x, int y, float z){
      std::priority_queue<grid_cellz, std::vector<grid_cellz>, std::greater<grid_cellz> >::push(grid_cellz(x,y,z));
    }
};

///A priority queue of grid_cells, sorted by ascending height or, if heights
///are equal, by the order of insertion
class grid_cellzk_pq : public std::priority_queue<grid_cellzk, std::vector<grid_cellzk>, std::greater<grid_cellzk> > {
  private:
    int count;
  public:
    grid_cellzk_pq() : count(0) {}
    void push(const grid_cellz &a){
      std::priority_queue<grid_cellzk, std::vector<grid_cellzk>, std::greater<grid_cellzk> >::push(grid_cellzk(a.x,a.y,a.z,count++));
    }
    void push_cell(int x, int y, float z){
      std::priority_queue<grid_cellzk, std::vector<grid_cellzk>, std::greater<grid_cellzk> >::push(grid_cellzk(x,y,z,count++));
    }
};

#endif
