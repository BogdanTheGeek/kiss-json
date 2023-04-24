//------------------------------------------------------------------------------
//       Filename: kJSON.h
//------------------------------------------------------------------------------
//       Bogdan Ionescu (c) 2022
//------------------------------------------------------------------------------
//       Purpose : Defines the kJSON API
//------------------------------------------------------------------------------
//       Notes : None
//------------------------------------------------------------------------------
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// Module includes
//------------------------------------------------------------------------------
#include <stdbool.h>
#include <stddef.h>

//------------------------------------------------------------------------------
// Module exported defines
//------------------------------------------------------------------------------
#define CONFIG_KJSON_SMALLEST 0

//------------------------------------------------------------------------------
// Module exported type definitions
//------------------------------------------------------------------------------
typedef struct
{
   char *const root;
   const size_t rootSize;
   char *tail;
   size_t size;
   int depth;
   const char *newLine;
   bool truncated;
} kjson_t;

//------------------------------------------------------------------------------
// Module exported functions
//------------------------------------------------------------------------------
void kJSON_InsertString(kjson_t *const jsonHandle, const char *const key, const char *const value);

void kJSON_InsertNumber(kjson_t *const jsonHandle, const char *const key, const int value);

void kJSON_InsertUnsignedNumber(kjson_t *const jsonHandle, const char *const key, const unsigned int value);

void kJSON_InsertBoolean(kjson_t *const jsonHandle, const char *const key, const bool value);

void kJSON_InsertArrayInt(kjson_t *const jsonHandle, const char *const key, const int *const array, const size_t size);

void kJSON_InsertArrayUInt(kjson_t *const jsonHandle, const char *const key, const unsigned int *const array, const size_t size);

void kJSON_InsertArrayString(kjson_t *const jsonHandle, const char *const key, char **array, const size_t size);

void kJSON_InitRoot(kjson_t *const jsonHandle);

void kJSON_ExitRoot(kjson_t *const jsonHandle);

void kJSON_EnterObject(kjson_t *const jsonHandle, const char *const key);

void kJSON_ExitObject(kjson_t *const jsonHandle);

//------------------------------------------------------------------------------
// Module exported variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif
