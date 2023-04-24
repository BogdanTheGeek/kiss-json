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
} json_t;

//------------------------------------------------------------------------------
// Module exported functions
//------------------------------------------------------------------------------
void kJSON_InsertString(json_t *const jsonHandle, const char *const key, const char *const value);

void kJSON_InsertNumber(json_t *const jsonHandle, const char *const key, const int value);

void kJSON_InsertBoolean(json_t *const jsonHandle, const char *const key, const bool value);

void kJSON_InsertArrayInt(json_t *const jsonHandle, const char *const key, const int *const array, const int size);

void kJSON_InsertArrayString(json_t *const jsonHandle, const char *const key, char **array, const int size);

void kJSON_InitRoot(json_t *const jsonHandle);

void kJSON_ExitRoot(json_t *const jsonHandle);

void kJSON_EnterObject(json_t *const jsonHandle, const char *const key);

void kJSON_ExitObject(json_t *const jsonHandle);

//------------------------------------------------------------------------------
// Module exported variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif
