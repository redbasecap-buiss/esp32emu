#pragma once
// esp32emu — cJSON mock
// Lightweight JSON library included in ESP-IDF. Provides create/parse/print/delete.

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <string>

#define cJSON_Invalid (0)
#define cJSON_False   (1 << 0)
#define cJSON_True    (1 << 1)
#define cJSON_NULL    (1 << 2)
#define cJSON_Number  (1 << 3)
#define cJSON_String  (1 << 4)
#define cJSON_Array   (1 << 5)
#define cJSON_Object  (1 << 6)
#define cJSON_Raw     (1 << 7)

#define cJSON_IsReference 256
#define cJSON_StringIsConst 512

typedef struct cJSON {
    struct cJSON *next, *prev;
    struct cJSON *child;
    int type;
    char *valuestring;
    int valueint;
    double valuedouble;
    char *string;  // key name
} cJSON;

// ── Internal helpers ──
namespace esp32emu_cjson {

inline cJSON* new_item() {
    cJSON* item = (cJSON*)calloc(1, sizeof(cJSON));
    return item;
}

inline void free_string(char*& s) {
    if (s) { free(s); s = nullptr; }
}

inline char* dup(const char* s) {
    if (!s) return nullptr;
    size_t len = strlen(s) + 1;
    char* d = (char*)malloc(len);
    memcpy(d, s, len);
    return d;
}

} // namespace esp32emu_cjson

// ── Create ──
inline cJSON* cJSON_CreateNull()   { auto* i = esp32emu_cjson::new_item(); i->type = cJSON_NULL; return i; }
inline cJSON* cJSON_CreateTrue()   { auto* i = esp32emu_cjson::new_item(); i->type = cJSON_True; return i; }
inline cJSON* cJSON_CreateFalse()  { auto* i = esp32emu_cjson::new_item(); i->type = cJSON_False; return i; }
inline cJSON* cJSON_CreateBool(int b) { return b ? cJSON_CreateTrue() : cJSON_CreateFalse(); }

inline cJSON* cJSON_CreateNumber(double num) {
    auto* i = esp32emu_cjson::new_item();
    i->type = cJSON_Number;
    i->valuedouble = num;
    i->valueint = (int)num;
    return i;
}

inline cJSON* cJSON_CreateString(const char* s) {
    auto* i = esp32emu_cjson::new_item();
    i->type = cJSON_String;
    i->valuestring = esp32emu_cjson::dup(s);
    return i;
}

inline cJSON* cJSON_CreateArray() {
    auto* i = esp32emu_cjson::new_item();
    i->type = cJSON_Array;
    return i;
}

inline cJSON* cJSON_CreateObject() {
    auto* i = esp32emu_cjson::new_item();
    i->type = cJSON_Object;
    return i;
}

// ── Delete (recursive) ──
inline void cJSON_Delete(cJSON* item) {
    if (!item) return;
    cJSON* child = item->child;
    while (child) {
        cJSON* next = child->next;
        cJSON_Delete(child);
        child = next;
    }
    esp32emu_cjson::free_string(item->valuestring);
    esp32emu_cjson::free_string(item->string);
    free(item);
}

// ── Add to array/object ──
inline void cJSON_AddItemToArray(cJSON* array, cJSON* item) {
    if (!array || !item) return;
    if (!array->child) {
        array->child = item;
        item->prev = item;  // circular for last
        item->next = nullptr;
    } else {
        cJSON* last = array->child->prev;
        last->next = item;
        item->prev = last;
        item->next = nullptr;
        array->child->prev = item;
    }
}

inline void cJSON_AddItemToObject(cJSON* object, const char* key, cJSON* item) {
    if (!object || !item) return;
    esp32emu_cjson::free_string(item->string);
    item->string = esp32emu_cjson::dup(key);
    cJSON_AddItemToArray(object, item);
}

// ── Convenience add-to-object ──
inline cJSON* cJSON_AddNumberToObject(cJSON* obj, const char* key, double n) {
    cJSON* item = cJSON_CreateNumber(n);
    cJSON_AddItemToObject(obj, key, item);
    return item;
}

