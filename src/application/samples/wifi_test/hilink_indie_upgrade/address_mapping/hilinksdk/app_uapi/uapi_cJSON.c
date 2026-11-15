/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Implementation of cJSON in sdk side. \n
 *
 * History: \n
 * 2024-11-26, Create file. \n
 */

#include "app_call.h"
#include "cJSON.h"

#ifdef true
#undef true
#endif
#define true ((cJSON_bool)1)

#ifdef false
#undef false
#endif
#define false ((cJSON_bool)0)

const char *cJSON_Version(void)
{
    app_call0(APP_CALL_C_JSON_VERSION, cJSON_Version, const char *);
    return NULL;
}

void cJSON_InitHooks(cJSON_Hooks * hooks)
{
    app_call1_ret_void(APP_CALL_C_JSON_INIT_HOOKS, cJSON_InitHooks, cJSON_Hooks *, hooks);
}

cJSON *cJSON_Parse(const char *value)
{
    app_call1(APP_CALL_C_JSON_PARSE, cJSON_Parse, cJSON *, const char *, value);
    return NULL;
}

cJSON *cJSON_ParseWithLength(const char *value, size_t buffer_length)
{
    app_call2(APP_CALL_C_JSON_PARSE_WITH_LENGTH, cJSON_ParseWithLength, cJSON *,
        const char *, value, size_t, buffer_length);
    return NULL;
}

cJSON *cJSON_ParseWithOpts(const char *value, const char **return_parse_end, cJSON_bool require_null_terminated)
{
    app_call3(APP_CALL_C_JSON_PARSE_WITH_OPTS, cJSON_ParseWithOpts, cJSON *,
        const char *, value, const char **, return_parse_end, cJSON_bool, require_null_terminated);
    return NULL;
}

cJSON *cJSON_ParseWithLengthOpts(const char *value, size_t buffer_length,
    const char **return_parse_end, cJSON_bool require_null_terminated)
{
    app_call4(APP_CALL_C_JSON_PARSE_WITH_LENGTH_OPTS, cJSON_ParseWithLengthOpts, cJSON *, const char *,
        value, size_t, buffer_length, const char **, return_parse_end, cJSON_bool, require_null_terminated);
    return NULL;
}

char *cJSON_Print(const cJSON *item)
{
    app_call1(APP_CALL_C_JSON_PRINT, cJSON_Print, char *, const cJSON *, item);
    return NULL;
}

char *cJSON_PrintUnformatted(const cJSON *item)
{
    app_call1(APP_CALL_C_JSON_PRINT_UNFORMATTED, cJSON_PrintUnformatted, char *, const cJSON *, item);
    return NULL;
}

char *cJSON_PrintBuffered(const cJSON *item, int prebuffer, cJSON_bool fmt)
{
    app_call3(APP_CALL_C_JSON_PRINT_BUFFERED, cJSON_PrintBuffered, char *,
        const cJSON *, item, int, prebuffer, cJSON_bool, fmt);
    return NULL;
}

cJSON_bool cJSON_PrintPreallocated(cJSON *item, char *buffer, const int length, const cJSON_bool format)
{
    app_call4(APP_CALL_C_JSON_PRINT_PREALLOCATED, cJSON_PrintPreallocated, cJSON_bool,
        cJSON *, item, char *, buffer, const int, length, const cJSON_bool, format);
    return false;
}

void cJSON_Delete(cJSON *item)
{
    app_call1_ret_void(APP_CALL_C_JSON_DELETE, cJSON_Delete, cJSON *, item);
}

int cJSON_GetArraySize(const cJSON *array)
{
    app_call1(APP_CALL_C_JSON_GET_ARRAY_SIZE, cJSON_GetArraySize, int, const cJSON *, array);
    return 0;
}

cJSON *cJSON_GetArrayItem(const cJSON *array, int index)
{
    app_call2(APP_CALL_C_JSON_GET_ARRAY_ITEM, cJSON_GetArrayItem, cJSON *, const cJSON *, array, int, index);
    return NULL;
}

cJSON *cJSON_GetObjectItem(const cJSON * const object, const char * const string)
{
    app_call2(APP_CALL_C_JSON_GET_OBJECT_ITEM, cJSON_GetObjectItem, cJSON *,
        const cJSON * const, object, const char * const, string);
    return NULL;
}

