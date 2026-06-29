/* func_ov002_020e930c at 0x020e930c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
 */
typedef unsigned char u8;

extern void* _ZN5Actor10FindWithIDEj(unsigned int id);
extern int _ZN5Event6GetBitEj(unsigned int bit);
extern int func_ov002_020e8ef0(void* a, void* b);
extern void func_ov002_020e7d84(void);

void func_ov002_020e930c(void* self) {
    char* a = (char*)self;
    void* o;
    char* b;
    int flags;
    unsigned int id;

    id = *(unsigned int*)(a + 0x134);
    if (id == 0) return;
    o = _ZN5Actor10FindWithIDEj(id);
    if (o == 0) return;
    b = (char*)o;

    flags = *(int*)(a + 0x130);
    if (flags & 0x400000) {
        if (*(u8*)(b + 0x709) != 0) return;
        if (_ZN5Event6GetBitEj(0x1e) != 0) return;
        if (func_ov002_020e8ef0(self, o) == 0) return;
        if (*(int*)(a + 0x43c) != 6) return;
        if (_ZN5Actor10FindWithIDEj(*(unsigned int*)(a + 0x434)) == 0) return;
        func_ov002_020e7d84();
    } else {
        if (flags & 0x8000) {
            if (*(u8*)(b + 0x709) != 0) return;
            if (_ZN5Event6GetBitEj(0x1e) != 0) return;
            if (*(int*)(a + 0x43c) != 6) return;
            if (_ZN5Actor10FindWithIDEj(*(unsigned int*)(a + 0x434)) == 0) return;
            func_ov002_020e7d84();
        }
    }
}
