//cpp
namespace cstd {
int strcmp(char const *a, char const *b) {
    unsigned int wb, wa, t;
    unsigned int cnt;
    unsigned int align_a, align_b;
    int diff;
    unsigned char c3, cb, ca;

    ca = (unsigned char)*a;
    cb = (unsigned char)*b;
    diff = (int)ca - (int)cb;
    if (diff != 0)
        return diff;

    align_a = (unsigned int)a & 3;
    align_b = (unsigned int)b & 3;
    if (align_b != align_a)
        goto scalar_check;

    if (align_a == 0)
        goto word_setup;

    if (ca == 0)
        return 0;
    cnt = 3 - align_a;
    if (cnt == 0)
        goto align_incr;
align_loop:
    c3 = *(unsigned char *)(++a);
    cb = *(unsigned char *)(++b);
    diff = (int)c3 - (int)cb;
    if (diff != 0)
        return diff;
    if (c3 == 0)
        return 0;
    if (--cnt != 0)
        goto align_loop;
align_incr:
    ++a;
    ++b;

word_setup:
    wa = *(unsigned int *)a;
    t = (wa + 0xfefefeff) & 0x80808080;
    wb = *(unsigned int *)b;
    if (t != 0)
        goto scalar_reload;
    if (wa != wb)
        goto final_cmp;
word_loop:
    wa = *(unsigned int *)(a += 4);
    wb = *(unsigned int *)(b += 4);
    t = (wa + 0xfefefeff) & 0x80808080;
    if (t != 0)
        goto scalar_reload;
    if (wa == wb)
        goto word_loop;
final_cmp:
    return (wa > wb) ? 1 : -1;

scalar_reload:
    ca = *(unsigned char *)a;
    cb = *(unsigned char *)b;
    diff = (int)ca - (int)cb;
    if (diff != 0)
        return diff;
scalar_check:
    if (ca == 0)
        return 0;
scalar_loop:
    c3 = *(unsigned char *)(++a);
    cb = *(unsigned char *)(++b);
    diff = (int)c3 - (int)cb;
    if (diff != 0)
        return diff;
    if (c3 == 0)
        return 0;
    goto scalar_loop;
}
}
