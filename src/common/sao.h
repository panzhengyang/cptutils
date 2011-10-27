/*
  sao.h

  J.J. Green 2011
*/

#ifndef SAO_H
#define SAO_H

typedef struct sao_t sao_t;

extern sao_t* sao_new(void);
extern void sao_destroy(sao_t*);

extern int sao_red_push(sao_t*, double, double);
extern int sao_blue_push(sao_t*, double, double);
extern int sao_green_push(sao_t*, double, double);

typedef int (stop_fn_t)(double,double,void*);

extern int sao_eachred(sao_t*, stop_fn_t*, void*);
extern int sao_eachgreen(sao_t*,stop_fn_t*, void*);
extern int sao_eachblue(sao_t*, stop_fn_t*, void*);

#endif
