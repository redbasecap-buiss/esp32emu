#pragma once
#include <cstdint>
#include <cstdio>
#include <functional>
#include <map>
#include <string>
#include <vector>

// Forward declarations
class AsyncWebServerRequest;
class AsyncWebServer;

// ── AsyncWebServerResponse ──────────────────────────────────────────
class AsyncWebServerResponse {
public:
    AsyncWebServerResponse(int code, const std::string& contentType, const std::string& body)
        : code_(code), contentType_(contentType), body_(body) {}

    void addHeader(const std::string& name, const std::string& value) {
        headers_[name] = value;
    }

    int code() const { return code_; }
    const std::string& contentType() const { return contentType_; }
    const std::string& body() const { return body_; }
    const std::map<std::string, std::string>& headers() const { return headers_; }

private:
    int code_;
    std::string contentType_;
    std::string body_;
    std::map<std::string, std::string> headers_;
};

// ── AsyncWebServerRequest ───────────────────────────────────────────
class AsyncWebServerRequest {
public:
    AsyncWebServerRequest() = default;

    // Emulator-only: set request details
    void setMethod(const std::string& m) { method_ = m; }
    void setUrl(const std::string& u) { url_ = u; }
    void setParam(const std::string& name, const std::string& value) { params_[name] = value; }
    void setHeader(const std::string& name, const std::string& value) { reqHeaders_[name] = value; }
    void setBody(const std::string& b) { body_ = b; }

    const std::string& method() const { return method_; }
    const std::string& url() const { return url_; }

    bool hasParam(const std::string& name) const { return params_.count(name) > 0; }
    std::string getParam(const std::string& name) const {
        auto it = params_.find(name);
        return it != params_.end() ? it->second : "";
    }

    bool hasHeader(const std::string& name) const { return reqHeaders_.count(name) > 0; }
    std::string header(const std::string& name) const {
        auto it = reqHeaders_.find(name);
        return it != reqHeaders_.end() ? it->second : "";
    }

    const std::string& body() const { return body_; }

    void send(int code, const std::string& contentType = "text/plain", const std::string& body = "") {
        lastResponse_ = {code, contentType, body};
        responded_ = true;
        fprintf(stderr, "[esp32emu] AsyncWebServer: %s %s → %d\n", method_.c_str(), url_.c_str(), code);
    }

    void send(AsyncWebServerResponse* response) {
        if (response) {
            lastResponse_ = {response->code(), response->contentType(), response->body()};
            responded_ = true;
            fprintf(stderr, "[esp32emu] AsyncWebServer: %s %s → %d\n", method_.c_str(), url_.c_str(), response->code());
        }
    }

    void redirect(const std::string& url) {
        lastResponse_ = {302, "text/html", ""};
        lastResponse_.addHeader("Location", url);
        responded_ = true;
        fprintf(stderr, "[esp32emu] AsyncWebServer: %s %s → 302 → %s\n", method_.c_str(), url_.c_str(), url.c_str());
    }

    AsyncWebServerResponse* beginResponse(int code, const std::string& contentType, const std::string& body = "") {
        return new AsyncWebServerResponse(code, contentType, body);
    }

    // Emulator-only: inspect response
    bool responded() const { return responded_; }
    const AsyncWebServerResponse& lastResponse() const { return lastResponse_; }

private:
    std::string method_ = "GET";
    std::string url_ = "/";
    std::string body_;
    std::map<std::string, std::string> params_;
    std::map<std::string, std::string> reqHeaders_;
    AsyncWebServerResponse lastResponse_{0, "", ""};
    bool responded_ = false;
};

// ── WebRequestMethod ────────────────────────────────────────────────
enum WebRequestMethod {
    HTTP_GET     = 0x01,
    HTTP_POST    = 0x02,
    HTTP_DELETE  = 0x04,
    HTTP_PUT     = 0x08,
    HTTP_PATCH   = 0x10,
    HTTP_HEAD    = 0x20,
    HTTP_OPTIONS = 0x40,
    HTTP_ANY     = 0x7F
};

// ── AsyncWebServer ──────────────────────────────────────────────────
using ArRequestHandlerFunction = std::function<void(AsyncWebServerRequest*)>;
using ArBodyHandlerFunction = std::function<void(AsyncWebServerRequest*, uint8_t*, size_t, size_t, size_t)>;

class AsyncWebServer {
public:
    explicit AsyncWebServer(uint16_t port) : port_(port) {}

    void begin() {
        running_ = true;
        fprintf(stderr, "[esp32emu] AsyncWebServer: listening on port %d\n", port_);
    }

    void end() {
        running_ = false;
        fprintf(stderr, "[esp32emu] AsyncWebServer: stopped\n");
    }

    void on(const char* uri, int method, ArRequestHandlerFunction handler) {
        routes_.push_back({uri, method, handler, nullptr});
    }

    void on(const char* uri, int method, ArRequestHandlerFunction handler, ArBodyHandlerFunction bodyHandler) {
        routes_.push_back({uri, method, handler, bodyHandler});
    }

    // Convenience: GET handler
    void on(const char* uri, ArRequestHandlerFunction handler) {
        on(uri, HTTP_GET, handler);
    }

    void onNotFound(ArRequestHandlerFunction handler) {
        notFoundHandler_ = handler;
    }

    bool isRunning() const { return running_; }
    uint16_t port() const { return port_; }

    // Emulator-only: simulate a request and trigger the matching handler
    bool simulateRequest(AsyncWebServerRequest* req) {
        int methodBit = methodStringToBit(req->method());
        for (auto& route : routes_) {
            if (matchUri(route.uri, req->url()) && (route.method & methodBit)) {
                route.handler(req);
                return true;
            }
        }
        if (notFoundHandler_) {
            notFoundHandler_(req);
            return true;
        }
        return false;
    }

    // Emulator-only: get registered route count
    size_t routeCount() const { return routes_.size(); }

private:
    struct Route {
        std::string uri;
        int method;
        ArRequestHandlerFunction handler;
        ArBodyHandlerFunction bodyHandler;
    };

    uint16_t port_;
    bool running_ = false;
    std::vector<Route> routes_;
    ArRequestHandlerFunction notFoundHandler_;

    static int methodStringToBit(const std::string& m) {
        if (m == "GET")     return HTTP_GET;
        if (m == "POST")    return HTTP_POST;
        if (m == "PUT")     return HTTP_PUT;
        if (m == "DELETE")  return HTTP_DELETE;
        if (m == "PATCH")   return HTTP_PATCH;
        if (m == "HEAD")    return HTTP_HEAD;
        if (m == "OPTIONS") return HTTP_OPTIONS;
        return HTTP_ANY;
    }

    static bool matchUri(const std::string& pattern, const std::string& uri) {
        if (pattern == uri) return true;
        // Wildcard: /api/* matches /api/anything
        if (pattern.size() > 1 && pattern.back() == '*') {
            return uri.substr(0, pattern.size() - 1) == pattern.substr(0, pattern.size() - 1);
        }
        return false;
    }
};
