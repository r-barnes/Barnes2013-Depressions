#!/usr/bin/env python3
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm
import sys
import gdal
from mpl_toolkits.mplot3d import Axes3D


def getGDALArray(filename):
  dem     = gdal.Open(filename)
  gt      = dem.GetGeoTransform()
  dem     = dem.ReadAsArray()
  
  xres = gt[1]
  yres = gt[5]

  #X = np.arange(gt[0], gt[0] + dem.shape[1]*xres, xres)
  #Y = np.arange(gt[3], gt[3] + dem.shape[0]*yres, yres)
  X = np.arange(0,dem.shape[1])
  Y = np.arange(0,dem.shape[0])

  X, Y = np.meshgrid(X, Y)

  return (dem,X,Y)

#Used to save test surfaces to files that ArcGRID files which GDAL can read
def readArrayFromArcGrid(dem,watersheds):
  dem, demX, demY = getGDALArray(dem)
  ws, wsX, wsY    = getGDALArray(watersheds)

  m = cm.ScalarMappable(cmap=cm.jet)
  m.set_array(ws)
  fig, ax = plt.subplots(figsize=(16,8), subplot_kw={'projection': '3d'})
  surf    = ax.plot_surface(demX,demY,dem, rstride=1, cstride=1, linewidth=0, antialiased=True) #facecolors=cm.jet(ws)
  ax.view_init(60,-105)
  #fig.colorbar(dem, shrink=0.4, aspect=20)
  plt.show()

  # negative contours will be dashed by default
  CS = plt.contour(demX, demY, dem, 6, colors='k')
  plt.clabel(CS, fontsize=9, inline=1)
  plt.pcolor(wsX, wsY, ws)
  plt.show()


if len(sys.argv)!=3:
  print("Syntax: {0} <dem> <watersheds>".format(sys.argv[0]))
  print("\tdem        = Digital elevation model")
  print("\twatersheds = Watershed labels to overlay on model")
  sys.exit(-1)

readArrayFromArcGrid(sys.argv[1],sys.argv[2])