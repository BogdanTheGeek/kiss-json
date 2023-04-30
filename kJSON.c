//------------------------------------------------------------------------------
//       Filename: kJSON.c
//------------------------------------------------------------------------------
//       Bogdan Ionescu (c) 2022
//------------------------------------------------------------------------------
//       Purpose : Implements the kJSON API
//------------------------------------------------------------------------------
//       Version : 1.2.1
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
#define array_size(array) (sizeof((array)) / sizeof((array)[0]))
#define char_size(string) (sizeof((string)) - 1)
#define unused(x)         (void)(x)

#define BOOLEAN_TRUE  ("true")
#define BOOLEAN_FALSE ("false")
#define NULL_VALUE    ("null")

#if CONFIG_KJSON_SMALLEST
#define STRING               ("\"%s\":\"%s\",")
#define NUMBER               ("\"%s\":%d,")
#define UNSIGNED             ("\"%s\":%u,")
#define FLOAT                ("\"%s\":%.*f,")
#define BOOLEAN              ("\"%s\":%s,")
#define ARRAY_KEY            ("\"%s\":[")
#define ARRAY_VALUE_STRING   ("\"%s\",")
#define ARRAY_VALUE_NULL     ("null,")
#define ARRAY_VALUE_NUMBER   ("%d,")
#define ARRAY_VALUE_UNSIGNED ("%u,")
#define ARRAY_VALUE_FLOAT    ("%.*f,")
#define ARRAY_END            ("],")
#define ARRAY_TRIM           (char_size(","))
#define OBJECT_KEY           ("\"%s\":{")
#define OBJECT_END           ("},")
#else
#define STRING               ("\"%s\":\t\"%s\",")
#define NUMBER               ("\"%s\":\t%d,")
#define UNSIGNED             ("\"%s\":\t%u,")
#define FLOAT                ("\"%s\":\t%.*f,")
#define BOOLEAN              ("\"%s\":\t%s,")
#define ARRAY_KEY            ("\"%s\":\t[")
#define ARRAY_VALUE_STRING   ("\"%s\", ")
#define ARRAY_VALUE_NULL     ("null, ")
#define ARRAY_VALUE_NUMBER   ("%d, ")
#define ARRAY_VALUE_UNSIGNED ("%u, ")
#define ARRAY_VALUE_FLOAT    ("%.*f, ")
#define ARRAY_END            ("],")
#define ARRAY_TRIM           (char_size(", "))
#define OBJECT_KEY           ("\"%s\":\t{")
#define OBJECT_END           ("},")
#endif // CONFIG_KJSON_SMALLEST

#if !CONFIG_KJSON_NO_FLOAT
#define FLOAT_MASK (0xFFFFFFFF)
#define IS_FLOAT_NULL(value, nullValue) \
   ((*(size_t *)&(value)&FLOAT_MASK) == (*(size_t *)&(nullValue)&FLOAT_MASK))
#endif

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
static size_t InsertString(char *const string, const char *const key, const char *const value);
static size_t InsertNumber(char *const string, const char *const key, const int value);
static size_t InsertUnsignedNumber(char *const string, const char *const key, const unsigned int value);
#if !CONFIG_KJSON_NO_FLOAT
static size_t InsertFloat(char *const string, const char *const key, const float value, const unsigned int decimals);
#endif
static size_t InsertBoolean(char *const string, const char *const key, bool value);
static size_t InsertNull(char *const string, const char *const key);

static size_t InsertArrayNumber(char *const string, const char *const key, const void *const array, const size_t size, const NumberType_e type, void *const nullValue);
static size_t InsertArrayString(char *const string, const char *const key, const char *const *const array, const size_t size);
#if !CONFIG_KJSON_NO_FLOAT
static size_t InsertArrayFloat(char *const string, const char *const key, const float *const array, const size_t size, const unsigned int decimals, const float nullValue);
#endif

