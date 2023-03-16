//------------------------------------------------------------------------------
//       Filename: kJSON.c
//------------------------------------------------------------------------------
//       Bogdan Ionescu (c) 2022
//------------------------------------------------------------------------------
//       Purpose : Implements the kJSON API
//------------------------------------------------------------------------------
//       Notes : None
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Module includes
//------------------------------------------------------------------------------
#include "kJSON.h"

#include <stdio.h>
#include <string.h>

//------------------------------------------------------------------------------
// Module constant defines
//------------------------------------------------------------------------------
#define array_size(array) (sizeof(array) / sizeof(array[0]))
#define char_size(string) (sizeof(string) - 1)
#define unused(x)         (void)(x)

#define BOOLEAN_TRUE  ("true")
#define BOOLEAN_FALSE ("false")

#if CONFIG_KJSON_SMALLEST
#define STRING             ("\"%s\":\"%s\",")
#define NUMBER             ("\"%s\":%d,")
#define BOOLEAN            ("\"%s\":%s,")
#define ARRAY_KEY          ("\"%s\":[")
#define ARRAY_VALUE_STRING ("\"%s\",")
#define ARRAY_VALUE_NUMBER ("%d,")
#define ARRAY_END          ("],")
#define ARRAY_TRIM         (char_size(","))
#define OBJECT_KEY         ("\"%s\":{")
#define OBJECT_END         ("},")
#else
#define STRING             ("\"%s\": \"%s\",")
#define NUMBER             ("\"%s\": %d,")
#define BOOLEAN            ("\"%s\": %s,")
#define ARRAY_KEY          ("\"%s\": [ ")
#define ARRAY_VALUE_STRING ("\"%s\", ")
#define ARRAY_VALUE_NUMBER ("%d, ")
#define ARRAY_END          (" ],")
#define ARRAY_TRIM         (char_size(", "))
#define OBJECT_KEY         ("\"%s\": {")
#define OBJECT_END         ("},")
#endif // CONFIG_KJSON_SMALLEST

//------------------------------------------------------------------------------
// External variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// External functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Module type definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Module static variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Module static function prototypes
//------------------------------------------------------------------------------
static int InsertString(char *const string, const char *const key, const char *const value);

static int InsertNumber(char *const string, const char *const key, const int value);

static int InsertBoolean(char *const string, const char *const key, bool value);

static int InsertArrayInt(char *const string, const char *const key, const int *const array, const int size);

static int InsertArrayString(char *const string, const char *const key, char **array, const int size);

static int InitRoot(char *const string);

static int Trim(char *const string);

static int ExitRoot(char *const string);

static int EnterObject(char *const string, const char *const key);

static int ExitObject(char *const string);

static int InsertDepth(char *const string, const char *const newLine, int depth);

static void StartEntry(json_t *const jsonHandle);

static int GetNumDigits(const int value);

static bool StringFits(json_t *const jsonHandle, const char *const key, const char *const value);

static bool IntFits(json_t *const jsonHandle, const char *const key, const int value);

static bool BooleanFits(json_t *const jsonHandle, const char *const key, bool value);

static bool ArrayIntFits(json_t *const jsonHandle, const char *const key, const int *const array, const int size);

static bool ArrayStringFits(json_t *const jsonHandle, const char *const key, char **array, const int size);

static bool ObjectFits(json_t *const jsonHandle, const char *const key);

//------------------------------------------------------------------------------
// Module externally exported functions
//------------------------------------------------------------------------------
void kJSON_InsertString(json_t *const jsonHandle, const char *const key, const char *const value)
{
   if (StringFits(jsonHandle, key, value))
   {
      StartEntry(jsonHandle);
      size_t bytes = InsertString(jsonHandle->tail, key, value);
      jsonHandle->size += bytes;
      jsonHandle->tail += bytes;
   }
}

void kJSON_InsertNumber(json_t *const jsonHandle, const char *const key, const int value)
{
   if (IntFits(jsonHandle, key, value))
   {
      StartEntry(jsonHandle);
      size_t bytes = InsertNumber(jsonHandle->tail, key, value);
      jsonHandle->size += bytes;
      jsonHandle->tail += bytes;
   }
}

void kJSON_InsertBoolean(json_t *const jsonHandle, const char *const key, bool value)
{
   if (BooleanFits(jsonHandle, key, value))
   {
      StartEntry(jsonHandle);
      size_t bytes = InsertBoolean(jsonHandle->tail, key, value);
      jsonHandle->size += bytes;
      jsonHandle->tail += bytes;
   }
}

