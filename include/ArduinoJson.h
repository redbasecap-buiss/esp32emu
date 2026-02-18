#pragma once
// esp32emu: ArduinoJson v7 mock — lightweight JSON library
// Supports: JsonDocument, serializeJson, deserializeJson, nested objects/arrays
#include <cstdint>
#include <cstring>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <memory>

class JsonVariant;
class JsonArray;
class JsonObject;

enum DeserializationError_Code {
    DeserializationError_Ok = 0,
    DeserializationError_EmptyInput,
    DeserializationError_InvalidInput,
    DeserializationError_NoMemory,
    DeserializationError_TooDeep
};

class DeserializationError {
public:
    DeserializationError(DeserializationError_Code c = DeserializationError_Ok) : code_(c) {}
    explicit operator bool() const { return code_ != DeserializationError_Ok; }
    const char* c_str() const {
        switch (code_) {
            case DeserializationError_Ok: return "Ok";
            case DeserializationError_EmptyInput: return "EmptyInput";
            case DeserializationError_InvalidInput: return "InvalidInput";
            case DeserializationError_NoMemory: return "NoMemory";
            case DeserializationError_TooDeep: return "TooDeep";
        }
        return "Unknown";
    }
    DeserializationError_Code code() const { return code_; }
private:
    DeserializationError_Code code_;
};

// Forward
class JsonNode {
public:
    enum Type { Null, Bool, Int, Float, String, Array, Object };
    Type type = Null;
    bool bVal = false;
    long iVal = 0;
    double fVal = 0.0;
    std::string sVal;
    std::vector<std::shared_ptr<JsonNode>> arr;
    std::map<std::string, std::shared_ptr<JsonNode>> obj;

    JsonNode() = default;
    JsonNode(Type t) : type(t) {}
};

class JsonVariantBase {
public:
    JsonVariantBase() : node_(std::make_shared<JsonNode>()) {}
    JsonVariantBase(std::shared_ptr<JsonNode> n) : node_(n ? n : std::make_shared<JsonNode>()) {}

    bool isNull() const { return node_->type == JsonNode::Null; }

    // Assignment
    JsonVariantBase& operator=(bool v) { node_->type = JsonNode::Bool; node_->bVal = v; return *this; }
    JsonVariantBase& operator=(int v) { node_->type = JsonNode::Int; node_->iVal = v; return *this; }
    JsonVariantBase& operator=(long v) { node_->type = JsonNode::Int; node_->iVal = v; return *this; }
    JsonVariantBase& operator=(double v) { node_->type = JsonNode::Float; node_->fVal = v; return *this; }
    JsonVariantBase& operator=(float v) { node_->type = JsonNode::Float; node_->fVal = v; return *this; }
    JsonVariantBase& operator=(const char* v) { node_->type = JsonNode::String; node_->sVal = v ? v : ""; return *this; }
    JsonVariantBase& operator=(const std::string& v) { node_->type = JsonNode::String; node_->sVal = v; return *this; }

    // Conversion
    template<typename T> T as() const;
    operator bool() const { return !isNull(); }

    // Object access
    JsonVariantBase operator[](const char* key) {
        if (node_->type != JsonNode::Object) node_->type = JsonNode::Object;
        auto& m = node_->obj;
        if (m.find(key) == m.end()) m[key] = std::make_shared<JsonNode>();
        return JsonVariantBase(m[key]);
    }
    JsonVariantBase operator[](const std::string& key) { return operator[](key.c_str()); }

    // Array access
    JsonVariantBase operator[](int idx) {
        if (node_->type != JsonNode::Array) return JsonVariantBase();
        if (idx < 0 || idx >= (int)node_->arr.size()) return JsonVariantBase();
        return JsonVariantBase(node_->arr[idx]);
    }

    // Array add
    JsonVariantBase add() {
        if (node_->type != JsonNode::Array) node_->type = JsonNode::Array;
        auto n = std::make_shared<JsonNode>();
        node_->arr.push_back(n);
        return JsonVariantBase(n);
    }

    JsonObject createNestedObject(const char* key = nullptr);
    JsonArray createNestedArray(const char* key = nullptr);

    bool containsKey(const char* key) const {
        return node_->type == JsonNode::Object && node_->obj.count(key);
    }

    size_t size() const {
        if (node_->type == JsonNode::Array) return node_->arr.size();
        if (node_->type == JsonNode::Object) return node_->obj.size();
        return 0;
    }

    std::shared_ptr<JsonNode> node_;
};

