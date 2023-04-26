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
#define NULL_VALUE    ("null")

#if CONFIG_KJSON_SMALLEST
#define STRING               ("\"%s\":\"%s\",")
#define NUMBER               ("\"%s\":%d,")
#define UNSIGNED             ("\"%s\":%u,")
#define BOOLEAN              ("\"%s\":%s,")
#define ARRAY_KEY            ("\"%s\":[")
#define ARRAY_VALUE_STRING   ("\"%s\",")
#define ARRAY_VALUE_NULL     ("null,")
#define ARRAY_VALUE_NUMBER   ("%d,")
#define ARRAY_VALUE_UNSIGNED ("%u,")
#define ARRAY_END            ("],")
#define ARRAY_TRIM           (char_size(","))
#define OBJECT_KEY           ("\"%s\":{")
#define OBJECT_END           ("},")
#else
#define STRING               ("\"%s\":\t\"%s\",")
#define NUMBER               ("\"%s\":\t%d,")
#define UNSIGNED             ("\"%s\":\t%u,")
#define BOOLEAN              ("\"%s\":\t%s,")
#define ARRAY_KEY            ("\"%s\":\t[")
#define ARRAY_VALUE_STRING   ("\"%s\", ")
#define ARRAY_VALUE_NULL     ("null, ")
#define ARRAY_VALUE_NUMBER   ("%d, ")
#define ARRAY_VALUE_UNSIGNED ("%u, ")
#define ARRAY_END            ("],")
#define ARRAY_TRIM           (char_size(", "))
#define OBJECT_KEY           ("\"%s\":\t{")
#define OBJECT_END           ("},")
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

typedef enum
{
   eUnsigned = 0,
   eSigned = 1,
} NumberType_e;

//------------------------------------------------------------------------------
// Module static variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Module static function prototypes
//------------------------------------------------------------------------------
static int InsertString(char *const string, const char *const key, const char *const value);
static int InsertNumber(char *const string, const char *const key, const int value);
static int InsertUnsignedNumber(char *const string, const char *const key, const unsigned int value);
static int InsertBoolean(char *const string, const char *const key, bool value);
static int InsertNull(char *const string, const char *const key);

static int InsertArrayNumber(char *const string, const char *const key, const void *const array, const size_t size, const NumberType_e type, void *const nullValue);
static int InsertArrayString(char *const string, const char *const key, const char *const *const array, const size_t size);

static int InitRoot(char *const string);
static int Trim(char *const string);
static int ExitRoot(char *const string);
static int EnterObject(char *const string, const char *const key);
static int ExitObject(char *const string);
static int InsertDepth(char *const string, const char *const newLine, const int depth);
static void StartEntry(kjson_t *const jsonHandle);

static int GetNumDigits(const void *const value, const NumberType_e type);
static bool StringFits(kjson_t *const jsonHandle, const char *const key, const char *const value);
static bool NumberFits(kjson_t *const jsonHandle, const char *const key, const void *const value, const NumberType_e type);
static bool BooleanFits(kjson_t *const jsonHandle, const char *const key, bool value);
static bool NullFits(kjson_t *const jsonHandle, const char *const key);
static bool ArrayNumberFits(kjson_t *const jsonHandle, const char *const key, const void *const array, const size_t size, const NumberType_e type);
static bool ArrayStringFits(kjson_t *const jsonHandle, const char *const key, const char *const *const array, const size_t size);
static bool ObjectFits(kjson_t *const jsonHandle, const char *const key);

//------------------------------------------------------------------------------
// Module externally exported functions
//------------------------------------------------------------------------------
void kJSON_InsertString(kjson_t *const jsonHandle, const char *const key, const char *const value)
{
   if (NULL == value)
   {
      kJSON_InsertNull(jsonHandle, key);
   }
   else
   {
      if (StringFits(jsonHandle, key, value))
      {
         StartEntry(jsonHandle);
         size_t bytes = InsertString(jsonHandle->tail, key, value);
         jsonHandle->size += bytes;
         jsonHandle->tail += bytes;
      }
      else
      {
         jsonHandle->truncated = true;
      }
   }
}

void kJSON_InsertNumber(kjson_t *const jsonHandle, const char *const key, const int value)
{
   if (NumberFits(jsonHandle, key, &value, eSigned))
   {
      StartEntry(jsonHandle);
      size_t bytes = InsertNumber(jsonHandle->tail, key, value);
      jsonHandle->size += bytes;
      jsonHandle->tail += bytes;
   }
   else
   {
      jsonHandle->truncated = true;
   }
}

