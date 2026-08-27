//cpp
namespace cstd {
char *strchr(const char *s, char ch_in) {
    unsigned char ch = (unsigned char)ch_in;
    unsigned char c = *(const unsigned char *)s;
    const char *p = s + 1;
    if (c != 0) {
        do {
            if (c == ch) return (char *)(p - 1);
            c = *(const unsigned char *)p++;
        } while (c != 0);
    }
    if (ch != 0) return 0;
    return (char *)(p - 1);
}
}