template<> inline int JsonVariantBase::as<int>() const {
    if (node_->type == JsonNode::Int) return (int)node_->iVal;
    if (node_->type == JsonNode::Float) return (int)node_->fVal;
    if (node_->type == JsonNode::Bool) return node_->bVal ? 1 : 0;
    return 0;
}
template<> inline long JsonVariantBase::as<long>() const { return (long)as<int>(); }
template<> inline float JsonVariantBase::as<float>() const {
    if (node_->type == JsonNode::Float) return (float)node_->fVal;
    if (node_->type == JsonNode::Int) return (float)node_->iVal;
    return 0.0f;
}
template<> inline double JsonVariantBase::as<double>() const {
    if (node_->type == JsonNode::Float) return node_->fVal;
    if (node_->type == JsonNode::Int) return (double)node_->iVal;
    return 0.0;
}
template<> inline bool JsonVariantBase::as<bool>() const {
    if (node_->type == JsonNode::Bool) return node_->bVal;
    if (node_->type == JsonNode::Int) return node_->iVal != 0;
    return false;
}
template<> inline const char* JsonVariantBase::as<const char*>() const {
    if (node_->type == JsonNode::String) return node_->sVal.c_str();
    return nullptr;
}
template<> inline std::string JsonVariantBase::as<std::string>() const {
    if (node_->type == JsonNode::String) return node_->sVal;
    return "";
}

class JsonObject : public JsonVariantBase {
public:
    JsonObject() : JsonVariantBase() { node_->type = JsonNode::Object; }
    JsonObject(std::shared_ptr<JsonNode> n) : JsonVariantBase(n) { if (n) n->type = JsonNode::Object; }
};

class JsonArray : public JsonVariantBase {
public:
    JsonArray() : JsonVariantBase() { node_->type = JsonNode::Array; }
    JsonArray(std::shared_ptr<JsonNode> n) : JsonVariantBase(n) { if (n) n->type = JsonNode::Array; }

    template<typename T>
    void add(T val) { auto e = JsonVariantBase::add(); e = val; }
};

inline JsonObject JsonVariantBase::createNestedObject(const char* key) {
    if (key) {
        if (node_->type != JsonNode::Object) node_->type = JsonNode::Object;
        auto n = std::make_shared<JsonNode>(JsonNode::Object);
        node_->obj[key] = n;
        return JsonObject(n);
    } else {
        // array context
        if (node_->type != JsonNode::Array) node_->type = JsonNode::Array;
        auto n = std::make_shared<JsonNode>(JsonNode::Object);
        node_->arr.push_back(n);
        return JsonObject(n);
    }
}

inline JsonArray JsonVariantBase::createNestedArray(const char* key) {
    if (key) {
        if (node_->type != JsonNode::Object) node_->type = JsonNode::Object;
        auto n = std::make_shared<JsonNode>(JsonNode::Array);
        node_->obj[key] = n;
        return JsonArray(n);
    } else {
        if (node_->type != JsonNode::Array) node_->type = JsonNode::Array;
        auto n = std::make_shared<JsonNode>(JsonNode::Array);
        node_->arr.push_back(n);
        return JsonArray(n);
    }
}

// Serializer
inline void _serializeNode(const std::shared_ptr<JsonNode>& node, std::ostringstream& os) {
    if (!node) { os << "null"; return; }
    switch (node->type) {
        case JsonNode::Null: os << "null"; break;
        case JsonNode::Bool: os << (node->bVal ? "true" : "false"); break;
        case JsonNode::Int: os << node->iVal; break;
        case JsonNode::Float: os << node->fVal; break;
        case JsonNode::String:
            os << "\"";
            for (char c : node->sVal) {
                if (c == '"') os << "\\\"";
                else if (c == '\\') os << "\\\\";
                else if (c == '\n') os << "\\n";
                else os << c;
            }
            os << "\"";
            break;
        case JsonNode::Array:
            os << "[";
            for (size_t i = 0; i < node->arr.size(); i++) {
                if (i > 0) os << ",";
                _serializeNode(node->arr[i], os);
            }
            os << "]";
            break;
        case JsonNode::Object:
            os << "{";
            { int idx = 0;
            for (auto& kv : node->obj) {
                if (idx++ > 0) os << ",";
                os << "\"" << kv.first << "\":";
                _serializeNode(kv.second, os);
            }}
            os << "}";
            break;
    }
}

class JsonDocument : public JsonVariantBase {
public:
    JsonDocument() : JsonVariantBase() {}
    explicit JsonDocument(size_t) : JsonVariantBase() {} // capacity ignored in mock

    JsonObject to() { node_->type = JsonNode::Object; node_->obj.clear(); return JsonObject(node_); }
    JsonObject as() { return JsonObject(node_); }
    void clear() { node_ = std::make_shared<JsonNode>(); }
};

// Compatibility aliases
using StaticJsonDocument = JsonDocument;
using DynamicJsonDocument = JsonDocument;

inline size_t serializeJson(const JsonVariantBase& doc, std::string& out) {
    std::ostringstream os;
    _serializeNode(doc.node_, os);
    out = os.str();
    return out.size();
}

inline size_t serializeJson(const JsonVariantBase& doc, char* buf, size_t len) {
    std::string s;
    serializeJson(doc, s);
    size_t n = std::min(s.size(), len - 1);
    memcpy(buf, s.c_str(), n);
    buf[n] = '\0';
    return n;
}

// Minimal JSON parser
namespace _jsonparse {
    inline void skipWs(const char*& p) { while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') p++; }

    inline std::shared_ptr<JsonNode> parseValue(const char*& p, int depth = 0);

    inline std::shared_ptr<JsonNode> parseString(const char*& p) {
        if (*p != '"') return nullptr;
        p++;
        std::string s;
        while (*p && *p != '"') {
            if (*p == '\\') { p++; if (*p == 'n') s += '\n'; else if (*p == 't') s += '\t'; else s += *p; }
            else s += *p;
            p++;
        }
        if (*p == '"') p++;
        auto n = std::make_shared<JsonNode>(JsonNode::String);
        n->sVal = s;
        return n;
    }

    inline std::shared_ptr<JsonNode> parseNumber(const char*& p) {
        const char* start = p;
        bool isFloat = false;
        if (*p == '-') p++;
        while (*p >= '0' && *p <= '9') p++;
        if (*p == '.') { isFloat = true; p++; while (*p >= '0' && *p <= '9') p++; }
        if (*p == 'e' || *p == 'E') { isFloat = true; p++; if (*p == '+' || *p == '-') p++; while (*p >= '0' && *p <= '9') p++; }
        std::string num(start, p);
        auto n = std::make_shared<JsonNode>();
        if (isFloat) { n->type = JsonNode::Float; n->fVal = std::stod(num); }
        else { n->type = JsonNode::Int; n->iVal = std::stol(num); }
        return n;
    }

    inline std::shared_ptr<JsonNode> parseValue(const char*& p, int depth) {
        if (depth > 32) return nullptr;
        skipWs(p);
        if (!*p) return nullptr;
        if (*p == '"') return parseString(p);
        if (*p == '{') {
            p++;
            auto n = std::make_shared<JsonNode>(JsonNode::Object);
            skipWs(p);
            if (*p == '}') { p++; return n; }
            while (true) {
                skipWs(p);
                auto key = parseString(p);
                if (!key) return nullptr;
                skipWs(p);
                if (*p != ':') return nullptr;
                p++;
                auto val = parseValue(p, depth + 1);
                if (!val) return nullptr;
                n->obj[key->sVal] = val;
                skipWs(p);
                if (*p == ',') { p++; continue; }
                if (*p == '}') { p++; break; }
                return nullptr;
            }
            return n;
        }
        if (*p == '[') {
            p++;
            auto n = std::make_shared<JsonNode>(JsonNode::Array);
            skipWs(p);
            if (*p == ']') { p++; return n; }
            while (true) {
                auto val = parseValue(p, depth + 1);
                if (!val) return nullptr;
                n->arr.push_back(val);
                skipWs(p);
                if (*p == ',') { p++; continue; }
                if (*p == ']') { p++; break; }
                return nullptr;
            }
            return n;
        }
        if (strncmp(p, "true", 4) == 0) { p += 4; auto n = std::make_shared<JsonNode>(JsonNode::Bool); n->bVal = true; return n; }
        if (strncmp(p, "false", 5) == 0) { p += 5; auto n = std::make_shared<JsonNode>(JsonNode::Bool); n->bVal = false; return n; }
        if (strncmp(p, "null", 4) == 0) { p += 4; return std::make_shared<JsonNode>(); }
        if (*p == '-' || (*p >= '0' && *p <= '9')) return parseNumber(p);
        return nullptr;
    }
}

inline DeserializationError deserializeJson(JsonDocument& doc, const char* json) {
    if (!json || !*json) return DeserializationError(DeserializationError_EmptyInput);
    const char* p = json;
    auto node = _jsonparse::parseValue(p);
    if (!node) return DeserializationError(DeserializationError_InvalidInput);
    doc.node_ = node;
    return DeserializationError(DeserializationError_Ok);
}

inline DeserializationError deserializeJson(JsonDocument& doc, const std::string& json) {
    return deserializeJson(doc, json.c_str());
}
