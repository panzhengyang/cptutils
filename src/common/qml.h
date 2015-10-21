/*
  qml.h
  Copyright (c) J.J. Green 2015
*/

#ifndef QML_H
#define QML_H

#define QML_TYPE_DISCRETE 1
#define QML_TYPE_INTERPOLATED 2

typedef struct {
  char red, green, blue;
  double value;
  char *label;
} qml_entry_t;

typedef struct {
  size_t n;
  qml_entry_t *entries;
  int type;
} qml_ramp_t;

extern qml_ramp_t* qml_ramp_new(int type, size_t n);
extern void qml_ramp_destroy(qml_ramp_t* qml_ramp);
extern int qml_set_entry(qml_ramp_t* qml_ramp, size_t i,
			 qml_entry_t *entry);

#endif
