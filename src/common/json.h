#ifndef JSON_H
#define JSON_H

#include "common.h"
#include "arena.h"
#include "sta_string.h"
#include <stdbool.h>

enum JsonType
{
  JSON_VALUE,
  JSON_OBJECT,
  JSON_ARRAY,
  JSON_STRING,
  JSON_NUMBER,
  JSON_BOOL,
  JSON_NULL
};
typedef enum JsonType JsonType;

struct JsonElement
{
  String       label;
  struct JsonElement* firstSubElement;
  struct jsonElement* nextSibling;
};

typedef struct JsonValue  JsonValue;
typedef struct JsonObject JsonObject;
typedef struct JsonArray  JsonArray;

struct JsonObject
{
  String*    keys;
  JsonValue* values;
  u64        size;
  u64        cap;
};

struct JsonArray
{
  u64        arraySize;
  u64        arrayCap;
  JsonValue* values;
};

struct JsonValue
{
  JsonType type;
  union
  {
    JsonObject obj;
    JsonArray  arr;
    bool       b;
    String     string;
    f64        number;
  };
};
typedef struct JsonArray JsonArray;

struct Json
{
  JsonType headType;
  union
  {
    JsonValue  value;
    JsonObject obj;
    JsonArray  array;
  };
};
typedef struct Json Json;

void                addElementToJsonObject(JsonObject* obj, String key, JsonValue value);
void                addElementToJsonArray(JsonArray* array, JsonValue value);
void                initJsonArray(Arena* arena, JsonArray* array);
void                initJsonObject(Arena* arena, JsonObject* obj);
bool                deserializeFromString(Arena* arena, Json* json, String fileContent);
bool                deserializeFromFile(Arena* arena, Json* json, String fileLocation);
bool                serializeToFile(Json* json, const char* filename);
void                debugJson(Json* json);
void                debugJsonArray(JsonArray* array);
void                debugJsonObject(JsonObject* obj);
void                debugJsonValue(JsonValue* value);

JsonValue*          lookupJsonElement(JsonObject* obj, const char* key);
void                freeJsonObject(JsonObject* obj);
void                freeJsonValue(JsonValue* value);
void                freeJsonArray(JsonArray* array);
void                freeJson(Json* json);
#endif