void kJSON_InsertUnsignedNumber(kjson_t *const jsonHandle, const char *const key, const unsigned int value)
{
   if (NumberFits(jsonHandle, key, &value, eUnsigned))
   {
      StartEntry(jsonHandle);
      size_t bytes = InsertUnsignedNumber(jsonHandle->tail, key, value);
      jsonHandle->size += bytes;
      jsonHandle->tail += bytes;
   }
   else
   {
      jsonHandle->truncated = true;
   }
}

void kJSON_InsertBoolean(kjson_t *const jsonHandle, const char *const key, bool value)
{
   if (BooleanFits(jsonHandle, key, value))
   {
      StartEntry(jsonHandle);
      size_t bytes = InsertBoolean(jsonHandle->tail, key, value);
      jsonHandle->size += bytes;
      jsonHandle->tail += bytes;
   }
   else
   {
      jsonHandle->truncated = true;
   }
}

void kJSON_InsertNull(kjson_t *const jsonHandle, const char *const key)
{
   if (NullFits(jsonHandle, key))
   {
      StartEntry(jsonHandle);
      size_t bytes = InsertNull(jsonHandle->tail, key);
      jsonHandle->size += bytes;
      jsonHandle->tail += bytes;
   }
   else
   {
      jsonHandle->truncated = true;
   }
}

void kJSON_InsertArrayInt(kjson_t *const jsonHandle, const char *const key, const int *const array, const size_t size)
{
   if (ArrayNumberFits(jsonHandle, key, array, size, eSigned))
   {
      StartEntry(jsonHandle);
      size_t bytes = InsertArrayNumber(jsonHandle->tail, key, array, size, eSigned, &jsonHandle->nullIntValue);
      jsonHandle->size += bytes;
      jsonHandle->tail += bytes;
   }
   else
   {
      jsonHandle->truncated = true;
   }
}

void kJSON_InsertArrayUInt(kjson_t *const jsonHandle, const char *const key, const unsigned int *const array, const size_t size)
{
   if (ArrayNumberFits(jsonHandle, key, array, size, eUnsigned))
   {
      StartEntry(jsonHandle);
      size_t bytes = InsertArrayNumber(jsonHandle->tail, key, array, size, eUnsigned, &jsonHandle->nullUIntValue);
      jsonHandle->size += bytes;
      jsonHandle->tail += bytes;
   }
   else
   {
      jsonHandle->truncated = true;
   }
}

void kJSON_InsertArrayString(kjson_t *const jsonHandle, const char *const key, const char *const *const array, const size_t size)
{
   if (ArrayStringFits(jsonHandle, key, array, size))
   {
      StartEntry(jsonHandle);
      size_t bytes = InsertArrayString(jsonHandle->tail, key, array, size);
      jsonHandle->size += bytes;
      jsonHandle->tail += bytes;
   }
   else
   {
      jsonHandle->truncated = true;
   }
}

void kJSON_InitRoot(kjson_t *const jsonHandle)
{
   if (!jsonHandle->newLine || CONFIG_KJSON_SMALLEST)
   {
      jsonHandle->newLine = "";
   }
   size_t bytes = InitRoot(jsonHandle->tail);
   jsonHandle->size += bytes;
   jsonHandle->tail += bytes;
   // Account for closing brace
   jsonHandle->size += strlen(jsonHandle->newLine) + (char_size(OBJECT_END) - 1);
}

void kJSON_ExitRoot(kjson_t *const jsonHandle)
{
   int trim = Trim(jsonHandle->tail);
   jsonHandle->tail += trim;
   StartEntry(jsonHandle);
   int bytes = ExitRoot(jsonHandle->tail);
   jsonHandle->tail += bytes;
   jsonHandle->size += trim;
   jsonHandle->size -= strlen(jsonHandle->newLine);
}

void kJSON_EnterObject(kjson_t *const jsonHandle, const char *const key)
{
   if (ObjectFits(jsonHandle, key))
   {
      StartEntry(jsonHandle);
      size_t bytes = EnterObject(jsonHandle->tail, key);
      jsonHandle->size += bytes;
      jsonHandle->tail += bytes;
      jsonHandle->size += strlen(jsonHandle->newLine) + jsonHandle->depth + (char_size(OBJECT_END) - 1);
#if !CONFIG_KJSON_SMALLEST
      jsonHandle->depth++;
#endif
   }
   else
   {
      jsonHandle->truncated = true;
   }
}

