int func_02021a04(char* table, char* n);

typedef int (*Method)(void* self, int arg, int m);
typedef struct { Method m[1]; } VT;
typedef struct { VT* vt; } VObj;
typedef struct {
    int f0;
    char pad4[6];
    unsigned char fa;
    char padb;
    int fc;
    VObj* f10;
} Entry;

void func_02021bec(char* obj)
{
    Entry* e = (Entry*)(obj + 8);
    int i;
    for (i = 0; i < 0x40; i++, e = (Entry*)((char*)e + 0x1c)) {
        int b;
        if (e->f0 == 0) continue;
        if (e->f10 != 0) {
            int m = (int)(e->fa == 1);
            if (e->f10->vt->m[1](e->f10, e->fc, m) != 0)
                e->fa = 1;
        }
        b = (int)(e->fa == 1);
        if (b != 0) {
            e->fa = 0;
            continue;
        }
        func_02021a04(obj, (char*)e);
        obj[4] = (char)i;
    }
}
