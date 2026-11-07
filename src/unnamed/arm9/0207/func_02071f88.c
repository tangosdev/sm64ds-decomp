/* func_02071f88 at 0x02071f88
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
typedef struct {
    unsigned int count;
    int pad4;
    int pad8;
    unsigned char *data;
} List;

extern int __throw_catch_compare(unsigned char *a, unsigned char *b, int *out);

int func_02071f88(void *a, List *list)
{
    unsigned char *p;
    unsigned int i;
    unsigned int buf[2];

    p = list->data;
    for (i = 0; i < list->count; i++) {
        unsigned int val = p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);
        buf[0] = val;
        if (__throw_catch_compare((unsigned char *)a, (unsigned char *)val, (int *)&buf[1]))
            return 1;
        p += 4;
    }
    return 0;
}