void kJSON_ExitObject(kjson_t *const jsonHandle)
{
   int bytes = Trim(jsonHandle->tail);
   jsonHandle->tail += bytes;
#if !CONFIG_KJSON_SMALLEST
   jsonHandle->depth--;
#endif
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
   char *const start = string;
   char *end = start;
   end += sprintf(end, STRING, key, value);
   return end - start;
}

static int InsertNumber(char *const string, const char *const key, const int value)
{
   char *const start = string;
   char *end = start;
   end += sprintf(end, NUMBER, key, value);
   return end - start;
}

static int InsertUnsignedNumber(char *const string, const char *const key, const unsigned int value)
{
   char *const start = string;
   char *end = start;
   end += sprintf(end, UNSIGNED, key, value);
   return end - start;
}

static int InsertBoolean(char *const string, const char *const key, bool value)
{
   char *const start = string;
   char *end = start;
   end += sprintf(end, BOOLEAN, key, value ? BOOLEAN_TRUE : BOOLEAN_FALSE);
   return end - start;
}

static int InsertNull(char *const string, const char *const key)
{
   char *const start = string;
   char *end = start;
   end += sprintf(end, BOOLEAN, key, NULL_VALUE);
   return end - start;
}

static int InsertArrayNumber(char *const string, const char *const key, const void *const array, const size_t size, const NumberType_e type, void *const nullValue)
{
   char *const start = string;
   char *end = start;
   end += sprintf(end, ARRAY_KEY, key);
   for (size_t i = 0; i < size; i++)
   {
      if (((int *)array)[i] == *(int *)nullValue)
      {
         end += sprintf(end, ARRAY_VALUE_NULL);
      }
      else
      {
         if (eSigned == type)
         {
            end += sprintf(end, ARRAY_VALUE_NUMBER, ((int *)array)[i]);
         }
         else
         {
            end += sprintf(end, ARRAY_VALUE_UNSIGNED, ((unsigned int *)array)[i]);
         }
      }
   }
   end -= ARRAY_TRIM;
   end += sprintf(end, ARRAY_END);
   return end - start;
}

static int InsertArrayString(char *const string, const char *const key, const char *const *const array, const size_t size)
{
   char *const start = string;
   char *end = start;
   end += sprintf(end, ARRAY_KEY, key);
   for (size_t i = 0; i < size; i++)
   {
      end += sprintf(end, array[i] ? ARRAY_VALUE_STRING : ARRAY_VALUE_NULL, array[i]);
   }
   end -= ARRAY_TRIM;
   end += sprintf(end, ARRAY_END);
   return end - start;
}

static int InitRoot(char *const string)
{
   char *const start = string;
   char *end = start;
   end += sprintf(end, "{");
   return end - start;
}

static int Trim(char *const string)
{
   char *const start = string;
   char *end = start - 1;
   if (',' == *end++)
   {
      end = start - char_size(",");
   }
   return end - start;
}

static int ExitRoot(char *const string)
{
   char *const start = string;
   char *end = start;
   *(end++) = '}';
   *(end++) = '\0';
   return end - start;
}

static int EnterObject(char *const string, const char *const key)
{
   char *const start = string;
   char *end = start;
   end += sprintf(end, OBJECT_KEY, key);
   return end - start;
}

static int ExitObject(char *const string)
{
   char *const start = string;
   char *end = start;
   end += sprintf(end, OBJECT_END);
   return end - start;
}

static int InsertDepth(char *const string, const char *const newLine, const int depth)
{
#if CONFIG_KJSON_SMALLEST
   unused(string);
   unused(newLine);
   unused(depth);
   return 0;
#else
   char *const start = string;
   char *end = start;
   const size_t newLineLength = strlen(newLine);
   for (size_t i = 0; i < newLineLength; i++)
   {
      *(end++) = newLine[i];
   }
   for (size_t i = 0; i < depth; i++)
   {
      end += sprintf(end, "\t");
   }
   return end - start;
#endif // CONFIG_KJSON_SMALLEST
}