inline cJSON* cJSON_AddStringToObject(cJSON* obj, const char* key, const char* s) {
    cJSON* item = cJSON_CreateString(s);
    cJSON_AddItemToObject(obj, key, item);
    return item;
}

inline cJSON* cJSON_AddBoolToObject(cJSON* obj, const char* key, int b) {
    cJSON* item = cJSON_CreateBool(b);
    cJSON_AddItemToObject(obj, key, item);
    return item;
}

inline cJSON* cJSON_AddNullToObject(cJSON* obj, const char* key) {
    cJSON* item = cJSON_CreateNull();
    cJSON_AddItemToObject(obj, key, item);
    return item;
}

// ── Query ──
inline int cJSON_GetArraySize(const cJSON* array) {
    if (!array) return 0;
    int count = 0;
    cJSON* child = array->child;
    while (child) { count++; child = child->next; }
    return count;
}

inline cJSON* cJSON_GetArrayItem(const cJSON* array, int index) {
    if (!array) return nullptr;
    cJSON* child = array->child;
    for (int i = 0; i < index && child; i++) child = child->next;
    return child;
}

inline cJSON* cJSON_GetObjectItem(const cJSON* object, const char* key) {
    if (!object || !key) return nullptr;
    cJSON* child = object->child;
    while (child) {
        if (child->string && strcmp(child->string, key) == 0) return child;
        child = child->next;
    }
    return nullptr;
}

inline cJSON* cJSON_GetObjectItemCaseSensitive(const cJSON* object, const char* key) {
    return cJSON_GetObjectItem(object, key);
}

inline int cJSON_HasObjectItem(const cJSON* object, const char* key) {
    return cJSON_GetObjectItem(object, key) != nullptr;
}

// ── Type checks ──
inline int cJSON_IsInvalid(const cJSON* item) { return item ? (item->type & 0xFF) == cJSON_Invalid : 1; }
inline int cJSON_IsFalse(const cJSON* item)   { return item ? (item->type & 0xFF) == cJSON_False : 0; }
inline int cJSON_IsTrue(const cJSON* item)    { return item ? (item->type & 0xFF) == cJSON_True : 0; }
inline int cJSON_IsBool(const cJSON* item)    { return item ? ((item->type & 0xFF) == cJSON_True || (item->type & 0xFF) == cJSON_False) : 0; }
inline int cJSON_IsNull(const cJSON* item)    { return item ? (item->type & 0xFF) == cJSON_NULL : 0; }
inline int cJSON_IsNumber(const cJSON* item)  { return item ? (item->type & 0xFF) == cJSON_Number : 0; }
inline int cJSON_IsString(const cJSON* item)  { return item ? (item->type & 0xFF) == cJSON_String : 0; }
inline int cJSON_IsArray(const cJSON* item)   { return item ? (item->type & 0xFF) == cJSON_Array : 0; }
inline int cJSON_IsObject(const cJSON* item)  { return item ? (item->type & 0xFF) == cJSON_Object : 0; }

inline char* cJSON_GetStringValue(const cJSON* item) {
    if (!item || !cJSON_IsString(item)) return nullptr;
    return item->valuestring;
}

inline double cJSON_GetNumberValue(const cJSON* item) {
    if (!item || !cJSON_IsNumber(item)) return NAN;
    return item->valuedouble;
}

