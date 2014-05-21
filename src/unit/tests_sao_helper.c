/*
  tests_sao_helper.c
  Copyright (c) J.J. Green 2014
*/

#include <stdlib.h>
#include "tests_sao_helper.h"

extern sao_t* build_sao(void)
{
  sao_t *sao;

  if ( (sao = sao_new()) == NULL )
    return NULL;
  
  double dat[3][2] = {
    {0.0, 0.25},
    {0.5, 0.50},
    {1.0, 0.25}
  };
   
  int i, err = 0;

  for (i=0 ; i<3 ; i++)
    {
      err += sao_red_push(   sao, dat[i][0], dat[i][1] );
      err += sao_green_push( sao, dat[i][0], dat[i][1] );
      err += sao_blue_push(  sao, dat[i][0], dat[i][1] );
    }

  if (err)
    {
      sao_destroy(sao);
      return NULL;
    }

  return sao;
}