cJSON *cJSON_GetObjectItemCaseSensitive(const cJSON * const object, const char * const string)
{
    app_call2(APP_CALL_C_JSON_GET_OBJECT_ITEM_CASE_SENSITIVE, cJSON_GetObjectItemCaseSensitive, cJSON *,
        const cJSON * const, object, const char * const, string);
    return NULL;
}

cJSON_bool cJSON_HasObjectItem(const cJSON *object, const char *string)
{
    app_call2(APP_CALL_C_JSON_HAS_OBJECT_ITEM, cJSON_HasObjectItem, cJSON_bool,
        const cJSON *, object, const char *, string);
    return false;
}

const char *cJSON_GetErrorPtr(void)
{
    app_call0(APP_CALL_C_JSON_GET_ERROR_PTR, cJSON_GetErrorPtr, const char *);
    return NULL;
}

char *cJSON_GetStringValue(const cJSON * const item)
{
    app_call1(APP_CALL_C_JSON_GET_STRING_VALUE, cJSON_GetStringValue, char *, const cJSON * const, item);
    return NULL;
}

double cJSON_GetNumberValue(const cJSON * const item)
{
    app_call1(APP_CALL_C_JSON_GET_NUMBER_VALUE, cJSON_GetNumberValue, double, const cJSON * const, item);
    return 0;
}

cJSON_bool cJSON_IsInvalid(const cJSON * const item)
{
    app_call1(APP_CALL_C_JSON_IS_INVALID, cJSON_IsInvalid, cJSON_bool, const cJSON * const, item);
    return false;
}

cJSON_bool cJSON_IsFalse(const cJSON * const item)
{
    app_call1(APP_CALL_C_JSON_IS_FALSE, cJSON_IsFalse, cJSON_bool, const cJSON * const, item);
    return false;
}

cJSON_bool cJSON_IsTrue(const cJSON * const item)
{
    app_call1(APP_CALL_C_JSON_IS_TRUE, cJSON_IsTrue, cJSON_bool, const cJSON * const, item);
    return false;
}

cJSON_bool cJSON_IsBool(const cJSON * const item)
{
    app_call1(APP_CALL_C_JSON_IS_BOOL, cJSON_IsBool, cJSON_bool, const cJSON * const, item);
    return false;
}

cJSON_bool cJSON_IsNull(const cJSON * const item)
{
    app_call1(APP_CALL_C_JSON_IS_NULL, cJSON_IsNull, cJSON_bool, const cJSON * const, item);
    return false;
}

cJSON_bool cJSON_IsNumber(const cJSON * const item)
{
    app_call1(APP_CALL_C_JSON_IS_NUMBER, cJSON_IsNumber, cJSON_bool, const cJSON * const, item);
    return false;
}

cJSON_bool cJSON_IsString(const cJSON * const item)
{
    app_call1(APP_CALL_C_JSON_IS_STRING, cJSON_IsString, cJSON_bool, const cJSON * const, item);
    return false;
}

cJSON_bool cJSON_IsArray(const cJSON * const item)
{
    app_call1(APP_CALL_C_JSON_IS_ARRAY, cJSON_IsArray, cJSON_bool, const cJSON * const, item);
    return false;
}

cJSON_bool cJSON_IsObject(const cJSON * const item)
{
    app_call1(APP_CALL_C_JSON_IS_OBJECT, cJSON_IsObject, cJSON_bool, const cJSON * const, item);
    return false;
}

cJSON_bool cJSON_IsRaw(const cJSON * const item)
{
    app_call1(APP_CALL_C_JSON_IS_RAW, cJSON_IsRaw, cJSON_bool, const cJSON * const, item);
    return false;
}

cJSON *cJSON_CreateNull(void)
{
    app_call0(APP_CALL_C_JSON_CREATE_NULL, cJSON_CreateNull, cJSON *);
    return NULL;
}

cJSON *cJSON_CreateTrue(void)
{
    app_call0(APP_CALL_C_JSON_CREATE_TRUE, cJSON_CreateTrue, cJSON *);
    return NULL;
}

cJSON *cJSON_CreateFalse(void)
{
    app_call0(APP_CALL_C_JSON_CREATE_FALSE, cJSON_CreateFalse, cJSON *);
    return NULL;
}

cJSON *cJSON_CreateBool(cJSON_bool boolean)
{
    app_call1(APP_CALL_C_JSON_CREATE_BOOL, cJSON_CreateBool, cJSON *, cJSON_bool, boolean);
    return NULL;
}

