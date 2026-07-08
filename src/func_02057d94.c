typedef struct {
    unsigned int count;
    char *buf;
} RingS;

void func_02057d94(RingS *s, char c, int len) {
    unsigned int i;
    unsigned int n;
    if (len <= 0) return;
    n = s->count;
    if (n > (unsigned int)len) n = (unsigned int)len;
    for (i = 0; i < n; i++) {
        s->buf[i] = c;
    }
    s->count -= n;
    *(int *)(((long long)(int)((char *)s + 4)) & 0xFFFFFFFFFFFFFFFFLL) += len;
}
