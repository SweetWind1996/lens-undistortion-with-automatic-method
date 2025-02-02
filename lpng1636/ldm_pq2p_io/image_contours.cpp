/*
   Copyright (c) 2010-2014, AMI RESEARCH GROUP <lalvarez@dis.ulpgc.es>
   License : CC Creative Commons "Attribution-NonCommercial-ShareAlike"
   see http://creativecommons.org/licenses/by-nc-sa/3.0/es/deed.en
 */


#ifndef AMI_DLL_CPP
  #define AMI_DLL_CPP
#endif

/**
 * \file image_contours.cpp
 * \brief Subpixel image contour basic methods
 * \author Luis Alvarez \n \n
 */

#include "image_contours.h"
#include "ami_point2d.h"
#include <math.h>

using namespace ami;


/**
 * \fn  image_contours::image_contours(int width_c,
                                                         int height_c)
 * \brief Constructor taking memory
 * \author Luis Alvarez
 */
AMI_DLL_CPP image_contours::
  image_contours(int width_c/** New subpixel width */,
                          int height_c/** New subpixel height */)
{
  width=width_c;
  height=height_c;
  c=new bool[width*height];
  x=new float[width*height];
  y=new float[width*height];
  d=new float[width*height];
  cosine=new float[width*height];
  sine=new float[width*height];
}

/**
 * \fn image_contours::~image_contours()
 * \brief Destructor to free memory
 */
AMI_DLL_CPP image_contours::~image_contours()
{
  delete[] c;
  delete[] x;
  delete[] y;
  delete[] d;
  delete[] cosine;
  delete[] sine;
}

/** \fn image_contours & operator=(const image_contours &contours)
   * \brief Assignment operator
   * \author Daniel Santana-Cedrés
   */
image_contours & image_contours::operator=(const image_contours &contours)
{
  #ifdef _OPENMP
  #pragma omp parallel for
  #endif
  for(int i=0; i<(int)contours.index.size(); i++)
  {
    int j     = contours.index[i];
    x[j]      = contours.x[j];
    y[j]      = contours.y[j];
    cosine[j] =  contours.cosine[j];
    sine[j]   = contours.sine[j];
    if(sine[j]<0)
    {
      cosine[j] *=-1;
      sine[j]   *=-1;
    }
    c[j] = contours.c[j];
  }
  return *this;
}

/**
 * \fn void image_contours::clean(const int neigborhood_radius,
                                           const int min_neigbor_points,
                                           const double min_orientation_value,
                                           const int min_distance_point);
 * \brief Remove outlier contour points
 * \author Luis Alvarez
 */
void image_contours::clean(
  const int neighborhood_radius, /** radius of neighborhood to take into account */
  const int min_neighbor_points, /** min number of contour points in a neighborhood*/
  const double min_orientation_value, /** min average scalar product of 
                                          neigborhood point orientation */
  const int min_distance_point) /** minimum distance between contour points */
{
  // WE CHECK THAT THE CONTOURS ARE NOT EMPTY
  if(c==NULL || x==NULL || y==NULL || cosine==NULL || sine==NULL) return;

  // WE CHECK PARAMETER neighborhood_radius
  if(neighborhood_radius<=0) return;

  int Nedges=index.size();
  if(Nedges==0) return;

  // AUXILIARY VARIABLES
  vector<double> scalar_product(width*height,0.);
  vector<int> number_neighborhood_points(width*height,0);
  int i_min=neighborhood_radius;
  int i_max=width-neighborhood_radius;
  int j_min=neighborhood_radius;
  int j_max=height-neighborhood_radius;

  // WE COMPUTE THE NUMBER OF NEIGHBORHOOD POINTS,
  // THE NEIGHBORHOOD SCALAR PRODUCT AND WE FILL index VECTOR
  int p=0;
  for(int j=j_min;j<j_max;j++){
    int mj=j*width;
    for(int i=i_min;i<i_max;i++){
      int m=mj+i;
      if(!c[m]) continue;
      index[p++]=m;
      for(int k=-neighborhood_radius;k<=neighborhood_radius;k++){
        int k2=m+k*width;
        for(int l=-neighborhood_radius;l<=neighborhood_radius;l++){
          int n=k2+l;
          if(!c[n] || n==m) continue;
          scalar_product[m]+=cosine[n]*cosine[m]+sine[n]*sine[m];
          number_neighborhood_points[m]++;
        }
      }
    }
  }
  // WE RESIZE index VECTOR
  index.resize(p);

  // WE REMOVE CONTOUR POINTS ACCORDING TO THE NUMBER OF NEIGHBORHOOD POINTS
  // AND STABILITY OF POINT ORIENTATION (CORNERS ARE REMOVED)
  for(int Np=index.size(),q=0;q<Np;q++){
    int m=index[q];
    if(number_neighborhood_points[m]<min_neighbor_points ||
      scalar_product[m]<(number_neighborhood_points[m]*min_orientation_value)){
      c[m]=0;
      continue;
    }
  }

  // WE REMOVE ISOLATED POINTS USING AN ITERATIVE PROCEDURE
  for(int s=0;s<4;s++){
    // WE COMPUTE THE NUMBER OF POINTS IN THE NEIGHBORHOOD
    for(int Np=index.size(),q=0;q<Np;q++){
      int m=index[q];
      if(!c[m]) continue;
      number_neighborhood_points[m]=0;
      for(int k=-neighborhood_radius;k<=neighborhood_radius;k++){
        int k2=m+k*width;
        for(int l=-neighborhood_radius;l<=neighborhood_radius;l++){
          int n=k2+l;
          if(!c[n] || n==m) continue;
          number_neighborhood_points[m]++;
        }
      }
    }
    // WE REMOVE ISOLATED CONTOUR POINTS
    for(int Np=index.size(),q=0;q<Np;q++){
      int m=index[q];
      if(!c[m]) continue;
      if(number_neighborhood_points[m]<min_neighbor_points){
        c[m]=0;
      }
    }
  }

  // WE KEEP ONLY 1 CONTOUR POINT IN EACH WINDOW OF RADIUS min_distance_point
  if(min_distance_point>0){
    i_min=min_distance_point;
    i_max=width-min_distance_point;
    j_min=min_distance_point;
    j_max=height-min_distance_point;
    int window_size=2*min_distance_point+1;
    for(int j=j_min;j<j_max;j+=window_size){
      int mj=j*width;
      for(int i=i_min;i<i_max;i+=window_size){
        int m=mj+i;
        double max=scalar_product[m];
        int m_max=m;
        for(int k=-min_distance_point;k<=min_distance_point;k++){
          int k2=m+k*width;
          for(int l=-min_distance_point;l<=min_distance_point;l++){
            int n=k2+l;
            if(!c[n] || m==n) continue;
            if(scalar_product[n]>max){
              max=scalar_product[n];
              c[m_max]=0;
              m_max=n;
            }
            else {
              c[n]=0;
            }
          }
        }
      }
    }
  }

  // WE FILL INDEX VECTOR
  int m=0;
  for(int Np=index.size(),q=0;q<Np;q++){
    if(c[index[q]]>0) index[m++]=index[q];
  }
  index.resize(m);
  return;
}