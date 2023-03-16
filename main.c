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

#if !CONFIG_KJSON_SMALLEST
   char root[752] = {0};
#else
   char root[636] = {0};
#endif

   json_t json = {
       .root = root,
       .rootSize = sizeof(root),
       .tail = root,
       .size = 0,
       .depth = 0,
       .newLine = "\n\r",
       .truncated = false,
   };
   json_t *jsonHandle = &json;

#if TEST
   int count = 1000000;
#else
   int count = 1;
#endif

   while (count--)
   {
      kJSON_InitRoot(jsonHandle);

      int hs[] = {1, 2, 9};
      int rs[] = {1, 1, 2, 3, 5, 16666};

      kJSON_InsertArrayInt(jsonHandle, "hs", hs, array_size(hs));

      kJSON_InsertArrayInt(jsonHandle, "rs", rs, array_size(rs));

      kJSON_EnterObject(jsonHandle, "debug_heatcontroller");
      {

         kJSON_InsertString(jsonHandle, "hubtimeStr", ("Tuesday the 21st of April 2023 at 21:59"));
         kJSON_InsertNumber(jsonHandle, "hubtimeUnix", (1699323));
         kJSON_InsertNumber(jsonHandle, "suspended", (true));
         kJSON_InsertNumber(jsonHandle, "controlRunning", (false));
         kJSON_InsertNumber(jsonHandle, "timeSlot", (31));
         kJSON_InsertString(jsonHandle, "controlsStartTimeStr", ("Tuesday the 21st of April 2023 at 21:59"));
         kJSON_InsertNumber(jsonHandle, "controlsStartTimeUnix", (1620200));
         kJSON_InsertNumber(jsonHandle, "secondsSinceLastAlgoRun", (160432));
         kJSON_InsertNumber(jsonHandle, "numberOfZones", (2));
         kJSON_InsertNumber(jsonHandle, "heatingOn", (false));
         kJSON_InsertNumber(jsonHandle, "dhwOn", (true));
         kJSON_InsertNumber(jsonHandle, "zone1On", (true));
         kJSON_InsertNumber(jsonHandle, "zone2On", (true));
         kJSON_InsertNumber(jsonHandle, "zone1Temp", (320));
         kJSON_InsertNumber(jsonHandle, "zone2Temp", (120));
         kJSON_InsertNumber(jsonHandle, "zone1Setpoint", (-1));
         kJSON_InsertNumber(jsonHandle, "zone2Setpoint", (123));
         kJSON_InsertNumber(jsonHandle, "dhwSetpoint", (222));
         kJSON_InsertNumber(jsonHandle, "flowSetpoint", (450));
         kJSON_InsertNumber(jsonHandle, "mode", (1));
         kJSON_InsertNumber(jsonHandle, "manualOverrideState", (0));
         kJSON_InsertNumber(jsonHandle, "manualOverrideHeatRemainingMs", (231312));
         kJSON_InsertNumber(jsonHandle, "manualOverrideDHWRemainingMs", (323123));
         kJSON_InsertNumber(jsonHandle, "secondsSinceLastControlTime", (2312));
      }
      kJSON_ExitObject(jsonHandle);

      kJSON_ExitRoot(jsonHandle);

#if !TEST
      printf("%s\r\n", json.root);
      printf("size: %zu\r\n", json.size);
      printf("truncated: %d\r\n", json.truncated);
#endif
      json.tail = json.root;
      json.size = 0;
   }
   return 0;
}
