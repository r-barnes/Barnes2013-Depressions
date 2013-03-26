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

template <class T>
class array2d {
  private:
    typedef std::vector< std::vector<T> > arr2d;
    arr2d data;
  public:
    ///Value corresponding the length of one edge of a square DEM cell
    double cellsize;
    ///Global grid location of lower left x-coordinate
    double xllcorner;
    ///Global grid location of lower left y-coordinate
    double yllcorner;
    ///Number of cells containing data (excludes NO_DATA cells)
    long data_cells;
    ///NO_DATA value. The cell does not contain data, should not be processed.
    T no_data;

    array2d ();
    ///Creates array2d with no data, but invokes copyprops()
    template<class U> array2d (const array2d<U> &copyfrom);
    long width() const  {return data[0].size();}
    long height() const {return data.size();}
    ///Copys everything but the data from another array2d
    template<class U> void copyprops (const array2d<U> &copyfrom);
    ///Prints an estimate of the file size were the array printed in ASCII
    long estimated_output_size();
    ///Sets all the cells of an array2d to "val"
    void init(T val);
    ///Returns true if each cell of the array2d equals its counterpart in "other"
    bool operator==(const array2d<T> &other) const;
    ///Returns true if (x,y) is within the bounds of the array2d
    inline bool in_grid(int x, int y) const
      {return (x>=0 && y>=0 && x<width() && y<height());}
    ///Returns true if (x,y) is not an edge cell
    inline bool interior_grid(int x, int y) const
      {return (x>=1 && y>=1 && x<width()-1 && y<height()-1);}
    ///Returns true if (x,y) lies on the edge of the array2d
    inline bool edge_grid(int x, int y) const
      {return (x==0 || y==0 || x==width()-1 || y==height()-1);}
    ///Returns a reference to (x,y)
    T& operator()(int x, int y)
      {return data[y][x];}
    ///Returns a const reference to (x,y)
    const T& operator()(int x, int y) const
      {return data[y][x];}
    ///Resizes the array2d. May or may not be destructive to existing data.
    void resize(int width, int height);
    ///Destroys all data in the array2d.
    void clear() {data.clear();}
};

template <class T>
array2d<T>::array2d(){
  cellsize=-1;
  xllcorner=-1;
  yllcorner=-1;
  data_cells=-1;
  no_data=-1;
}

template <class T>
void array2d<T>::resize(int width, int height){
  fprintf(
    stderr,
    "\n\tApprox RAM requirement: %lluMB\n",
    (unsigned long long)width * (unsigned long long)height *
      (unsigned long long)sizeof(T) / 1024 / 1024
  );
  data.resize(height, std::vector<T> (width));
}

template<class T>
template<class U>
void array2d<T>::copyprops (const array2d<U> &copyfrom){
  cellsize=copyfrom.cellsize;
  xllcorner=copyfrom.xllcorner;
  yllcorner=copyfrom.yllcorner;
  data_cells=copyfrom.data_cells;
  no_data=copyfrom.no_data;
  resize(copyfrom.width(),copyfrom.height());
}

template <class T>
template <class U>
array2d<T>::array2d(const array2d<U> &copyfrom){
  *this=array2d();
  copyprops(copyfrom);
}

template <class T>
void array2d<T>::init(T val){
  #pragma omp parallel for
  for(int x=0;x<width();x++)
  for(int y=0;y<height();y++)
    operator()(x,y)=val;
}

template <> inline long array2d<float>::estimated_output_size(){
  return 9*this->width()*this->height();
}
template <> inline long array2d<char>::estimated_output_size(){
  return 4*this->width()*this->height();
}
template <> inline long array2d<bool>::estimated_output_size(){
  return 2*this->width()*this->height();
}
template <> inline long array2d<unsigned int>::estimated_output_size(){
  return 9*this->width()*this->height();
}

template <class T>
bool array2d<T>::operator==(const array2d<T> &other) const {
  if(width()!=other.width() || height()!=other.height())
    return false; 
  for(int x=0;x<width();x++)
  for(int y=0;y<height();y++)
    if(operator()(x,y)!=other(x,y))
      return false;
  return true;
}




typedef array2d<double>       double_2d;
typedef array2d<float>        float_2d;
typedef array2d<signed char>  char_2d;
typedef array2d<char>         bool_2d;
typedef array2d<unsigned int> uint_2d;
typedef array2d<int>          int_2d;


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
class grid_cellz {
  public:
    int x;           ///< Grid cell's x-coordinate
    int y;           ///< Grid cell's y-coordinate
    float z;         ///< Grid cell's z-coordinate
    grid_cellz(int x, int y, float z):x(x), y(y), z(z) {}
    grid_cellz(){}
    bool operator< (const grid_cellz& a) const { return z< a.z; }
    bool operator> (const grid_cellz& a) const { return z> a.z; }
    bool operator>=(const grid_cellz& a) const { return z>=a.z; }
    bool operator<=(const grid_cellz& a) const { return z<=a.z; }
    bool operator==(const grid_cellz& a) const { return z==a.z; }
    bool operator!=(const grid_cellz& a) const { return !operator==(a); }
};



/// Stores the (x,y,z) coordinates of a grid cell and a priority indicator k;
/// useful for stable priority sorting with \ref grid_cellzk_compare
/// @todo z-coordinate should be templated
typedef struct grid_cell_typezk {
  int x;          ///< Grid cell's x-coordinate 
  int y;          ///< Grid cell's y-coordinate 
  float z;        ///< Grid cell's z-coordinate
  int k;          ///< Used to store an integer to make sorting stable
  grid_cell_typezk(int x0, int y0, float z0, int k0):x(x0),y(y0),z(z0),k(k0){}
  grid_cell_typezk(){}
} grid_cellzk;

/// Used for stable sorting of grid cells defined by \struct grid_cell_typezk
class grid_cellzk_compare{
  bool reverse;
  public:
    grid_cellzk_compare(const bool& revparam=false){reverse=revparam;}
    bool operator() (const grid_cellzk &lhs, const grid_cellzk &rhs) const{
      if (reverse) return (lhs.z<rhs.z || (lhs.z==rhs.z && lhs.k<rhs.k));
      else return (lhs.z>rhs.z || (lhs.z==rhs.z && lhs.k>rhs.k));
    }
};



typedef std::priority_queue<grid_cellz, std::vector<grid_cellz>, std::greater<grid_cellz> > grid_cellz_pq;
typedef std::priority_queue<grid_cellzk, std::vector<grid_cellzk>, grid_cellzk_compare> grid_cellzk_pq;

#endif
