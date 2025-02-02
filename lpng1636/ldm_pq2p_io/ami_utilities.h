/*
   Copyright (c) 2010-2014, AMI RESEARCH GROUP <lalvarez@dis.ulpgc.es>
   License : CC Creative Commons "Attribution-NonCommercial-ShareAlike"
   see http://creativecommons.org/licenses/by-nc-sa/3.0/es/deed.en
 */

#ifndef UTILITIES_H
#define UTILITIES_H

#include <vector>
#include "ami_image.h"
#include "ami_image_draw.h"
#include "ami_image_primitives.h"
#include <fstream>

  /* *
  * \def ami_free1d(direccion)
  * \brief Frees the memory of the array created by ami_calloc1d o ami_malloc1d.
  * \author Luis Alvarez
  */
  #define ami_free1d(direccion) { if (direccion != NULL){\
    free(direccion);direccion=NULL;}}

  /**
  * \def ami_malloc1d(direccion,tipo,size)
  * \brief Get memory in the array direccion with size given.
  * \author Luis Alvarez
  */
  #ifdef _AMI_TIF_H_
  #undef ami_malloc1d
  #endif
  #define ami_malloc1d(direccion,tipo,size) {\
    if (size > 0){\
      direccion=(tipo *) malloc(sizeof(tipo)*(size)); \
      if (direccion == NULL)\
      { int ml; printf("\nMemoria insuficiente.\n");int val=scanf("%d",&ml);\
        if(val==EOF) printf("Read error\n");}}\
    else direccion = NULL;}

  #define ami2_malloc2d(direccion,tipo,height,width) {int ml; \
    direccion=(tipo **) malloc(sizeof(tipo *)*(height)); \
    direccion[0]=(tipo *)malloc(sizeof(tipo)*(width)*(height)); \
    for(ml=0;ml<(height);ml++) direccion[ml]=&(direccion[0][ml*(width)]);\
  }
        
  #define ami2_free2d(direccion) { free(direccion[0]); free(direccion); }

  #define ami2_malloc1d(direccion,tipo,size) {direccion=(tipo *) malloc(sizeof(tipo)*(size));}
  
  /**
  * Structure for saving the input parameters
  */
  typedef struct
  {
    float canny_low_threshold/** Low threshold for canny detector */, 
          canny_high_threshold/** High threshold for canny detector */,
          initial_distortion_parameter/** Left side of allowed distortion 
                                          parameter interval */,
          final_distortion_parameter/** Right side of allowed distortion 
                                        parameter interval */,
          distance_point_line_max_hough/** Hough parameter */,
          angle_point_orientation_max_difference/** Hough parameter */;
    string input_name/** Input image name */,
           canny_name/** Canny result name */,
           hough_name/** Name for the image with the detected lines */,
           result_name/** Result image name */,
           tmodel/** Type of model */,
           primitives_file/** Name for the output file with the detected 
                              primitives */;
    bool opt_center/** Center optimization */;
    
  }input_params;

  std::vector < std::vector <unsigned int> >
  boundary_neighborhood_9n(const unsigned int width, const unsigned int height);

  /**
  * \fn ami::image<unsigned char> drawHoughLines(image_primitives ip,
                                                int width, int height,
                                                ami::image<unsigned char> &bn)
  * \brief Draw the points detected by the improved Hough transform
  * \author Luis Alvarez and Daniel Santana-Cedr�s
  */
  void drawHoughLines(image_primitives ip /**Detected primitives*/,
                    ami::image<unsigned char> &bn /**Gray level image where we
                    draw the lines*/);

  /**
  * \fn void invert(ami::image<unsigned char> &input,
                   ami::image<unsigned char> &output)
  * \brief Invert the colors of the input image
  * \author Luis Alvarez and Daniel Santana-Cedr�s
  */
  void invert(ami::image<unsigned char> &input /**Input image*/,
            ami::image<unsigned char> &output/**Output image with the inverted
            colors*/);

  /**
  * \fn print_function_syntax()
  * \brief Print the function syntax
  * \author Luis Alvarez and Daniel Santana-Cedr�s
  */
  void print_function_syntax_lens_distortion_correction_2p_iterative_optimization();

  /**
  * \fn check_params(char *argv[])
  * \brief Check the input parameters
  * \author Luis Alvarez and Daniel Santana-Cedr�s
  */
  int check_params_lens_distortion_correction_2p_iterative_optimization(char *argv[],
                                                        input_params &i_params);

  /**
  * \fn count_points(image_primitives ip)
  * \brief This function counts the number of points of the primitives
  * \author Luis Alvarez and Daniel Santana-Cedr�s
  */
  int count_points(image_primitives ip);

  /**
  * \fn manage_failure(char *argv[])
  * \brief This function manages the output in case of failure
  * \author Luis Alvarez and Daniel Santana-Cedr�s
  */
  void manage_failure(char *argv[], int code);

  /**
  * \fn double update_rsqmax(point2d<double> dc, int w, int h);
  * \brieg This function computes the distance from the center
  *        of distortion to the furthest corner of the image
  * \author Luis Alvarez
  */
  double update_rsqmax(point2d<double> dc, int w, int h);


  /**
  * \fn void compute_ps(double &p1, double &p2, lens_distortion_model &ldm,
                       int w, int h, bool quo)
  * \brief This procedure computes the values of the normalized distortion
          parameters
  * \author Luis Alvarez and Daniel Santana-Cedr�s
  */
  void compute_ps(double &p1, double &p2, const lens_distortion_model &ldm,
                int w, int h);

  /**
  * \fn int ami2_gauss(double **A,double *b,int N);
  * \brief This function solve a system of equations by means of Gauss method
  * \author Luis Alvarez
  */
  int ami2_gauss(double **A,double *b,int N);

  /**
  * \fn int check_invertibility(lens_distortion_model &ldm)
  * \brief This function checks the invertibility of the lens distortion model
  * \author Luis Alvarez
  */
  bool check_invertibility(lens_distortion_model &ldm, int w, int h);
  
                
#endif
