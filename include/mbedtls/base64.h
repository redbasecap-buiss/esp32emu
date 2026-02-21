#pragma once
// esp32emu — mbedtls base64 encode/decode mock

#include <cstddef>
#include <cstdint>
#include <cstring>

#define MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL  -0x002A
#define MBEDTLS_ERR_BASE64_INVALID_CHARACTER -0x002C

namespace esp32emu_base64 {

static const char table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

inline int decode_char(unsigned char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    return -1;
}

} // namespace esp32emu_base64

inline int mbedtls_base64_encode(unsigned char* dst, size_t dlen, size_t* olen,
                                  const unsigned char* src, size_t slen) {
    *olen = ((slen + 2) / 3) * 4;
    if (dlen < *olen + 1) return MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL;

    size_t j = 0;
    for (size_t i = 0; i < slen; i += 3) {
        uint32_t v = (uint32_t)src[i] << 16;
        if (i + 1 < slen) v |= (uint32_t)src[i + 1] << 8;
        if (i + 2 < slen) v |= (uint32_t)src[i + 2];

        dst[j++] = esp32emu_base64::table[(v >> 18) & 0x3F];
        dst[j++] = esp32emu_base64::table[(v >> 12) & 0x3F];
        dst[j++] = (i + 1 < slen) ? esp32emu_base64::table[(v >> 6) & 0x3F] : '=';
        dst[j++] = (i + 2 < slen) ? esp32emu_base64::table[v & 0x3F] : '=';
    }
    dst[j] = '\0';
    return 0;
}

inline int mbedtls_base64_decode(unsigned char* dst, size_t dlen, size_t* olen,
                                  const unsigned char* src, size_t slen) {
    // Skip padding/whitespace for length calculation
    size_t pad = 0;
    if (slen > 0 && src[slen - 1] == '=') pad++;
    if (slen > 1 && src[slen - 2] == '=') pad++;

    *olen = (slen / 4) * 3 - pad;
    if (dlen < *olen) return MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL;

    size_t j = 0;
    for (size_t i = 0; i < slen; i += 4) {
        int a = esp32emu_base64::decode_char(src[i]);
        int b = (i + 1 < slen) ? esp32emu_base64::decode_char(src[i + 1]) : 0;
        int c = (i + 2 < slen && src[i + 2] != '=') ? esp32emu_base64::decode_char(src[i + 2]) : 0;
        int d = (i + 3 < slen && src[i + 3] != '=') ? esp32emu_base64::decode_char(src[i + 3]) : 0;

        if (a < 0 || b < 0 || c < 0 || d < 0) return MBEDTLS_ERR_BASE64_INVALID_CHARACTER;

        uint32_t v = (a << 18) | (b << 12) | (c << 6) | d;
        if (j < *olen) dst[j++] = (v >> 16) & 0xFF;
        if (j < *olen) dst[j++] = (v >> 8) & 0xFF;
        if (j < *olen) dst[j++] = v & 0xFF;
    }
    return 0;
}
