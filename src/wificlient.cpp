#include "WiFiClient.h"
#include "esp32emu_string.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <cerrno>
#include <cstdio>
#include <fcntl.h>
#include <poll.h>

WiFiClient::WiFiClient() {}
WiFiClient::WiFiClient(const char* host, uint16_t port, int fd)
    : fd_(fd) { (void)host; (void)port; }
WiFiClient::~WiFiClient() { stop(); }

int WiFiClient::connect(const char* host, uint16_t port) {
    return connect(host, port, timeout_ms_);
}

int WiFiClient::connect(const char* host, uint16_t port, int timeout_ms) {
    struct addrinfo hints{}, *res;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    char port_str[8];
    snprintf(port_str, sizeof(port_str), "%d", port);

    if (getaddrinfo(host, port_str, &hints, &res) != 0) return 0;

    fd_ = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (fd_ < 0) { freeaddrinfo(res); return 0; }

    // Non-blocking connect with timeout
    if (timeout_ms > 0) {
        int flags = fcntl(fd_, F_GETFL, 0);
        fcntl(fd_, F_SETFL, flags | O_NONBLOCK);

        int rc = ::connect(fd_, res->ai_addr, res->ai_addrlen);
        if (rc < 0 && errno == EINPROGRESS) {
            struct pollfd pfd{};
            pfd.fd = fd_;
            pfd.events = POLLOUT;
            rc = poll(&pfd, 1, timeout_ms);
            if (rc <= 0) {
                ::close(fd_); fd_ = -1; freeaddrinfo(res); return 0;
            }
            int err = 0; socklen_t len = sizeof(err);
            getsockopt(fd_, SOL_SOCKET, SO_ERROR, &err, &len);
            if (err != 0) {
                ::close(fd_); fd_ = -1; freeaddrinfo(res); return 0;
            }
        } else if (rc < 0) {
            ::close(fd_); fd_ = -1; freeaddrinfo(res); return 0;
        }
        // Restore blocking mode
        fcntl(fd_, F_SETFL, flags);
    } else {
        if (::connect(fd_, res->ai_addr, res->ai_addrlen) < 0) {
            ::close(fd_); fd_ = -1; freeaddrinfo(res); return 0;
        }
    }

    freeaddrinfo(res);

    struct timeval tv{};
    tv.tv_sec = timeout_ms_ / 1000;
    tv.tv_usec = (timeout_ms_ % 1000) * 1000;
    setsockopt(fd_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    return 1;
}

size_t WiFiClient::write(const uint8_t* buf, size_t size) {
    if (fd_ < 0) return 0;
    ssize_t n = ::send(fd_, buf, size, 0);
    return n > 0 ? (size_t)n : 0;
}

size_t WiFiClient::print(const char* s) {
    return write((const uint8_t*)s, strlen(s));
}

size_t WiFiClient::println(const char* s) {
    size_t n = print(s);
    n += write((const uint8_t*)"\r\n", 2);
    return n;
}

int WiFiClient::available() {
    if (fd_ < 0) return 0;
    char c;
    int n = recv(fd_, &c, 1, MSG_PEEK);
    return n > 0 ? 1 : 0;
}

int WiFiClient::read() {
    if (fd_ < 0) return -1;
    uint8_t c;
    ssize_t n = recv(fd_, &c, 1, 0);
    return n > 0 ? c : -1;
}

int WiFiClient::read(uint8_t* buf, size_t size) {
    if (fd_ < 0) return -1;
    ssize_t n = recv(fd_, buf, size, 0);
    return (int)n;
}

String WiFiClient::readString() {
    std::string result;
    char buf[1024];
    while (true) {
        ssize_t n = recv(fd_, buf, sizeof(buf), 0);
        if (n <= 0) break;
        result.append(buf, n);
    }
    return String(result.c_str());
}

String WiFiClient::readStringUntil(char terminator) {
    std::string result;
    char c;
    while (true) {
        ssize_t n = recv(fd_, &c, 1, 0);
        if (n <= 0) break;
        if (c == terminator) break;
        result += c;
    }
    return String(result.c_str());
}

bool WiFiClient::connected() {
    return fd_ >= 0;
}

void WiFiClient::stop() {
    if (fd_ >= 0) {
        ::close(fd_);
        fd_ = -1;
    }
}