static size_t InitRoot(char *const string);
static size_t Trim(char *const string);
static size_t ExitRoot(char *const string);
static size_t EnterObject(char *const string, const char *const key);
static size_t ExitObject(char *const string);
static size_t InsertDepth(char *const string, const char *const newLine, const int depth);
static void StartEntry(kjson_t *const jsonHandle);

static size_t GetNumDigits(const void *const value, const NumberType_e type);
static bool StringFits(kjson_t *const jsonHandle, const char *const key, const char *const value);
static bool NumberFits(kjson_t *const jsonHandle, const char *const key, const void *const value, const NumberType_e type);
#if !CONFIG_KJSON_NO_FLOAT
static bool FloatFits(kjson_t *const jsonHandle, const char *const key, const float value, const unsigned int decimals);
#endif
static bool BooleanFits(kjson_t *const jsonHandle, const char *const key, bool value);
static bool NullFits(kjson_t *const jsonHandle, const char *const key);
static bool ArrayNumberFits(kjson_t *const jsonHandle, const char *const key, const void *const array, const size_t size, const NumberType_e type);
#if !CONFIG_KJSON_NO_FLOAT
static bool ArrayFloatFits(kjson_t *const jsonHandle, const char *const key, const float *const array, const size_t size, const unsigned int decimals);
#endif
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
         const size_t bytes = InsertString(jsonHandle->tail, key, value);
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
   if (value == jsonHandle->nullIntValue)
   {
      kJSON_InsertNull(jsonHandle, key);
   }
   else
   {
      if (NumberFits(jsonHandle, key, &value, eSigned))
      {
         StartEntry(jsonHandle);
         const size_t bytes = InsertNumber(jsonHandle->tail, key, value);
         jsonHandle->size += bytes;
         jsonHandle->tail += bytes;
      }
      else
      {
         jsonHandle->truncated = true;
      }
   }
}

void kJSON_InsertUnsignedNumber(kjson_t *const jsonHandle, const char *const key, const unsigned int value)
{
   if (value == jsonHandle->nullUIntValue)
   {
      kJSON_InsertNull(jsonHandle, key);
   }
   else
   {
      if (NumberFits(jsonHandle, key, &value, eUnsigned))
      {
         StartEntry(jsonHandle);
         const size_t bytes = InsertUnsignedNumber(jsonHandle->tail, key, value);
         jsonHandle->size += bytes;
         jsonHandle->tail += bytes;
      }
      else
      {
         jsonHandle->truncated = true;
      }
   }
}

#if !CONFIG_KJSON_NO_FLOAT
void kJSON_InsertFloat(kjson_t *const jsonHandle, const char *const key, const float value, const unsigned int decimals)
{
   if (IS_FLOAT_NULL(value, jsonHandle->nullFloatValue))
   {
      kJSON_InsertNull(jsonHandle, key);
   }
   else
   {
      if (FloatFits(jsonHandle, key, value, decimals))
      {
         StartEntry(jsonHandle);
         const size_t bytes = InsertFloat(jsonHandle->tail, key, value, decimals);
         jsonHandle->size += bytes;
         jsonHandle->tail += bytes;
      }
      else
      {
         jsonHandle->truncated = true;
      }
   }
}
#endif // CONFIG_KJSON_NO_FLOAT

void kJSON_InsertBoolean(kjson_t *const jsonHandle, const char *const key, bool value)
{
   if (BooleanFits(jsonHandle, key, value))
   {
      StartEntry(jsonHandle);
      const size_t bytes = InsertBoolean(jsonHandle->tail, key, value);
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
      const size_t bytes = InsertNull(jsonHandle->tail, key);
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
      const size_t bytes = InsertArrayNumber(jsonHandle->tail, key, array, size, eSigned, &jsonHandle->nullIntValue);
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
      const size_t bytes = InsertArrayNumber(jsonHandle->tail, key, array, size, eUnsigned, &jsonHandle->nullUIntValue);
      jsonHandle->size += bytes;
      jsonHandle->tail += bytes;
   }
   else
   {
      jsonHandle->truncated = true;
   }
}

