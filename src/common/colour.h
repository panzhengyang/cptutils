/*
  colour.h
  
  simple GRB colours for gimpcpt

  (c) J.J.Green 2001,2004
  $Id: colour.h,v 1.1 2002/06/18 22:25:44 jjg Exp jjg $
*/

#ifndef COLOUR_H
#define COLOUR_H

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

extern int hsv_to_rgb(double,double,double,double*,double*,double*);
extern int rgb_to_hsv(double,double,double,double*,double*,double*);

extern int colour_to_rgb(double*,rgb_t*);
extern int rgb_to_colour(rgb_t,double*);

extern int colour_to_hsv(double*,hsv_t*);
extern int hsv_to_colour(hsv_t,double*);


#endif