void kJSON_InsertArrayInt(json_t *const jsonHandle, const char *const key, const int *const array, const int size)
{
   if (ArrayIntFits(jsonHandle, key, array, size))
   {
      StartEntry(jsonHandle);
      size_t bytes = InsertArrayInt(jsonHandle->tail, key, array, size);
      jsonHandle->size += bytes;
      jsonHandle->tail += bytes;
   }
}

void kJSON_InsertArrayString(json_t *const jsonHandle, const char *const key, char **array, const int size)
{
   if (ArrayStringFits(jsonHandle, key, array, size))
   {
      StartEntry(jsonHandle);
      size_t bytes = InsertArrayString(jsonHandle->tail, key, array, size);
      jsonHandle->size += bytes;
      jsonHandle->tail += bytes;
   }
}

void kJSON_InitRoot(json_t *const jsonHandle)
{
   if (!jsonHandle->newLine)
   {
      jsonHandle->newLine = "";
   }
   size_t bytes = InitRoot(jsonHandle->tail);
   jsonHandle->size += bytes;
   jsonHandle->tail += bytes;
   // Account for closing brace
   jsonHandle->size += strlen(jsonHandle->newLine) + (char_size(OBJECT_END) - 1);
}

void kJSON_ExitRoot(json_t *const jsonHandle)
{
   int bytes = Trim(jsonHandle->tail);
   jsonHandle->tail += bytes;
   StartEntry(jsonHandle);
   bytes = ExitRoot(jsonHandle->tail);
   jsonHandle->tail += bytes;
   jsonHandle->size -= 1;
}

void kJSON_EnterObject(json_t *const jsonHandle, const char *const key)
{
   if (ObjectFits(jsonHandle, key))
   {
      StartEntry(jsonHandle);
      size_t bytes = EnterObject(jsonHandle->tail, key);
      jsonHandle->size += bytes;
      jsonHandle->tail += bytes;
      jsonHandle->size += strlen(jsonHandle->newLine) + jsonHandle->depth + (char_size(OBJECT_END) - 1);
      jsonHandle->depth++;
   }
}

void kJSON_ExitObject(json_t *const jsonHandle)
{
   int bytes = Trim(jsonHandle->tail);
   jsonHandle->tail += bytes;
   jsonHandle->depth--;
   StartEntry(jsonHandle);
   bytes = ExitObject(jsonHandle->tail);
   jsonHandle->tail += bytes;
   jsonHandle->size -= strlen(jsonHandle->newLine) + jsonHandle->depth;
}

//------------------------------------------------------------------------------
// Module static functions
//------------------------------------------------------------------------------
static int InsertString(char *const string, const char *const key, const char *const value)
{
   char *start = string;
   char *end = start;
   end += sprintf(end, STRING, key, value);
   return end - start;
}

static int InsertNumber(char *const string, const char *const key, const int value)
{
   char *start = string;
   char *end = start;
   end += sprintf(end, NUMBER, key, value);
   return end - start;
}

static int InsertBoolean(char *const string, const char *const key, bool value)
{
   char *start = string;
   char *end = start;
   end += sprintf(end, BOOLEAN, key, value ? BOOLEAN_TRUE : BOOLEAN_FALSE);
   return end - start;
}

static int InsertArrayInt(char *const string, const char *const key, const int *const array, const int size)
{
   char *start = string;
   char *end = start;
   end += sprintf(end, ARRAY_KEY, key);
   for (int i = 0; i < size; i++)
   {
      end += sprintf(end, ARRAY_VALUE_NUMBER, array[i]);
   }
   end -= ARRAY_TRIM;
   end += sprintf(end, ARRAY_END);
   return end - start;
}

static int InsertArrayString(char *const string, const char *const key, char **array, const int size)
{
   char *start = string;
   char *end = start;
   end += sprintf(end, ARRAY_KEY, key);
   for (int i = 0; i < size; i++)
   {
      end += sprintf(end, ARRAY_VALUE_STRING, array[i]);
   }
   end -= ARRAY_TRIM;
   end += sprintf(end, ARRAY_END);
   return end - start;
}

static int InitRoot(char *const string)
{
   char *start = string;
   char *end = start;
   end += sprintf(end, "{");
   return end - start;
}

static int Trim(char *const string)
{
   char *start = string;
   char *end = start - 1;
   if (',' == *end++)
   {
      end = start - char_size(",");
   }
   return end - start;
}