void kJSON_InsertArrayFloat(kjson_t *const jsonHandle, const char *const key, const float *const array, const size_t size, const unsigned int decimals)
{
   if (ArrayFloatFits(jsonHandle, key, array, size, decimals))
   {
      StartEntry(jsonHandle);
      const size_t bytes = InsertArrayFloat(jsonHandle->tail, key, array, size, decimals, jsonHandle->nullFloatValue);
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
      const size_t bytes = InsertArrayString(jsonHandle->tail, key, array, size);
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
   const size_t bytes = InitRoot(jsonHandle->tail);
   jsonHandle->size += bytes;
   jsonHandle->tail += bytes;
   // Account for closing brace
   jsonHandle->size += strlen(jsonHandle->newLine) + char_size("}");
}

void kJSON_ExitRoot(kjson_t *const jsonHandle)
{
   const size_t trim = Trim(jsonHandle->tail);
   jsonHandle->tail += trim;
   StartEntry(jsonHandle);
   const size_t bytes = ExitRoot(jsonHandle->tail);
   jsonHandle->tail += bytes;
   jsonHandle->size += trim;
   jsonHandle->size -= strlen(jsonHandle->newLine);
}

void kJSON_EnterObject(kjson_t *const jsonHandle, const char *const key)
{
   if (ObjectFits(jsonHandle, key))
   {
      StartEntry(jsonHandle);
      const size_t bytes = EnterObject(jsonHandle->tail, key);
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
   size_t bytes = Trim(jsonHandle->tail);
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
static size_t InsertString(char *const string, const char *const key, const char *const value)
{
   char *const start = string;
   char *end = start;
   end += sprintf(end, STRING, key, value);
   return (size_t)(end - start);
}

static size_t InsertNumber(char *const string, const char *const key, const int value)
{
   char *const start = string;
   char *end = start;
   end += sprintf(end, NUMBER, key, value);
   return (size_t)(end - start);
}

static size_t InsertUnsignedNumber(char *const string, const char *const key, const unsigned int value)
{
   char *const start = string;
   char *end = start;
   end += sprintf(end, UNSIGNED, key, value);
   return (size_t)(end - start);
}

#if !CONFIG_KJSON_NO_FLOAT
static size_t InsertFloat(char *const string, const char *const key, const float value, const unsigned int decimals)
{
   char *const start = string;
   char *end = start;
   end += sprintf(end, FLOAT, key, decimals, value);
   return (size_t)(end - start);
}
#endif // CONFIG_KJSON_NO_FLOAT

static size_t InsertBoolean(char *const string, const char *const key, bool value)
{
   char *const start = string;
   char *end = start;
   end += sprintf(end, BOOLEAN, key, value ? BOOLEAN_TRUE : BOOLEAN_FALSE);
   return (size_t)(end - start);
}

static size_t InsertNull(char *const string, const char *const key)
{
   char *const start = string;
   char *end = start;
   end += sprintf(end, BOOLEAN, key, NULL_VALUE);
   return (size_t)(end - start);
}

static size_t InsertArrayNumber(char *const string, const char *const key, const void *const array, const size_t size, const NumberType_e type, void *const nullValue)
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
      else if (eSigned == type)
      {
         end += sprintf(end, ARRAY_VALUE_NUMBER, ((int *)array)[i]);
      }
      else
      {
         end += sprintf(end, ARRAY_VALUE_UNSIGNED, ((unsigned int *)array)[i]);
      }
   }
   end -= ARRAY_TRIM;
   end += sprintf(end, ARRAY_END);
   return (size_t)(end - start);
}

static size_t InsertArrayFloat(char *const string, const char *const key, const float *const array, const size_t size, const unsigned int decimals, const float nullValue)
{
   char *const start = string;
   char *end = start;
   end += sprintf(end, ARRAY_KEY, key);
   for (size_t i = 0; i < size; i++)
   {
      end += sprintf(end, (IS_FLOAT_NULL(array[i], nullValue) ? ARRAY_VALUE_NULL : ARRAY_VALUE_FLOAT), decimals, array[i]);
   }
   end -= ARRAY_TRIM;
   end += sprintf(end, ARRAY_END);
   return (size_t)(end - start);
}

static size_t InsertArrayString(char *const string, const char *const key, const char *const *const array, const size_t size)
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
   return (size_t)(end - start);
}

