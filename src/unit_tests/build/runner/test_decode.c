/* AUTOGENERATED FILE. DO NOT EDIT. */

/*=======Test Runner Used To Run Each Test Below=====*/
#define RUN_TEST(TestFunc, TestLineNum) \
{ \
  Unity.CurrentTestName = #TestFunc; \
  Unity.CurrentTestLineNumber = TestLineNum; \
  Unity.NumberOfTests++; \
  if (TEST_PROTECT()) \
  { \
      setUp(); \
      TestFunc(); \
  } \
  if (TEST_PROTECT()) \
  { \
    tearDown(); \
  } \
  UnityConcludeTest(); \
}

/*=======Automagically Detected Files To Include=====*/
#ifdef __WIN32__
#define UNITY_INCLUDE_SETUP_STUBS
#endif
#include "unity.h"
#ifndef UNITY_EXCLUDE_SETJMP_H
#include <setjmp.h>
#endif
#include <stdio.h>
#include "../emulate/decode.h"

/*=======External Functions This Runner Calls=====*/
extern void setUp(void);
extern void tearDown(void);
extern void test_decodeDp_rotated_immediate(void);
extern void test_decodeDp_shifted_register(void);
extern void test_decodeSDT_invalid_register(void);
extern void test_decodeMul(void);
extern void test_decodeBrn(void);
extern void test_decodeBrn2(void);
extern void test_decodeBrn3(void);
extern void test_decodeHal(void);


/*=======Suite Setup=====*/
static void suite_setup(void)
{
#if defined(UNITY_WEAK_ATTRIBUTE) || defined(UNITY_WEAK_PRAGMA)
  suiteSetUp();
#endif
}

/*=======Suite Teardown=====*/
static int suite_teardown(int num_failures)
{
#if defined(UNITY_WEAK_ATTRIBUTE) || defined(UNITY_WEAK_PRAGMA)
  return suiteTearDown(num_failures);
#else
  return num_failures;
#endif
}

/*=======Test Reset Option=====*/
void resetTest(void);
void resetTest(void)
{
  tearDown();
  setUp();
}


/*=======MAIN=====*/
int main(void)
{
  suite_setup();
  UnityBegin("test_decode.c");
  RUN_TEST(test_decodeDp_rotated_immediate, 99);
  RUN_TEST(test_decodeDp_shifted_register, 121);
  RUN_TEST(test_decodeSDT_invalid_register, 145);
  RUN_TEST(test_decodeMul, 167);
  RUN_TEST(test_decodeBrn, 188);
  RUN_TEST(test_decodeBrn2, 203);
  RUN_TEST(test_decodeBrn3, 218);
  RUN_TEST(test_decodeHal, 233);

  return suite_teardown(UnityEnd());
}
