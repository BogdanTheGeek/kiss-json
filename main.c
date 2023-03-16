/*
 * File    : main.c
 * Created : 15/03/2023
 * Modified: 15/03/2023
 * Authors : Bogdan Ionescu
 */

#include <stdbool.h>
#include <stdio.h>

#include "kJSON.h"

/*
 * JSON example:
 * {
 *  "name": "Bogdan",
 *  "age": 23,
 *  "address":
 *   {
 *    "street": "Strada",
 *    "number": 23
 *   },
 *  "friends": [ "Ion", "Gheorghe", "Vasile" ],
 *  "favoriteNumbers": [ 1, 2, 3, 4, 5 ],
 *  "isMarried": false,
 * }
 */

#define array_size(array) (sizeof(array) / sizeof(array[0]))

int main(int argc, char *argv[])
{

   char root[210] = {0};

   json_t json = {
       .root = root,
       .rootSize = sizeof(root),
       .tail = root,
       .size = 0,
       .depth = 0,
       .newLine = "\r\n",
   };
   json_t *jsonHandle = &json;

   int favouriteNumbers[] = {1, 2, 3, 4, 5};
   char *friends[] = {"Ion", "Gheorghe", "Vasile"};

   kJSON_InitRoot(jsonHandle);
   {
      kJSON_InsertString(jsonHandle, "name", "Bogdan");
      kJSON_InsertNumber(jsonHandle, "age", 23);
      kJSON_EnterObject(jsonHandle, "address");
      {
         kJSON_InsertString(jsonHandle, "street", "Strada");
         kJSON_InsertNumber(jsonHandle, "number", 23);
         kJSON_EnterObject(jsonHandle, "address2");
         {
            kJSON_InsertString(jsonHandle, "street", "Strada");
            kJSON_InsertNumber(jsonHandle, "number", -23);
            kJSON_ExitObject(jsonHandle);
         }
         kJSON_EnterObject(jsonHandle, "address3");
         {
            kJSON_InsertString(jsonHandle, "street", "Strada");
            kJSON_InsertNumber(jsonHandle, "number", 23);
            kJSON_ExitObject(jsonHandle);
         }
         kJSON_ExitObject(jsonHandle);
      }
      kJSON_InsertArrayString(jsonHandle, "friends", friends, array_size(friends));
      kJSON_InsertArrayInt(jsonHandle, "favouriteNumbers", favouriteNumbers, array_size(favouriteNumbers));
      kJSON_InsertBoolean(jsonHandle, "isMarried", false);
   }
   kJSON_ExitRoot(jsonHandle);

   printf("%s\r\n", json.root);
   printf("sizeof(root) = %ld\r\n", json.size);

   return (json.size == 316) ? 1 : 0;
}