cJSON *cJSON_CreateNumber(double num)
{
    app_call1(APP_CALL_C_JSON_CREATE_NUMBER, cJSON_CreateNumber, cJSON *, double, num);
    return NULL;
}

cJSON *cJSON_CreateString(const char *string)
{
    app_call1(APP_CALL_C_JSON_CREATE_STRING, cJSON_CreateString, cJSON *, const char *, string);
    return NULL;
}

cJSON *cJSON_CreateRaw(const char *raw)
{
    app_call1(APP_CALL_C_JSON_CREATE_RAW, cJSON_CreateRaw, cJSON *, const char *, raw);
    return NULL;
}

cJSON *cJSON_CreateArray(void)
{
    app_call0(APP_CALL_C_JSON_CREATE_ARRAY, cJSON_CreateArray, cJSON *);
    return NULL;
}

cJSON *cJSON_CreateObject(void)
{
    app_call0(APP_CALL_C_JSON_CREATE_OBJECT, cJSON_CreateObject, cJSON *);
    return NULL;
}

cJSON *cJSON_CreateStringReference(const char *string)
{
    app_call1(APP_CALL_C_JSON_CREATE_STRING_REFERENCE, cJSON_CreateStringReference, cJSON *, const char *, string);
    return NULL;
}

cJSON *cJSON_CreateObjectReference(const cJSON *child)
{
    app_call1(APP_CALL_C_JSON_CREATE_OBJECT_REFERENCE, cJSON_CreateObjectReference, cJSON *, const cJSON *, child);
    return NULL;
}

cJSON *cJSON_CreateArrayReference(const cJSON *child)
{
    app_call1(APP_CALL_C_JSON_CREATE_ARRAY_REFERENCE, cJSON_CreateArrayReference, cJSON *, const cJSON *, child);
    return NULL;
}

cJSON *cJSON_CreateIntArray(const int *numbers, int count)
{
    app_call2(APP_CALL_C_JSON_CREATE_INT_ARRAY, cJSON_CreateIntArray, cJSON *, const int *, numbers, int, count);
    return NULL;
}

cJSON *cJSON_CreateFloatArray(const float *numbers, int count)
{
    app_call2(APP_CALL_C_JSON_CREATE_FLOAT_ARRAY, cJSON_CreateFloatArray, cJSON *,
        const float *, numbers, int, count);
    return NULL;
}

cJSON *cJSON_CreateDoubleArray(const double *numbers, int count)
{
    app_call2(APP_CALL_C_JSON_CREATE_DOUBLE_ARRAY, cJSON_CreateDoubleArray, cJSON *,
        const double *, numbers, int, count);
    return NULL;
}

cJSON *cJSON_CreateStringArray(const char *const *strings, int count)
{
    app_call2(APP_CALL_C_JSON_CREATE_STRING_ARRAY, cJSON_CreateStringArray, cJSON *,
        const char *const *, strings, int, count);
    return NULL;
}

cJSON_bool cJSON_AddItemToArray(cJSON *array, cJSON *item)
{
    app_call2(APP_CALL_C_JSON_ADD_ITEM_TO_ARRAY, cJSON_AddItemToArray, cJSON_bool, cJSON *, array, cJSON *, item);
    return false;
}

cJSON_bool cJSON_AddItemToObject(cJSON *object, const char *string, cJSON *item)
{
    app_call3(APP_CALL_C_JSON_ADD_ITEM_TO_OBJECT, cJSON_AddItemToObject, cJSON_bool,
        cJSON *, object, const char *, string, cJSON *, item);
    return false;
}

cJSON_bool cJSON_AddItemToObjectCS(cJSON *object, const char *string, cJSON *item)
{
    app_call3(APP_CALL_C_JSON_ADD_ITEM_TO_OBJECT_CS, cJSON_AddItemToObjectCS, cJSON_bool,
        cJSON *, object, const char *, string, cJSON *, item);
    return false;
}

cJSON_bool cJSON_AddItemReferenceToArray(cJSON *array, cJSON *item)
{
    app_call2(APP_CALL_C_JSON_ADD_ITEM_REFERENCE_TO_ARRAY, cJSON_AddItemReferenceToArray, cJSON_bool,
        cJSON *, array, cJSON *, item);
    return false;
}

