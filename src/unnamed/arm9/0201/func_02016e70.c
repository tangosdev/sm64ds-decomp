typedef struct Entry { int pad; void *data; void *buf; } Entry;

extern Entry *_ZN5Model23AddToCommonModelDataArrER8BMD_File(void *file);
extern void *_Znwj(unsigned int sz);
extern unsigned int func_02046564(void *file);
extern void *_ZN6Memory13operator_new2Ej(unsigned int sz);
extern void _ZN6Memory16operator_delete2EPv(void *p);
extern void func_020462d0(void *d, void *file, void *buf);

void *func_02016e70(void *file)
{
    Entry *e;
    void *b;

    e = _ZN5Model23AddToCommonModelDataArrER8BMD_File(file);
    if (e->data == 0) {
        e->data = _Znwj(0x14);
        if (e->data == 0) return 0;
        e->buf = _ZN6Memory13operator_new2Ej(func_02046564(file));
        b = e->buf;
        if (b == 0) {
            _ZN6Memory16operator_delete2EPv(e->data);
            e->data = 0;
            return 0;
        }
        func_020462d0(e->data, file, b);
    }
    return e->data;
}
