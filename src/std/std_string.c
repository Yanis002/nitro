#include "nitro/std.h"

char *STD_CopyString(char *dst, const char *src) {
    const char *iter;
    char *result;

    iter   = src;
    result = dst;
    while (*iter != '\0') {
        *dst++ = *iter++;
    }
    *dst = '\0';
    return result;
}

s32 STD_CopyLString(char *dst, const char *src, s32 size) {
    s32 i;
    const char *iter;

    iter = src;
    for (i = 0; i < size - 1; ++i) {
        dst[i] = *iter;
        if (*iter == '\0') {
            break;
        }
        ++iter;
    }
    if (i >= size - 1 && size != 0) {
        dst[i] = 0;
    }
    return STD_GetStringLength(src);
}

const char *STD_SearchString(const char *haystack, const char *needle) {
    s32 j;
    const char *iter;
    s32 i;

    i = 0;
    while (haystack[i] != '\0') {
        j    = 0;
        iter = &haystack[i];
        while (needle[j] != '\0' && *iter == needle[j]) {
            ++iter;
            ++j;
        }
        if (needle[j] == '\0') {
            return &haystack[i];
        }
        ++i;
    }
    return NULL;
}

s32 STD_GetStringLength(const char *str) {
    s32 i;

    i = 0;
    while (str[i] != '\0') {
        ++i;
    }
    return i;
}

char *STD_ConcatenateString(char *dst, const char *src) {
    STD_CopyString(dst + STD_GetStringLength(dst), src);
    return dst;
}

s32 STD_CompareString(const char *lhs, const char *rhs) {
    while (*lhs == *rhs && *lhs != '\0') {
        ++lhs;
        ++rhs;
    }
    return *lhs - *rhs;
}

s32 STD_CompareNString(const char *lhs, const char *rhs, s32 length) {
    s32 i;
    u8 left;
    u8 right;

    if (length != 0) {
        for (i = 0; i < length; ++i) {
            left  = lhs[i];
            right = rhs[i];
            if (left != right) {
                return left - right;
            }
        }
    }
    return 0;
}