cJSON_bool cJSON_AddItemReferenceToObject(cJSON *object, const char *string, cJSON *item)
{
    app_call3(APP_CALL_C_JSON_ADD_ITEM_REFERENCE_TO_OBJECT, cJSON_AddItemReferenceToObject, cJSON_bool,
        cJSON *, object, const char *, string, cJSON *, item);
    return false;
}

cJSON *cJSON_DetachItemViaPointer(cJSON *parent, cJSON * const item)
{
    app_call2(APP_CALL_C_JSON_DETACH_ITEM_VIA_POINTER, cJSON_DetachItemViaPointer, cJSON *,
        cJSON *, parent, cJSON * const, item);
    return NULL;
}

cJSON *cJSON_DetachItemFromArray(cJSON *array, int which)
{
    app_call2(APP_CALL_C_JSON_DETACH_ITEM_FROM_ARRAY, cJSON_DetachItemFromArray, cJSON *,
        cJSON *, array, int, which);
    return NULL;
}

void cJSON_DeleteItemFromArray(cJSON *array, int which)
{
    app_call2_ret_void(APP_CALL_C_JSON_DELETE_ITEM_FROM_ARRAY, cJSON_DeleteItemFromArray, cJSON *, array, int, which);
}

cJSON *cJSON_DetachItemFromObject(cJSON *object, const char *string)
{
    app_call2(APP_CALL_C_JSON_DETACH_ITEM_FROM_OBJECT, cJSON_DetachItemFromObject, cJSON *,
        cJSON *, object, const char *, string);
    return NULL;
}

cJSON *cJSON_DetachItemFromObjectCaseSensitive(cJSON *object, const char *string)
{
    app_call2(APP_CALL_C_JSON_DETACH_ITEM_FROM_OBJECT_CASE_SENSITIVE, cJSON_DetachItemFromObjectCaseSensitive, cJSON *,
        cJSON *, object, const char *, string);
    return NULL;
}

void cJSON_DeleteItemFromObject(cJSON *object, const char *string)
{
    app_call2_ret_void(APP_CALL_C_JSON_DELETE_ITEM_FROM_OBJECT, cJSON_DeleteItemFromObject,
        cJSON *, object, const char *, string);
}

void cJSON_DeleteItemFromObjectCaseSensitive(cJSON *object, const char *string)
{
    app_call2_ret_void(APP_CALL_C_JSON_DELETE_ITEM_FROM_OBJECT_CASE_SENSITIVE, cJSON_DeleteItemFromObjectCaseSensitive,
        cJSON *, object, const char *, string);
}

cJSON_bool cJSON_InsertItemInArray(cJSON *array, int which, cJSON *newitem)
{
    app_call3(APP_CALL_C_JSON_INSERT_ITEM_IN_ARRAY, cJSON_InsertItemInArray, cJSON_bool,
        cJSON *, array, int, which, cJSON *, newitem);
    return false;
}

cJSON_bool cJSON_ReplaceItemViaPointer(cJSON * const parent, cJSON *const item, cJSON *replacement)
{
    app_call3(APP_CALL_C_JSON_REPLACE_ITEM_VIA_POINTER, cJSON_ReplaceItemViaPointer, cJSON_bool,
        cJSON * const, parent, cJSON * const, item, cJSON *, replacement);
    return false;
}

cJSON_bool cJSON_ReplaceItemInArray(cJSON *array, int which, cJSON *newitem)
{
    app_call3(APP_CALL_C_JSON_REPLACE_ITEM_IN_ARRAY, cJSON_ReplaceItemInArray, cJSON_bool,
        cJSON *, array, int, which, cJSON *, newitem);
    return false;
}

cJSON_bool cJSON_ReplaceItemInObject(cJSON *object, const char *string, cJSON *newitem)
{
    app_call3(APP_CALL_C_JSON_REPLACE_ITEM_IN_OBJECT, cJSON_ReplaceItemInObject, cJSON_bool,
        cJSON *, object, const char *, string, cJSON *, newitem);
    return false;
}

cJSON_bool cJSON_ReplaceItemInObjectCaseSensitive(cJSON *object, const char *string, cJSON *newitem)
{
    app_call3(APP_CALL_C_JSON_REPLACE_ITEM_IN_OBJECT_CASE_SENSITIVE, cJSON_ReplaceItemInObjectCaseSensitive,
        cJSON_bool, cJSON *, object, const char *, string, cJSON *, newitem);
    return false;
}

