/*
  colour.h
  
  simple GRB colours for gimpcpt

  (c) J.J.Green 2001,2004
  $Id: colour.h,v 1.2 2004/02/13 01:17:49 jjg Exp jjg $
*/

#ifndef COLOUR_H
#define COLOUR_H

/*
  these are the GMT colour types : 

  rgb_t : rgb triple of integers in the range 0-255
  hsv_t : hsv triple of doubles, hue in the range 0-360.
          saturation & value in the range of 0-1. The hue
	  rotates clockwise (ie, corresponds to a gimp
	  GRAD_HSV_CW type gradient)

  these correspond to colours as interpreted in cpt files.
*/

#define RGB_MODEL 
#define HSV_MODEL

typedef struct rgb_t
{
    int red,green,blue;
} rgb_t;

typedef struct hsv_t
{
    double hue,sat,val;
} hsv_t;

typedef union colour_t
{
  rgb_t rgb;
  hsv_t hsv;
} colour_t;

extern int parse_rgb(char*,rgb_t*);

/*
  gimp colour types rgbD and hsvD are triples of doubles
  int the range 0-1
*/

extern int hsvD_to_rgbD(double*,double*);
extern int rgbD_to_hsvD(double*,double*);

extern int rgbD_to_rgb(double*,rgb_t*);
extern int rgb_to_rgbD(rgb_t,double*);

extern int rgbD_to_hsv(double*,hsv_t*);
extern int hsv_to_rgbD(hsv_t,double*);


#endif