static void StartEntry(kjson_t *const jsonHandle)
{
   const size_t bytes = InsertDepth(jsonHandle->tail, jsonHandle->newLine, jsonHandle->depth);
   jsonHandle->size += bytes;
   jsonHandle->tail += bytes;
}

static int GetNumDigits(const void *const value, const NumberType_e type)
{
   int count = 1;
   long long num = 0;
   if (eSigned == type)
   {
      num = *(int *)value;
   }
   else
   {
      num = *(unsigned int *)value;
   }
   if (num)
   {
      count = 0;
      while (0 != num)
      {
         num /= 10;
         count++;
      }
      if ((eSigned == type) && (*(int *)value < 0))
      {
         count++;
      }
   }
   return count;
}

static bool StringFits(kjson_t *const jsonHandle, const char *const key, const char *const value)
{
   const size_t size = strlen(jsonHandle->newLine) + jsonHandle->depth + strlen(key) + strlen(value) + char_size(STRING) - char_size("%s") - char_size("%s");
   return (jsonHandle->size + size <= jsonHandle->rootSize);
}

static bool NumberFits(kjson_t *const jsonHandle, const char *const key, const void *const value, const NumberType_e type)
{
   const size_t valueSize = GetNumDigits(value, type);
   const size_t size = strlen(jsonHandle->newLine) + jsonHandle->depth + strlen(key) + valueSize + char_size(NUMBER) - char_size("%s") - char_size("%d");
   return (jsonHandle->size + size <= jsonHandle->rootSize);
}

static bool BooleanFits(kjson_t *const jsonHandle, const char *const key, bool value)
{
   const size_t valueSize = strlen(value ? BOOLEAN_TRUE : BOOLEAN_FALSE);
   const size_t size = strlen(jsonHandle->newLine) + jsonHandle->depth + strlen(key) + valueSize + char_size(BOOLEAN) - char_size("%s") - char_size("%s");
   return (jsonHandle->size + size <= jsonHandle->rootSize);
}

static bool NullFits(kjson_t *const jsonHandle, const char *const key)
{
   const size_t valueSize = char_size(NULL_VALUE);
   const size_t size = strlen(jsonHandle->newLine) + jsonHandle->depth + strlen(key) + valueSize + char_size(BOOLEAN) - char_size("%s") - char_size("%s");
   return (jsonHandle->size + size <= jsonHandle->rootSize);
}

static bool ArrayNumberFits(kjson_t *const jsonHandle, const char *const key, const void *const array, const size_t size, const NumberType_e type)
{
   size_t total = strlen(jsonHandle->newLine) + jsonHandle->depth + strlen(key) + char_size(ARRAY_KEY) - char_size("%s");
   for (size_t i = 0; i < size; i++)
   {
      size_t valueSize = 0;
      if (eSigned == type)
      {
         const int num = *((int *)array + i);
         valueSize = GetNumDigits(&num, type);
      }
      else
      {
         const unsigned int num = *((unsigned int *)array + i);
         valueSize = GetNumDigits(&num, type);
      }
      total += valueSize + char_size(ARRAY_VALUE_NUMBER) - char_size("%d");
   }
   total -= ARRAY_TRIM;
   total += char_size(ARRAY_END);
   return (jsonHandle->size + total <= jsonHandle->rootSize);
}

static bool ArrayStringFits(kjson_t *const jsonHandle, const char *const key, const char *const *const array, const size_t size)
{
   size_t total = strlen(jsonHandle->newLine) + jsonHandle->depth + strlen(key) + char_size(ARRAY_KEY) - char_size("%s");
   for (size_t i = 0; i < size; i++)
   {
      size_t valueSize = char_size(NULL_VALUE);
      if (array[i])
      {
         valueSize = strlen(array[i]);
      }
      total += valueSize + char_size(ARRAY_VALUE_STRING) - char_size("%s");
   }
   total -= ARRAY_TRIM;
   total += char_size(ARRAY_END);
   return (jsonHandle->size + total <= jsonHandle->rootSize);
}

static bool ObjectFits(kjson_t *const jsonHandle, const char *const key)
{
   size_t size = strlen(jsonHandle->newLine) + jsonHandle->depth + strlen(key) + char_size(OBJECT_KEY) - char_size("%s");
   size += strlen(jsonHandle->newLine) + jsonHandle->depth + (char_size(OBJECT_END) - 1); // Closing bracket
   return (jsonHandle->size + size <= jsonHandle->rootSize);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
