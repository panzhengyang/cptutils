/*
  cpthsv.h

  J.J.Green 2007
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