static int ExitRoot(char *const string)
{
   char *start = string;
   char *end = start;
   *(end++) = '}';
   *(end++) = '\0';
   return end - start;
}

static int EnterObject(char *const string, const char *const key)
{
   char *start = string;
   char *end = start;
   end += sprintf(end, OBJECT_KEY, key);
   return end - start;
}

static int ExitObject(char *const string)
{
   char *start = string;
   char *end = start;
   end += sprintf(end, OBJECT_END);
   return end - start;
}

static int InsertDepth(char *const string, const char *const newLine, int depth)
{
#if CONFIG_KJSON_SMALLEST
   unused(string);
   unused(newLine);
   unused(depth);
   return 0;
#else
   char *start = string;
   char *end = start;
   int newLineLength = strlen(newLine);
   for (int i = 0; i < newLineLength; i++)
   {
      *(end++) = newLine[i];
   }
   for (int i = 0; i < depth; i++)
   {
      end += sprintf(end, "\t");
   }
   return end - start;
#endif // CONFIG_KJSON_SMALLEST
}

static void StartEntry(json_t *const jsonHandle)
{
   size_t bytes = InsertDepth(jsonHandle->tail, jsonHandle->newLine, jsonHandle->depth);
   jsonHandle->size += bytes;
   jsonHandle->tail += bytes;
}

static int GetNumDigits(const int value)
{
   int count = 1;
   int num = value;
   if (num)
   {
      count = 0;
      while (0 != num)
      {
         num /= 10;
         count++;
      }
      if (value < 0)
      {
         count++;
      }
   }
   return count;
}

static bool StringFits(json_t *const jsonHandle, const char *const key, const char *const value)
{
   size_t size = strlen(jsonHandle->newLine) + jsonHandle->depth + strlen(key) + strlen(value) + char_size(STRING) - char_size("%s") - char_size("%s");
   return (jsonHandle->size + size < jsonHandle->rootSize);
}

static bool IntFits(json_t *const jsonHandle, const char *const key, const int value)
{
   size_t valueSize = GetNumDigits(value);
   size_t size = strlen(jsonHandle->newLine) + jsonHandle->depth + strlen(key) + valueSize + char_size(NUMBER) - char_size("%s") - char_size("%d");
   return (jsonHandle->size + size < jsonHandle->rootSize);
}

static bool BooleanFits(json_t *const jsonHandle, const char *const key, bool value)
{
   size_t valueSize = strlen(value ? BOOLEAN_TRUE : BOOLEAN_FALSE);
   size_t size = strlen(jsonHandle->newLine) + jsonHandle->depth + strlen(key) + valueSize + char_size(BOOLEAN) - char_size("%s") - char_size("%s");
   return (jsonHandle->size + size < jsonHandle->rootSize);
}

static bool ArrayIntFits(json_t *const jsonHandle, const char *const key, const int *const array, const int size)
{
   size_t total = strlen(jsonHandle->newLine) + jsonHandle->depth + strlen(key) + char_size(ARRAY_KEY) - char_size("%s");
   for (int i = 0; i < size; i++)
   {
      size_t valueSize = GetNumDigits(array[i]);
      total += valueSize + char_size(ARRAY_VALUE_NUMBER) - char_size("%d");
   }
   total -= ARRAY_TRIM;
   total += char_size(ARRAY_END);
   return (jsonHandle->size + total < jsonHandle->rootSize);
}

static bool ArrayStringFits(json_t *const jsonHandle, const char *const key, char **array, const int size)
{
   size_t total = strlen(jsonHandle->newLine) + jsonHandle->depth + strlen(key) + char_size(ARRAY_KEY) - char_size("%s");
   for (int i = 0; i < size; i++)
   {
      size_t valueSize = strlen(array[i]);
      total += valueSize + char_size(ARRAY_VALUE_STRING) - char_size("%s");
   }
   total -= ARRAY_TRIM;
   total += char_size(ARRAY_END);
   return (jsonHandle->size + total < jsonHandle->rootSize);
}

static bool ObjectFits(json_t *const jsonHandle, const char *const key)
{
   size_t size = strlen(jsonHandle->newLine) + jsonHandle->depth + strlen(key) + char_size(OBJECT_KEY) - char_size("%s");
   size += strlen(jsonHandle->newLine) + jsonHandle->depth + (char_size(OBJECT_END) - 1); // Closing bracket
   return (jsonHandle->size + size < jsonHandle->rootSize);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
