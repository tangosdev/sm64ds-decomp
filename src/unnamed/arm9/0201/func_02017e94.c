extern void func_0205dc0c(int a, int b);
extern void func_02018c00(void *rgn, int a, int b);
extern int func_0203d7b8(void);
extern void func_02017fd0(void *rgn);

typedef struct Entry { int a; int b; int c; } Entry;

extern Entry data_0209d3c4[];

void func_02017e94(int key)
{
    char rgn[0x28];
    Entry *found = 0;
    unsigned int i;
    for (i = 0; i < 0xc; i++) {
        Entry *e = &data_0209d3c4[i];
        if (e->a == key) {
            e->a = -1;
            e->b = 0;
            found = e;
            e->c = 0;
        }
    }
    if (found == 0) return;
    func_0205dc0c(0, key);
    func_02018c00(rgn, 0, key);
    if (func_0203d7b8() != 0) return;
    func_02017fd0(rgn);
}
