//------------------------------------------------------------------------------
//       Filename: kJSON.h
//------------------------------------------------------------------------------
//       Bogdan Ionescu (c) 2022
//------------------------------------------------------------------------------
//       Purpose : Defines the kJSON API
//------------------------------------------------------------------------------
//       Version : 1.1.1
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
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>

//------------------------------------------------------------------------------
// Module exported defines
//------------------------------------------------------------------------------

#define KJSON_VERSION_MAJOR (1)
#define KJSON_VERSION_MINOR (1)
#define KJSON_VERSION_PATCH (1)

#define CONFIG_KJSON_SMALLEST (1)

#define KJSON_INITIALISE(buffer)    \
   {                                \
      .root = (buffer),             \
      .rootSize = sizeof((buffer)), \
      .tail = (buffer),             \
      .size = 0,                    \
      .depth = 0,                   \
      .newLine = "\n",              \
      .nullIntValue = (INT_MAX),    \
      .truncated = false            \
   }

//------------------------------------------------------------------------------
// Module exported type definitions
//------------------------------------------------------------------------------
typedef struct
{
   // Initialisation parameters
   char *const root;      // Buffer to store output
   const size_t rootSize; // Size of the buffer
   char *tail;            // Point to last character inserted (point to root)
   const char *newLine;   // Character to use for new line
   union
   {
      int nullIntValue;           // Value that marks a null integer
      unsigned int nullUIntValue; // Value that marks a null unsigned integer
   };

   // Output parameters
   size_t size;    // Size of the output
   bool truncated; // True if some objects could not fit

   // Internal parameters
   int depth; // Used to track the depth of the JSON object
} kjson_t;

//------------------------------------------------------------------------------
// Module exported functions
//------------------------------------------------------------------------------

/**
 * @brief  Inserts a string into the JSON object
 * @param  jsonHandle: JSON object handle
 * @param  key: Key of the string
 * @param  value: Value of the string
 * @return None
 */
void kJSON_InsertString(kjson_t *const jsonHandle, const char *const key, const char *const value);

/**
 * @brief  Inserts a number into the JSON object
 * @param  jsonHandle: JSON object handle
 * @param  key: Key of the number
 * @param  value: Value of the number
 * @return None
 */
void kJSON_InsertNumber(kjson_t *const jsonHandle, const char *const key, const int value);

/**
 * @brief  Inserts an unsigned number into the JSON object
 * @param  jsonHandle: JSON object handle
 * @param  key: Key of the number
 * @param  value: Value of the number
 * @return None
 */
void kJSON_InsertUnsignedNumber(kjson_t *const jsonHandle, const char *const key, const unsigned int value);

/**
 * @brief  Inserts a boolean into the JSON object
 * @param  jsonHandle: JSON object handle
 * @param  key: Key of the boolean
 * @param  value: Value of the boolean
 * @return None
 */
void kJSON_InsertBoolean(kjson_t *const jsonHandle, const char *const key, const bool value);

/**
 * @brief  Inserts a null into the JSON object
 * @param  jsonHandle: JSON object handle
 * @param  key: Key of the null
 * @return None
 */
void kJSON_InsertNull(kjson_t *const jsonHandle, const char *const key);

/**
 * @brief  Inserts an array of numbers into the JSON object
 * @param  jsonHandle: JSON object handle
 * @param  key: Key of the array
 * @param  array: Array of numbers
 * @param  size: Size of the array
 * @return None
 */
void kJSON_InsertArrayInt(kjson_t *const jsonHandle, const char *const key, const int *const array, const size_t size);

/**
 * @brief  Inserts an array of unsigned numbers into the JSON object
 * @param  jsonHandle: JSON object handle
 * @param  key: Key of the array
 * @param  array: Array of numbers
 * @param  size: Size of the array
 * @return None
 */
void kJSON_InsertArrayUInt(kjson_t *const jsonHandle, const char *const key, const unsigned int *const array, const size_t size);

/**
 * @brief  Inserts an array of strings into the JSON object
 * @param  jsonHandle: JSON object handle
 * @param  key: Key of the array
 * @param  array: Array of strings
 * @param  size: Size of the array
 * @return None
 */
void kJSON_InsertArrayString(kjson_t *const jsonHandle, const char *const key, const char *const *const array, const size_t size);

/**
 * @brief  Inserts the root object into the JSON object
 * @param  jsonHandle: JSON object handle
 * @return None
 */
void kJSON_InitRoot(kjson_t *const jsonHandle);

/**
 * @brief  Terminates the root object
 * @param  jsonHandle: JSON object handle
 * @return None
 */
void kJSON_ExitRoot(kjson_t *const jsonHandle);

/**
 * @brief  Inserts an object into the JSON object
 * @param  jsonHandle: JSON object handle
 * @param  key: Key of the object
 * @return None
 */
void kJSON_EnterObject(kjson_t *const jsonHandle, const char *const key);

/**
 * @brief  Terminates the object
 * @param  jsonHandle: JSON object handle
 * @return None
 */
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
