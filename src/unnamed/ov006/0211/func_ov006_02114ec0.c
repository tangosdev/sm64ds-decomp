/* func_ov006_02114ec0 at 0x02114ec0
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov006).
 */
extern void func_ov006_02111e7c(int *o);
extern void func_ov006_0211470c(int *a, int *b);
extern void func_02012718(void *a, int b);
typedef struct { int *p[13]; } Arr;
static inline int *get(char *self, int i){ return i >= 13 ? 0 : ((Arr*)(self + 0x4688))->p[i]; }
void func_ov006_02114ec0(char *self){
    int t[3]; int n = *(int *)(self + 0x4668); int i;
    for (i = 0; i < n; i++) {
        if (*(unsigned char *)((char *)get(self, i) + 0x30) == 0) continue;
        if (*(unsigned char *)((char *)get(self, i) + 0x121) != 0) continue;
        func_ov006_02111e7c(get(self, i));
        func_ov006_0211470c(t, get(self, i));
        func_02012718((void *)0x1a5, t[0]); return;
    }
}
