/*
  grd5_type.h
  Copyright (c) J.J. Green 2014
*/

#ifndef GRD5_TYPE_H
#define GRD5_TYPE_H

#define TYPE_UNKNOWN       0
#define TYPE_PATTERN       1
#define TYPE_DESCRIPTION   2
#define TYPE_VAR_LEN_LIST  3
#define TYPE_TEXT          4
#define TYPE_OBJECT        5
#define TYPE_UntF          6
#define TYPE_BOOL          7
#define TYPE_LONG          8
#define TYPE_DOUBLE        9

extern int grd5_type(const char*);

#endif
