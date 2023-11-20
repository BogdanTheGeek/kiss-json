/*
 * File    : test.c
 * Created : 29/04/2023
 * Modified: 29/04/2023
 * Authors : Bogdan Ionescu
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "kJSON.h"

#define array_size(array) (sizeof(array) / sizeof(array[0]))

#define TEST(test) \
   if (!test)      \
   {               \
      result |= 1; \
   }

#define CHECK_JSON_GOOD(json, expected)                                 \
   if (strcmp(json.root, expected) != 0)                                \
   {                                                                    \
      printf("\n%s FAILED:\n", __func__);                               \
      printf("File: ./%s:%d\n", __FILE__, __LINE__);                    \
      printf("--------------------------------------------------\n");   \
      printf("Expected(%zu):\n%s\n\n", array_size(expected), expected); \
      printf("Actual(%zu):\n%s\n", json.size, json.root);               \
      printf("--------------------------------------------------\n");   \
      return false;                                                     \
   }

#define CHECK_JSON_BAD(json, expected)                                  \
   if (strcmp(json.root, expected) == 0)                                \
   {                                                                    \
      printf("\n%s didn't fail when it should have\n", __func__);       \
      printf("File: ./%s:%d\n", __FILE__, __LINE__);                    \
      printf("--------------------------------------------------\n");   \
      printf("Expected(%zu):\n%s\n\n", array_size(expected), expected); \
      printf("Actual(%zu):\n%s\n", json.size, json.root);               \
      printf("--------------------------------------------------\n");   \
      return false;                                                     \
   }                                                                    \
   if (json.truncated == false)                                         \
   {                                                                    \
      printf("\n%s: didn't truncate\n", __func__);                      \
      printf("File: ./%s:%d\n\n", __FILE__, __LINE__);                  \
      return false;                                                     \
   }

static bool kJSON_InsertNumber_PASS(void);
static bool kJSON_InsertNumber_FAIL(void);
static bool kJSON_InsertUnsignedNumber_PASS(void);
static bool kJSON_InsertUnsignedNumber_FAIL(void);
static bool kJSON_InsertFloat_PASS(void);
static bool kJSON_InsertFloat_FAIL(void);
static bool kJSON_InsertString_PASS(void);
static bool kJSON_InsertString_FAIL(void);
static bool kJSON_InsertNull_PASS(void);
static bool kJSON_InsertNull_FAIL(void);
static bool kJSON_InsertBoolean_PASS(void);
static bool kJSON_InsertBoolean_FAIL(void);
static bool kJSON_InsertArrayInt_PASS(void);
static bool kJSON_InsertArrayInt_FAIL(void);
static bool kJSON_InsertArrayFloat_PASS(void);
static bool kJSON_InsertArrayFloat_FAIL(void);
static bool kJSON_InsertArrayString_PASS(void);
static bool kJSON_InsertArrayString_FAIL(void);
static bool kJSON_InsertObject_PASS(void);
static bool kJSON_InsertObject_FAIL(void);

int main(void)
{

#if CONFIG_KJSON_SMALLEST
   printf("Testing CONFIG_KJSON_SMALLEST=1\n");
#else
   printf("Testing CONFIG_KJSON_SMALLEST=0\n");
#endif

   int result = 0;

   TEST(kJSON_InsertNumber_PASS());
   TEST(kJSON_InsertNumber_FAIL());
   TEST(kJSON_InsertUnsignedNumber_PASS());
   TEST(kJSON_InsertUnsignedNumber_FAIL());
   TEST(kJSON_InsertFloat_PASS());
   TEST(kJSON_InsertFloat_FAIL());
   TEST(kJSON_InsertString_PASS());
   TEST(kJSON_InsertString_FAIL());
   TEST(kJSON_InsertNull_PASS());
   TEST(kJSON_InsertNull_FAIL());
   TEST(kJSON_InsertBoolean_PASS());
   TEST(kJSON_InsertBoolean_FAIL());
   TEST(kJSON_InsertArrayInt_PASS());
   TEST(kJSON_InsertArrayInt_FAIL());
   TEST(kJSON_InsertArrayFloat_PASS());
   TEST(kJSON_InsertArrayFloat_FAIL());
   TEST(kJSON_InsertArrayString_PASS());
   TEST(kJSON_InsertArrayString_FAIL());
   TEST(kJSON_InsertObject_PASS());
   TEST(kJSON_InsertObject_FAIL());

   return result;
}

static bool kJSON_InsertNumber_PASS(void)
{
#if CONFIG_KJSON_SMALLEST
   const char expected[] = "{\"number\":-1234}";
#else
   const char expected[] = "{\n"
                           "\"number\":\t-1234\n"
                           "}";
#endif

   char root[sizeof(expected)] = {0};
   kjson_t json = KJSON_INITIALISE(root, sizeof(root));
   kjson_t *jsonHandle = &json;

   int number = -1234;

   kJSON_InitRoot(jsonHandle);
   kJSON_InsertNumber(jsonHandle, "number", number);
   kJSON_ExitRoot(jsonHandle);

   CHECK_JSON_GOOD(json, expected);

   return true;
}

static bool kJSON_InsertNumber_FAIL(void)
{
#if CONFIG_KJSON_SMALLEST
   const char expected[] = "{\"number\":-1234}";
#else
   const char expected[] = "{\n"
                           "\"number\":\t-1234\n"
                           "}";
#endif

   char root[sizeof(expected) - 1] = {0};
   kjson_t json = KJSON_INITIALISE(root, sizeof(root));
   kjson_t *jsonHandle = &json;

   int number = -1234;

   kJSON_InitRoot(jsonHandle);
   kJSON_InsertNumber(jsonHandle, "number", number);
   kJSON_ExitRoot(jsonHandle);

   CHECK_JSON_BAD(json, expected);

   return true;
}

static bool kJSON_InsertUnsignedNumber_PASS(void)
{
#if CONFIG_KJSON_SMALLEST
   const char expected[] = "{\"number\":1234}";
#else
   const char expected[] = "{\n"
                           "\"number\":\t1234\n"
                           "}";
#endif

   char root[sizeof(expected)] = {0};
   kjson_t json = KJSON_INITIALISE(root, sizeof(root));
   kjson_t *jsonHandle = &json;

   unsigned int number = 1234;

   kJSON_InitRoot(jsonHandle);
   kJSON_InsertUnsignedNumber(jsonHandle, "number", number);
   kJSON_ExitRoot(jsonHandle);

   CHECK_JSON_GOOD(json, expected);

   return true;
}

static bool kJSON_InsertUnsignedNumber_FAIL(void)
{
#if CONFIG_KJSON_SMALLEST
   const char expected[] = "{\"number\":1234}";
#else
   const char expected[] = "{\n"
                           "\"number\":\t1234\n"
                           "}";
#endif

   char root[sizeof(expected) - 1] = {0};
   kjson_t json = KJSON_INITIALISE(root, sizeof(root));
   kjson_t *jsonHandle = &json;

   unsigned int number = 1234;

   kJSON_InitRoot(jsonHandle);
   kJSON_InsertUnsignedNumber(jsonHandle, "number", number);
   kJSON_ExitRoot(jsonHandle);

   CHECK_JSON_BAD(json, expected);

   return true;
}

static bool kJSON_InsertFloat_PASS(void)
{
#if CONFIG_KJSON_SMALLEST
   const char expected[] = "{\"number\":-123.12}";
#else
   const char expected[] = "{\n"
                           "\"number\":\t-123.12\n"
                           "}";
#endif

   char root[sizeof(expected)] = {0};
   kjson_t json = KJSON_INITIALISE(root, sizeof(root));
   kjson_t *jsonHandle = &json;

   float number = -123.1234567890f;

   kJSON_InitRoot(jsonHandle);
   kJSON_InsertFloat(jsonHandle, "number", number, 2);
   kJSON_ExitRoot(jsonHandle);

   CHECK_JSON_GOOD(json, expected);

   return true;
}

static bool kJSON_InsertFloat_FAIL(void)
{
#if CONFIG_KJSON_SMALLEST
   const char expected[] = "{\"number\":-123.12}";
#else
   const char expected[] = "{\n"
                           "\"number\":\t-123.12\n"
                           "}";
#endif

   char root[sizeof(expected) - 1] = {0};
   kjson_t json = KJSON_INITIALISE(root, sizeof(root));
   kjson_t *jsonHandle = &json;

   float number = -123.1234567890f;

   kJSON_InitRoot(jsonHandle);
   kJSON_InsertFloat(jsonHandle, "number", number, 2);
   kJSON_ExitRoot(jsonHandle);

   CHECK_JSON_BAD(json, expected);

   return true;
}

static bool kJSON_InsertString_PASS(void)
{
#if CONFIG_KJSON_SMALLEST
   const char expected[] = "{\"string\":\"Hello World\"}";
#else
   const char expected[] = "{\n"
                           "\"string\":\t\"Hello World\"\n"
                           "}";
#endif

   char root[sizeof(expected)] = {0};
   kjson_t json = KJSON_INITIALISE(root, sizeof(root));
   kjson_t *jsonHandle = &json;
   const char *string = "Hello World";

   kJSON_InitRoot(jsonHandle);
   kJSON_InsertString(jsonHandle, "string", string);
   kJSON_ExitRoot(jsonHandle);

   CHECK_JSON_GOOD(json, expected);

   return true;
}

static bool kJSON_InsertString_FAIL(void)
{
#if CONFIG_KJSON_SMALLEST
   const char expected[] = "{\"string\":\"Hello World\"}";
#else
   const char expected[] = "{\n"
                           "\"string\":\t\"Hello World\"\n"
                           "}";
#endif

   char root[sizeof(expected) - 1] = {0};
   kjson_t json = KJSON_INITIALISE(root, sizeof(root));
   kjson_t *jsonHandle = &json;
   const char *string = "Hello World";

   kJSON_InitRoot(jsonHandle);
   kJSON_InsertString(jsonHandle, "string", string);
   kJSON_ExitRoot(jsonHandle);

   CHECK_JSON_BAD(json, expected);

   return true;
}

static bool kJSON_InsertNull_PASS(void)
{
#if CONFIG_KJSON_SMALLEST
   const char expected[] = "{\"null\":null}";
#else
   const char expected[] = "{\n"
                           "\"null\":\tnull\n"
                           "}";
#endif

   char root[sizeof(expected)] = {0};
   kjson_t json = KJSON_INITIALISE(root, sizeof(root));
   kjson_t *jsonHandle = &json;

   kJSON_InitRoot(jsonHandle);
   kJSON_InsertNull(jsonHandle, "null");
   kJSON_ExitRoot(jsonHandle);

   CHECK_JSON_GOOD(json, expected);

   return true;
}

static bool kJSON_InsertNull_FAIL(void)
{
#if CONFIG_KJSON_SMALLEST
   const char expected[] = "{\"null\":null}";
#else
   const char expected[] = "{\n"
                           "\"null\":\tnull\n"
                           "}";
#endif

   char root[sizeof(expected) - 1] = {0};
   kjson_t json = KJSON_INITIALISE(root, sizeof(root));
   kjson_t *jsonHandle = &json;

   kJSON_InitRoot(jsonHandle);
   kJSON_InsertNull(jsonHandle, "null");
   kJSON_ExitRoot(jsonHandle);

   CHECK_JSON_BAD(json, expected);

   return true;
}

static bool kJSON_InsertBoolean_PASS(void)
{
#if CONFIG_KJSON_SMALLEST
   const char expected[] = "{\"true\":true,\"false\":false}";
#else
   const char expected[] = "{\n"
                           "\"true\":\ttrue,\n"
                           "\"false\":\tfalse\n"
                           "}";
#endif

   char root[sizeof(expected)] = {0};
   kjson_t json = KJSON_INITIALISE(root, sizeof(root));
   kjson_t *jsonHandle = &json;

   kJSON_InitRoot(jsonHandle);
   kJSON_InsertBoolean(jsonHandle, "true", true);
   kJSON_InsertBoolean(jsonHandle, "false", false);
   kJSON_ExitRoot(jsonHandle);

   CHECK_JSON_GOOD(json, expected);

   return true;
}

static bool kJSON_InsertBoolean_FAIL(void)
{
#if CONFIG_KJSON_SMALLEST
   const char expected[] = "{\"true\":true,\"false\":false}";
#else
   const char expected[] = "{\n"
                           "\"true\":\ttrue,\n"
                           "\"false\":\tfalse\n"
                           "}";
#endif

   char root[sizeof(expected) - 1] = {0};
   kjson_t json = KJSON_INITIALISE(root, sizeof(root));
   kjson_t *jsonHandle = &json;

   kJSON_InitRoot(jsonHandle);
   kJSON_InsertBoolean(jsonHandle, "true", true);
   kJSON_InsertBoolean(jsonHandle, "false", false);
   kJSON_ExitRoot(jsonHandle);

   CHECK_JSON_BAD(json, expected);

   return true;
}

static bool kJSON_InsertArrayInt_PASS(void)
{
#if CONFIG_KJSON_SMALLEST
   const char expected[] = "{\"digits\":[0,1,2,3,4,5,6,7,8,null]}";
#else
   const char expected[] = "{\n"
                           "\"digits\":\t[0, 1, 2, 3, 4, 5, 6, 7, 8, null]\n"
                           "}";
#endif

   char root[sizeof(expected)] = {0};
   kjson_t json = KJSON_INITIALISE(root, sizeof(root));
   kjson_t *jsonHandle = &json;

   int digits[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, INT_MAX};

   kJSON_InitRoot(jsonHandle);
   kJSON_InsertArrayInt(jsonHandle, "digits", digits, array_size(digits));
   kJSON_ExitRoot(jsonHandle);

   CHECK_JSON_GOOD(json, expected);

   return true;
}

static bool kJSON_InsertArrayInt_FAIL(void)
{
#if CONFIG_KJSON_SMALLEST
   const char expected[] = "{\"digits\":[0,1,2,3,4,5,6,7,8,null]}";
#else
   const char expected[] = "{\n"
                           "\"digits\":\t[0, 1, 2, 3, 4, 5, 6, 7, 8, null]\n"
                           "}";
#endif

   char root[sizeof(expected) - 1] = {0};
   kjson_t json = KJSON_INITIALISE(root, sizeof(root));
   kjson_t *jsonHandle = &json;

   int digits[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, INT_MAX};

   kJSON_InitRoot(jsonHandle);
   kJSON_InsertArrayInt(jsonHandle, "digits", digits, array_size(digits));
   kJSON_ExitRoot(jsonHandle);

   CHECK_JSON_BAD(json, expected);

   return true;
}

static bool kJSON_InsertArrayFloat_PASS(void)
{
#if CONFIG_KJSON_SMALLEST
   const char expected[] = "{\"digits\":[0.0,1.1,2.3,3.3,4.4,5.5,6.6,7.7,8.8,null]}";
#else
   const char expected[] = "{\n"
                           "\"digits\":\t[0.0, 1.1, 2.3, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, null]\n"
                           "}";
#endif

   char root[sizeof(expected)] = {0};
   kjson_t json = KJSON_INITIALISE(root, sizeof(root));
   kjson_t *jsonHandle = &json;

   json.nullFloatValue = -99999.0f;

   float digits[] = {0.0f, 1.1222f, 2.29999f, 3.3f, 4.4f, 5.5f, 6.6f, 7.7f, 8.8f, -99999.0f};

   kJSON_InitRoot(jsonHandle);
   kJSON_InsertArrayFloat(jsonHandle, "digits", digits, array_size(digits), 1);
   kJSON_ExitRoot(jsonHandle);

   CHECK_JSON_GOOD(json, expected);

   return true;
}

static bool kJSON_InsertArrayFloat_FAIL(void)
{
#if CONFIG_KJSON_SMALLEST
   const char expected[] = "{\"digits\":[0.0,1.1,2.3,3.3,4.4,5.5,6.6,7.7,8.8,null]}";
#else
   const char expected[] = "{\n"
                           "\"digits\":\t[0.0, 1.1, 2.3, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, null]\n"
                           "}";
#endif

   char root[sizeof(expected) - 1] = {0};
   kjson_t json = KJSON_INITIALISE(root, sizeof(root));
   kjson_t *jsonHandle = &json;

   json.nullFloatValue = -99999.0f;

   float digits[] = {0.0f, 1.1222f, 2.29999f, 3.3f, 4.4f, 5.5f, 6.6f, 7.7f, 8.8f, -99999.0f};

   kJSON_InitRoot(jsonHandle);
   kJSON_InsertArrayFloat(jsonHandle, "digits", digits, array_size(digits), 1);
   kJSON_ExitRoot(jsonHandle);

   CHECK_JSON_BAD(json, expected);

   return true;
}

static bool kJSON_InsertArrayString_PASS(void)
{
#if CONFIG_KJSON_SMALLEST
   const char expected[] = "{\"digits\":[\"0\",\"1\",\"2\",\"3\",\"4\",\"5\",\"6\",\"7\",\"8\",null]}";
#else
   const char expected[] = "{\n"
                           "\"digits\":\t[\"0\", \"1\", \"2\", \"3\", \"4\", \"5\", \"6\", \"7\", \"8\", null]\n"
                           "}";
#endif

   char root[sizeof(expected)] = {0};
   kjson_t json = KJSON_INITIALISE(root, sizeof(root));
   kjson_t *jsonHandle = &json;

   const char *digits[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", NULL};

   kJSON_InitRoot(jsonHandle);
   kJSON_InsertArrayString(jsonHandle, "digits", digits, array_size(digits));
   kJSON_ExitRoot(jsonHandle);

   CHECK_JSON_GOOD(json, expected);

   return true;
}

static bool kJSON_InsertArrayString_FAIL(void)
{
#if CONFIG_KJSON_SMALLEST
   const char expected[] = "{\"digits\":[\"0\",\"1\",\"2\",\"3\",\"4\",\"5\",\"6\",\"7\",\"8\",null]}";
#else
   const char expected[] = "{\n"
                           "\"digits\":\t[\"0\", \"1\", \"2\", \"3\", \"4\", \"5\", \"6\", \"7\", \"8\", null]\n"
                           "}";
#endif

   char root[sizeof(expected) - 1] = {0};
   kjson_t json = KJSON_INITIALISE(root, sizeof(root));
   kjson_t *jsonHandle = &json;

   const char *digits[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", NULL};

   kJSON_InitRoot(jsonHandle);
   kJSON_InsertArrayString(jsonHandle, "digits", digits, array_size(digits));
   kJSON_ExitRoot(jsonHandle);

   CHECK_JSON_BAD(json, expected);

   return true;
}

static bool kJSON_InsertObject_PASS(void)
{
#if CONFIG_KJSON_SMALLEST
   const char expected[] = "{\"object\":{\"key\":\"value\"}}";
#else
   const char expected[] = "{\n"
                           "\"object\":\t{\n"
                           "\t\"key\":\t\"value\"\n"
                           "}\n"
                           "}";
#endif

   char root[sizeof(expected)] = {0};
   kjson_t json = KJSON_INITIALISE(root, sizeof(root));
   kjson_t *jsonHandle = &json;

   kJSON_InitRoot(jsonHandle);

   kJSON_EnterObject(jsonHandle, "object");
   {
      kJSON_InsertString(jsonHandle, "key", "value");
   }
   kJSON_ExitObject(jsonHandle);

   kJSON_ExitRoot(jsonHandle);

   CHECK_JSON_GOOD(json, expected);

   return true;
}

static bool kJSON_InsertObject_FAIL(void)
{
#if CONFIG_KJSON_SMALLEST
   const char expected[] = "{\"object\":{\"key\":\"value\"}}";
#else
   const char expected[] = "{\n"
                           "\"object\":\t{\n"
                           "\t\"key\": \"value\"\n"
                           "}\n"
                           "}";
#endif

   char root[sizeof(expected) - 1] = {0};
   kjson_t json = KJSON_INITIALISE(root, sizeof(root));
   kjson_t *jsonHandle = &json;

   kJSON_InitRoot(jsonHandle);

   kJSON_EnterObject(jsonHandle, "object");
   {
      kJSON_InsertString(jsonHandle, "key", "value");
   }
   kJSON_ExitObject(jsonHandle);

   kJSON_ExitRoot(jsonHandle);

   CHECK_JSON_BAD(json, expected);

   return true;
}
