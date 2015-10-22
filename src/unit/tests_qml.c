/*
  tests_qml.h
  Copyright (c) J.J. Green 2014
*/

#include <qml.h>
#include "tests_qml.h"

CU_TestInfo tests_qml[] =
  {
    {"create", test_qml_create },
    {"set_entry", test_qml_set_entry },
    CU_TEST_INFO_NULL,
  };

extern void test_qml_create(void)
{
  qml_t *qml;

  CU_TEST_FATAL((qml = qml_new(QML_TYPE_DISCRETE, 3)) != NULL);
  CU_ASSERT_EQUAL(qml->n, 3);
  CU_ASSERT_EQUAL(qml->type, QML_TYPE_DISCRETE);
  CU_ASSERT_PTR_NOT_NULL(qml->entries);

  qml_destroy(qml);
}

extern void test_qml_set_entry(void)
{
  qml_t *qml;

  CU_ASSERT_PTR_NOT_NULL_FATAL(qml = qml_new(QML_TYPE_DISCRETE, 3));

  qml_entry_t entry = {
    .red   = 3,
    .green = 4,
    .blue  = 5,
    .value = 2.4,
    .label = "label"
  };

  CU_ASSERT_NOT_EQUAL(qml_set_entry(qml, 6, &entry), 0);

  CU_ASSERT_EQUAL_FATAL(qml_set_entry(qml, 1, &entry), 0);

  CU_ASSERT_EQUAL(qml->entries[1].red, entry.red);
  CU_ASSERT_EQUAL(qml->entries[1].green, entry.green);
  CU_ASSERT_EQUAL(qml->entries[1].blue, entry.blue);
  CU_ASSERT_DOUBLE_EQUAL(qml->entries[1].value, entry.value, 1e-16);
  CU_ASSERT_STRING_EQUAL(qml->entries[1].label, entry.label);

  /* label is a copy, so pointers should differ */

  CU_ASSERT_NOT_EQUAL(qml->entries[1].label, entry.label);

  qml_destroy(qml);
}
