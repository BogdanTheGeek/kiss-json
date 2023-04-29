# KISS-JSON

K.I.S.S JSON composer library in C.

Features:
 - No allocation
 - CJSON style
 - Small and hackable
 - Custom runtime newline
 - Handle `null` strings
 - Custom `null` value for numbers (eg. `-999` will be replaced with `null`)
 - Floating point support can be disabled
 - Compile time minimisation
 - Always produces valid json
 - Alerts the user if a key was skiped (not enough room in buffer)
 - MIT licence

Limitations:
 - Key pointers are not checked
 - No arrays of objects (use `for` loop)

## Notes:
 - `kiss-json` is not a parser, use [jsmn](https://github.com/zserge/jsmn) for memory efficient parsing
 - The default output format is not the prettiest, its ment to be a good balance between readability and memeory usage. Pass the output to [jq](https://stedolan.github.io/jq/) to make it pretty.

## Versioning

Versions follow the standard [Semantic Versioning](https://en.wikipedia.org/wiki/Software_versioning#Semantic_versioning) system.

## Example:

For a full working example, check `main.c`.
```C
   char root[1024] = {0};

   kjson_t json = KJSON_INITIALISE(root);
   kjson_t *jsonHandle = &json;

   kJSON_InitRoot(jsonHandle);

   int digits[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
   const char *job = NULL;

   kJSON_InsertArrayInt(jsonHandle, "digits", digits, array_size(digits));

   kJSON_EnterObject(jsonHandle, "people");
   {
      kJSON_EnterObject(jsonHandle, "bob");
      {
         kJSON_InsertString(jsonHandle, "name", "Bob");
         kJSON_InsertString(jsonHandle, "job", job);
         kJSON_InsertUnsignedNumber(jsonHandle, "age", 32);
         kJSON_InsertBoolean(jsonHandle, "married", false);
         kJSON_InsertString(jsonHandle, "car", "🚗");
         kJSON_InsertNumber(jsonHandle, "balance", -300);
      }
      kJSON_ExitObject(jsonHandle);
   }
   kJSON_ExitObject(jsonHandle);

   kJSON_ExitRoot(jsonHandle);

   // Reset
   json.tail = json.root;
   json.size = 0;
```

Output with `CONFIG_KJSON_SMALLEST (1)`:
```json
{"digits":[0,1,2,3,4,5,6,7,8,9],"people":{"bob":{"name":"Bob","job":null,"age":32,"married":false,"car":"🚗","balance":-300}}}
```

Output with `CONFIG_KJSON_SMALLEST (0)`:
```json
{
"digits":	[0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
"people":	{
	"bob":	{
		"name":	"Bob",
		"job":	null,
		"age":	32,
		"married":	false,
		"car":	"🚗",
		"balance":	-300
	}
}
}
```

## Acknowledgments:
 - [cJSON](https://github.com/DaveGamble/cJSON) - "Ultralightweight JSON parser in ANSI C" -> requires dynamic allocation
 - [jsmn](https://github.com/zserge/jsmn) - "Jsmn is a world fastest JSON parser/tokenizer." -> great for parsing
 - [tiny-json](https://github.com/rafagafe/tiny-json) - "The tiny-json is a versatile and easy to use json parser in C suitable for embedded systems."
 - [microtojson](https://sr.ht/~rkta/microtojson/) - "A simple embedded friendly JSON generator."
 - [json-maker](https://github.com/rafagafe/json-maker) - "C library used to code JSON objects in null-terminated strings"
