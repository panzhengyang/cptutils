/*
  colour.h
  
  simple GRB colours for gimpcpt

  (c) J.J.Green 2001
  $Id: colour.h,v 1.4 2001/05/24 01:04:32 jjg Exp $
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
    int hue,sat,val;
} hsv_t;

extern int parse_rgb(char*,rgb_t*);

extern int hsv_to_rgb(double,double,double,double*,double*,double*);
extern int rgb_to_hsv(double,double,double,double*,double*,double*);

extern int colour_to_rgb(double*,rgb_t*);
extern int rgb_to_colour(rgb_t,double*);


#endif

