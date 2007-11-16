/*
  cpthsv.h

  J.J.Green 2007
  $Id: cptsvg.h,v 1.1 2005/05/30 23:17:19 jjg Exp $
*/

#ifndef CPTHSV_H
#define CPTHSV_H

enum hsvchan_e { hue, saturation, value };
typedef enum hsvchan_e hsvchan_t;

enum hsvop_e { plus, minus, times, percent };
typedef enum hsvop_e hsvop_t;

typedef struct
{
  double z;
  hsvchan_t channel;
  hsvop_t op;
} hsvtrans_t;

typedef struct
{
  int n;
  hsvtrans_t *tran;
  int verbose;
} cpthsv_opt_t;

extern int cpthsv(char*,char*,cpthsv_opt_t);

#endif
