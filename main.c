/*
 * File    : main.c
 * Created : 15/03/2023
 * Modified: 29/04/2023
 * Authors : Bogdan Ionescu
 */

#include <stdbool.h>
#include <stdio.h>

#include "kJSON.h"

#define array_size(array) (sizeof(array) / sizeof(array[0]))

int main(void)
{

// Buffer size should include the null terminator
#if CONFIG_KJSON_SMALLEST
   char root[512] = {0};
#else
   char root[640] = {0};
#endif

   kjson_t json = KJSON_INITIALISE(root, sizeof(root));
   kjson_t *jsonHandle = &json;

   // Use this to mark null values
   const int null = -999;
   json.nullIntValue = null;

   kJSON_InitRoot(jsonHandle);

   int digits[] = {0, 1, 2, 3, null, 5, 6, 7, 8, 9};
   const char *bobsFriends[] = {"Alice", "John", "Mary", NULL, "Peter", "Paul"};

   kJSON_InsertArrayInt(jsonHandle, "digits", digits, array_size(digits));

#if !CONFIG_KJSON_NO_FLOAT
   kJSON_InsertFloat(jsonHandle, "pi", 3.14159265f, 2);

   const float fNull = -99.0f;
   json.nullFloatValue = fNull;

   float temps[] = {12.5f, 13.2f, 14.1f, -99.0f, 16.0f, 17.0f, 18.0f};
   kJSON_InsertArrayFloat(jsonHandle, "temps", temps, array_size(temps), 2);
#endif

#if !CONFIG_KJSON_NO_DOUBLE
   kJSON_InsertDouble(jsonHandle, "e", 2.718281828459045, 4);

   const double dNull = -9999.0;
   json.nullDoubleValue = dNull;

   double voltages[] = {3.3001, 3.2999, -9999.0, 3.3010};
   kJSON_InsertArrayDouble(jsonHandle, "voltages", voltages, array_size(voltages), 3);
#endif

   kJSON_EnterObject(jsonHandle, "people");
   {
      kJSON_EnterObject(jsonHandle, "bob");
      {
         kJSON_InsertString(jsonHandle, "name", "Bob");
         kJSON_InsertNull(jsonHandle, "job");
         kJSON_InsertString(jsonHandle, "bestFriend", bobsFriends[3]);
         kJSON_InsertArrayString(jsonHandle, "friends", bobsFriends, array_size(bobsFriends));
         kJSON_InsertUnsignedNumber(jsonHandle, "age", 32);
         kJSON_InsertBoolean(jsonHandle, "married", false);
         kJSON_InsertString(jsonHandle, "car", "🚗");
         kJSON_InsertNumber(jsonHandle, "balance", -300);
         kJSON_InsertNumber(jsonHandle, "temperature", null);
      }
      kJSON_ExitObject(jsonHandle);
   }
   kJSON_ExitObject(jsonHandle);

   kJSON_ExitRoot(jsonHandle);

   printf("%s\r\n", json.root);
   printf("size: %zu\r\n", json.size);
   printf("truncated: %d\r\n", json.truncated);
   if (json.truncated)
   {
      return 1;
   }

   // Reset the json object
   json.tail = json.root;
   json.size = 0;

   return 0;
}