// ── Print (simplified) ──
namespace esp32emu_cjson {

inline void print_item(const cJSON* item, std::string& out, int depth, bool fmt) {
    if (!item) { out += "null"; return; }
    std::string indent(fmt ? depth * 2 : 0, ' ');
    std::string childIndent(fmt ? (depth + 1) * 2 : 0, ' ');

    switch (item->type & 0xFF) {
        case cJSON_NULL:   out += "null"; break;
        case cJSON_False:  out += "false"; break;
        case cJSON_True:   out += "true"; break;
        case cJSON_Number: {
            char buf[64];
            if (item->valuedouble == (double)item->valueint && std::abs(item->valuedouble) < 1e15) {
                snprintf(buf, sizeof(buf), "%d", item->valueint);
            } else {
                snprintf(buf, sizeof(buf), "%g", item->valuedouble);
            }
            out += buf;
            break;
        }
        case cJSON_String:
            out += "\"";
            if (item->valuestring) out += item->valuestring;
            out += "\"";
            break;
        case cJSON_Array: {
            out += "[";
            cJSON* child = item->child;
            bool first = true;
            while (child) {
                if (!first) out += ",";
                if (fmt) out += "\n" + childIndent;
                print_item(child, out, depth + 1, fmt);
                first = false;
                child = child->next;
            }
            if (item->child && fmt) out += "\n" + indent;
            out += "]";
            break;
        }
        case cJSON_Object: {
            out += "{";
            cJSON* child = item->child;
            bool first = true;
            while (child) {
                if (!first) out += ",";
                if (fmt) out += "\n" + childIndent;
                out += "\"";
                if (child->string) out += child->string;
                out += "\":";
                if (fmt) out += " ";
                print_item(child, out, depth + 1, fmt);
                first = false;
                child = child->next;
            }
            if (item->child && fmt) out += "\n" + indent;
            out += "}";
            break;
        }
        default: out += "null"; break;
    }
}

} // namespace esp32emu_cjson

inline char* cJSON_Print(const cJSON* item) {
    std::string out;
    esp32emu_cjson::print_item(item, out, 0, true);
    return esp32emu_cjson::dup(out.c_str());
}

inline char* cJSON_PrintUnformatted(const cJSON* item) {
    std::string out;
    esp32emu_cjson::print_item(item, out, 0, false);
    return esp32emu_cjson::dup(out.c_str());
}

