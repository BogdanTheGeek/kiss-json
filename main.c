/*
 * File    : main.c
 * Created : 15/03/2023
 * Modified: 15/03/2023
 * Authors : Bogdan Ionescu
 */

#include <stdbool.h>
#include <stdio.h>

#include "kJSON.h"

#define array_size(array) (sizeof(array) / sizeof(array[0]))

int main(int argc, char *argv[])
{

// Buffer size should include the null terminator
#if CONFIG_KJSON_SMALLEST
   char root[118] = {0};
#else
   char root[169] = {0};
#endif

   kjson_t json = {
       .root = root,
       .rootSize = sizeof(root),
       .tail = root,
       .size = 0,
       .depth = 0,
       .newLine = "\n\r",
       .truncated = false,
   };
   kjson_t *jsonHandle = &json;

#if TEST
   int count = 1000000;
#else
   int count = 1;
#endif

   while (count--)
   {
      kJSON_InitRoot(jsonHandle);

      int digits[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

      kJSON_InsertArrayInt(jsonHandle, "digits", digits, array_size(digits));

      kJSON_EnterObject(jsonHandle, "people");
      {
         kJSON_EnterObject(jsonHandle, "bob");
         {
            kJSON_InsertString(jsonHandle, "name", "Bob");
            kJSON_InsertUnsignedNumber(jsonHandle, "age", 32);
            kJSON_InsertBoolean(jsonHandle, "married", false);
            kJSON_InsertString(jsonHandle, "car", "🚗");
            kJSON_InsertNumber(jsonHandle, "balance", -300);
         }
         kJSON_ExitObject(jsonHandle);
      }
      kJSON_ExitObject(jsonHandle);

      kJSON_ExitRoot(jsonHandle);

#if !TEST
      printf("%s\r\n", json.root);
      printf("size: %zu\r\n", json.size);
      printf("truncated: %d\r\n", json.truncated);
      if (json.truncated)
      {
         return 1;
      }
#endif
      json.tail = json.root;
      json.size = 0;
   }
   return 0;
}