cJSON *cJSON_Duplicate(const cJSON *item, cJSON_bool recurse)
{
    app_call2(APP_CALL_C_JSON_DUPLICATE, cJSON_Duplicate, cJSON *, const cJSON *, item, cJSON_bool, recurse);
    return NULL;
}

cJSON_bool cJSON_Compare(const cJSON * const a, const cJSON * const b, const cJSON_bool case_sensitive)
{
    app_call3(APP_CALL_C_JSON_COMPARE, cJSON_Compare, cJSON_bool,
        const cJSON * const, a, const cJSON * const, b, const cJSON_bool, case_sensitive);
    return false;
}

void cJSON_Minify(char *json)
{
    app_call1_ret_void(APP_CALL_C_JSON_MINIFY, cJSON_Minify, char *, json);
}

cJSON *cJSON_AddNullToObject(cJSON * const object, const char * const name)
{
    app_call2(APP_CALL_C_JSON_ADD_NULL_TO_OBJECT, cJSON_AddNullToObject, cJSON *,
        cJSON * const, object, const char * const, name);
    return NULL;
}

cJSON *cJSON_AddTrueToObject(cJSON * const object, const char * const name)
{
    app_call2(APP_CALL_C_JSON_ADD_TRUE_TO_OBJECT, cJSON_AddTrueToObject, cJSON *,
        cJSON * const, object, const char * const, name);
    return NULL;
}

cJSON *cJSON_AddFalseToObject(cJSON * const object, const char * const name)
{
    app_call2(APP_CALL_C_JSON_ADD_FALSE_TO_OBJECT, cJSON_AddFalseToObject, cJSON *,
        cJSON * const, object, const char * const, name);
    return NULL;
}

cJSON *cJSON_AddBoolToObject(cJSON * const object, const char * const name, const cJSON_bool boolean)
{
    app_call3(APP_CALL_C_JSON_ADD_BOOL_TO_OBJECT, cJSON_AddBoolToObject, cJSON *,
        cJSON * const, object, const char * const, name, const cJSON_bool, boolean);
    return NULL;
}

cJSON *cJSON_AddNumberToObject(cJSON * const object, const char * const name, const double number)
{
    app_call3(APP_CALL_C_JSON_ADD_NUMBER_TO_OBJECT, cJSON_AddNumberToObject, cJSON *,
        cJSON * const, object, const char * const, name, const double, number);
    return NULL;
}

cJSON *cJSON_AddStringToObject(cJSON * const object, const char * const name, const char * const string)
{
    app_call3(APP_CALL_C_JSON_ADD_STRING_TO_OBJECT, cJSON_AddStringToObject, cJSON *,
        cJSON * const, object, const char * const, name, const char * const, string);
    return NULL;
}

cJSON *cJSON_AddRawToObject(cJSON * const object, const char * const name, const char * const raw)
{
    app_call3(APP_CALL_C_JSON_ADD_RAW_TO_OBJECT, cJSON_AddRawToObject, cJSON *,
        cJSON * const, object, const char * const, name, const char * const, raw);
    return NULL;
}

cJSON *cJSON_AddObjectToObject(cJSON * const object, const char * const name)
{
    app_call2(APP_CALL_C_JSON_ADD_OBJECT_TO_OBJECT, cJSON_AddObjectToObject, cJSON *,
        cJSON * const, object, const char * const, name);
    return NULL;
}

cJSON *cJSON_AddArrayToObject(cJSON * const object, const char * const name)
{
    app_call2(APP_CALL_C_JSON_ADD_ARRAY_TO_OBJECT, cJSON_AddArrayToObject, cJSON *,
        cJSON * const, object, const char * const, name);
    return NULL;
}

double cJSON_SetNumberHelper(cJSON *object, double number)
{
    app_call2(APP_CALL_C_JSON_SET_NUMBER_HELPER, cJSON_SetNumberHelper, double, cJSON *, object, double, number);
    return 0;
}

char *cJSON_SetValuestring(cJSON *object, const char *valuestring)
{
    app_call2(APP_CALL_C_JSON_SET_VALUESTRING, cJSON_SetValuestring, char *,
        cJSON *, object, const char *, valuestring);
    return NULL;
}

void *cJSON_malloc(size_t size)
{
    app_call1(APP_CALL_C_JSON_MALLOC, cJSON_malloc, void *, size_t, size);
    return NULL;
}

void cJSON_free(void *object)
{
    app_call1_ret_void(APP_CALL_C_JSON_FREE, cJSON_free, void *, object);
}