// ── Parse (simplified) ──
namespace esp32emu_cjson {

inline const char* skip_ws(const char* p) {
    while (p && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')) p++;
    return p;
}

inline cJSON* parse_value(const char*& p);

inline cJSON* parse_string(const char*& p) {
    if (*p != '"') return nullptr;
    p++;
    const char* start = p;
    std::string val;
    while (*p && *p != '"') {
        if (*p == '\\') {
            p++;
            switch (*p) {
                case '"': val += '"'; break;
                case '\\': val += '\\'; break;
                case '/': val += '/'; break;
                case 'n': val += '\n'; break;
                case 't': val += '\t'; break;
                case 'r': val += '\r'; break;
                default: val += *p; break;
            }
        } else {
            val += *p;
        }
        p++;
    }
    if (*p == '"') p++;
    (void)start;
    cJSON* item = cJSON_CreateString(val.c_str());
    return item;
}

inline cJSON* parse_number(const char*& p) {
    char* end = nullptr;
    double val = strtod(p, &end);
    if (end == p) return nullptr;
    p = end;
    return cJSON_CreateNumber(val);
}

inline cJSON* parse_array(const char*& p) {
    p++; // skip [
    p = skip_ws(p);
    cJSON* arr = cJSON_CreateArray();
    if (*p == ']') { p++; return arr; }
    while (*p) {
        p = skip_ws(p);
        cJSON* item = parse_value(p);
        if (item) cJSON_AddItemToArray(arr, item);
        p = skip_ws(p);
        if (*p == ',') { p++; continue; }
        if (*p == ']') { p++; break; }
        break;
    }
    return arr;
}

inline cJSON* parse_object(const char*& p) {
    p++; // skip {
    p = skip_ws(p);
    cJSON* obj = cJSON_CreateObject();
    if (*p == '}') { p++; return obj; }
    while (*p) {
        p = skip_ws(p);
        if (*p != '"') break;
        cJSON* key_item = parse_string(p);
        if (!key_item) break;
        char* key = esp32emu_cjson::dup(key_item->valuestring);
        cJSON_Delete(key_item);
        p = skip_ws(p);
        if (*p == ':') p++;
        p = skip_ws(p);
        cJSON* val = parse_value(p);
        if (val) cJSON_AddItemToObject(obj, key, val);
        free(key);
        p = skip_ws(p);
        if (*p == ',') { p++; continue; }
        if (*p == '}') { p++; break; }
        break;
    }
    return obj;
}

inline cJSON* parse_value(const char*& p) {
    p = skip_ws(p);
    if (!p || !*p) return nullptr;
    if (*p == '"') return parse_string(p);
    if (*p == '{') return parse_object(p);
    if (*p == '[') return parse_array(p);
    if (*p == '-' || (*p >= '0' && *p <= '9')) return parse_number(p);
    if (strncmp(p, "true", 4) == 0)  { p += 4; return cJSON_CreateTrue(); }
    if (strncmp(p, "false", 5) == 0) { p += 5; return cJSON_CreateFalse(); }
    if (strncmp(p, "null", 4) == 0)  { p += 4; return cJSON_CreateNull(); }
    return nullptr;
}

} // namespace esp32emu_cjson

inline cJSON* cJSON_Parse(const char* value) {
    if (!value) return nullptr;
    const char* p = value;
    return esp32emu_cjson::parse_value(p);
}

// ── Duplicate ──
inline cJSON* cJSON_Duplicate(const cJSON* item, int recurse) {
    if (!item) return nullptr;
    char* printed = recurse ? cJSON_PrintUnformatted(item) : nullptr;
    if (printed) {
        cJSON* dup = cJSON_Parse(printed);
        free(printed);
        return dup;
    }
    // Non-recursive: copy just this item
    cJSON* n = esp32emu_cjson::new_item();
    n->type = item->type;
    n->valueint = item->valueint;
    n->valuedouble = item->valuedouble;
    if (item->valuestring) n->valuestring = esp32emu_cjson::dup(item->valuestring);
    if (item->string) n->string = esp32emu_cjson::dup(item->string);
    return n;
}

// ── Replace ──
inline int cJSON_ReplaceItemInObject(cJSON* object, const char* key, cJSON* newitem) {
    if (!object || !key || !newitem) return 0;
    cJSON* child = object->child;
    while (child) {
        if (child->string && strcmp(child->string, key) == 0) {
            newitem->string = esp32emu_cjson::dup(key);
            newitem->next = child->next;
            newitem->prev = child->prev;
            if (child->prev) {
                if (child->prev->next == child) child->prev->next = newitem;
            }
            if (child->next) child->next->prev = newitem;
            if (object->child == child) object->child = newitem;
            // Fix last pointer
            if (object->child->prev == child) object->child->prev = newitem;
            child->next = nullptr;
            child->prev = nullptr;
            cJSON_Delete(child);
            return 1;
        }
        child = child->next;
    }
    return 0;
}

// ── Delete from object ──
inline void cJSON_DeleteItemFromObject(cJSON* object, const char* key) {
    if (!object || !key) return;
    cJSON* child = object->child;
    while (child) {
        if (child->string && strcmp(child->string, key) == 0) {
            if (child->prev && child->prev->next == child) child->prev->next = child->next;
            if (child->next) child->next->prev = child->prev;
            if (object->child == child) object->child = child->next;
            if (object->child && object->child->prev == child) {
                // Fix last pointer
                cJSON* last = object->child;
                while (last->next) last = last->next;
                object->child->prev = last;
            }
            child->next = nullptr;
            child->prev = nullptr;
            cJSON_Delete(child);
            return;
        }
        child = child->next;
    }
}

// ── Create arrays of primitives ──
inline cJSON* cJSON_CreateIntArray(const int* numbers, int count) {
    cJSON* arr = cJSON_CreateArray();
    for (int i = 0; i < count; i++) cJSON_AddItemToArray(arr, cJSON_CreateNumber(numbers[i]));
    return arr;
}

inline cJSON* cJSON_CreateDoubleArray(const double* numbers, int count) {
    cJSON* arr = cJSON_CreateArray();
    for (int i = 0; i < count; i++) cJSON_AddItemToArray(arr, cJSON_CreateNumber(numbers[i]));
    return arr;
}

inline cJSON* cJSON_CreateStringArray(const char* const* strings, int count) {
    cJSON* arr = cJSON_CreateArray();
    for (int i = 0; i < count; i++) cJSON_AddItemToArray(arr, cJSON_CreateString(strings[i]));
    return arr;
}

// ── Iteration macro ──
#define cJSON_ArrayForEach(element, array) \
    for (element = (array != nullptr) ? (array)->child : nullptr; element != nullptr; element = element->next)
