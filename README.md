# KISS-JSON

K.I.S.S JSON composer library in C.

Features:
 - no allocation
 - cJSON style
 - small and hackable
 - custom runtime newline
 - compile time minimisation
 - always produces valid json
 - alerts the user if a key was skiped (not enough room in buffer)
 - MIT licence

## Versioning

Versions follow the standard [Semantic Versioning](https://en.wikipedia.org/wiki/Software_versioning#Semantic_versioning) system.

## Example:

For a fully working example, check `main.c`.

```C
   char root[1024] = {0};

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

   kJSON_InitRoot(jsonHandle);

   int array[] = {1, 2, 9};
   int numbers[] = {1, 1, 2, 3, 5, 16666};

   kJSON_InsertArrayInt(jsonHandle, "array", hs, array_size(array));

   kJSON_InsertArrayInt(jsonHandle, "numbers", rs, array_size(numbers));

   kJSON_EnterObject(jsonHandle, "bob");
   {

      kJSON_InsertString(jsonHandle, "last_name", "boberson");
      kJSON_InsertNumber(jsonHandle, "birthday", 1619271862);
      kJSON_InsertNumber(jsonHandle, "baby", true);
      kJSON_InsertNumber(jsonHandle, "legs", 2);
      kJSON_InsertNumber(jsonHandle, "balance", -320);
   }
   kJSON_ExitObject(jsonHandle);

   kJSON_ExitRoot(jsonHandle);

   if (jsonHandle->truncated)
   {
      printf("some keys did not fit, but the json is still valid");
   }

   // Reset the buffer
   json.tail = json.root;
   json.size = 0;
```