static size_t InitRoot(char *const string)
{
   char *const start = string;
   char *end = start;
   *(end++) = '{';
   return (size_t)(end - start);
}

static size_t Trim(char *const string)
{
   const char *const start = string;
   const char *const end = start - 1;
   if (',' == *end)
   {
      return (size_t)-1;
   }
   return 0;
}

static size_t ExitRoot(char *const string)
{
   char *const start = string;
   char *end = start;
   *(end++) = '}';
   *(end++) = '\0';
   return (size_t)(end - start);
}

static size_t EnterObject(char *const string, const char *const key)
{
   char *const start = string;
   char *end = start;
   end += sprintf(end, OBJECT_KEY, key);
   return (size_t)(end - start);
}

static size_t ExitObject(char *const string)
{
   char *const start = string;
   char *end = start;
   end += sprintf(end, OBJECT_END);
   return (size_t)(end - start);
}

static size_t InsertDepth(char *const string, const char *const newLine, const int depth)
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
   for (size_t i = 0; i < (size_t)depth; i++)
   {
      *(end++) = '\t';
   }
   return (size_t)(end - start);
#endif // CONFIG_KJSON_SMALLEST
}

static void StartEntry(kjson_t *const jsonHandle)
{
   const size_t bytes = InsertDepth(jsonHandle->tail, jsonHandle->newLine, jsonHandle->depth);
   jsonHandle->size += bytes;
   jsonHandle->tail += bytes;
}

static size_t GetNumDigits(const void *const value, const NumberType_e type)
{
   size_t count = 1;
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

#if !CONFIG_KJSON_NO_FLOAT
static bool FloatFits(kjson_t *const jsonHandle, const char *const key, const float value, const unsigned int decimals)
{
   const int intVal = (int)value;
   const size_t valueSize = GetNumDigits(&intVal, eSigned) + char_size(".") + decimals;
   const size_t size = strlen(jsonHandle->newLine) + jsonHandle->depth + strlen(key) + valueSize + char_size(FLOAT) - char_size("%s") - char_size("%.*f");
   return (jsonHandle->size + size <= jsonHandle->rootSize);
}
#endif // CONFIG_KJSON_NO_FLOAT

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
      if (((int *)array)[i] == jsonHandle->nullIntValue)
      {
         valueSize = char_size(NULL_VALUE);
      }
      else if (eSigned == type)
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

static bool ArrayFloatFits(kjson_t *const jsonHandle, const char *const key, const float *const array, const size_t size, const unsigned int decimals)
{
   size_t total = strlen(jsonHandle->newLine) + jsonHandle->depth + strlen(key) + char_size(ARRAY_KEY) - char_size("%s");
   for (size_t i = 0; i < size; i++)
   {
      if (IS_FLOAT_NULL(array[i], jsonHandle->nullFloatValue))
      {
         total += char_size(NULL_VALUE) + char_size(ARRAY_VALUE_FLOAT) - char_size("%.*f");
      }
      else
      {
         const int intVal = (int)array[i];
         const size_t valueSize = GetNumDigits(&intVal, eSigned) + char_size(".") + decimals;
         total += valueSize + char_size(ARRAY_VALUE_FLOAT) - char_size("%.*f");
      }
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
      if (array[i])
      {
         const size_t valueSize = strlen(array[i]);
         total += valueSize + char_size(ARRAY_VALUE_STRING) - char_size("%s");
      }
      else
      {
         total += char_size(ARRAY_VALUE_NULL);
      }
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
