//cpp
/* SharedFilePtr::Load() at 0x02017c54
 *
 * Resolve the file for this->fileID (either already resident in the
 * overlay-file table, or streamed from card), decompress it if LZ16
 * compressed, and stash + return the resulting buffer in this->filePtr.
 *
 * Structurally mirrors the already-matched func_0201818c/func_0201834c
 * pair (same helper calls, same Obj context layout) but inlined into a
 * single member function instead of split across two free functions.
 */

struct Obj { char pad[0x20]; unsigned int cur; unsigned int end; char pad2[0x1c]; };

extern "C" {
extern unsigned int data_0209d3bc;

int func_020186c0(unsigned int val);
int func_02018568(int a);
unsigned int func_02018ac4(unsigned int *p);
int _ZN6Memory8AllocateEj(unsigned int size);
void DecompressLZ16(void *src, void *dst);
void func_020185c0(void *buf, unsigned int x);
void func_0205d4cc(void *a);
int _ZN6Memory8AllocateEji(unsigned int size, int align);
void func_0205a61c(void *a, void *b, unsigned int c);
void func_02018770(void);
void _ZN4CP1514FlushDataCacheEjj(unsigned int a, unsigned int b);
void Crash(void);
void _ZN4CP1527FlushAndInvalidateDataCacheEjj(unsigned int a, unsigned int b);
int func_02018d48(void *a, void *b, unsigned int c);
void _ZN6Memory10DeallocateEPv(void *p);
}

struct SharedFilePtr {
    unsigned short fileID;
    unsigned char numRefs;
    void *filePtr;

    void *Load();
};

void *SharedFilePtr::Load()
{
    data_0209d3bc = fileID;
    int p = func_020186c0(fileID);
    if (p != 0) {
        unsigned int size;
        int mem;
        if (func_02018568(p)) {
            unsigned int *q = (unsigned int *)(p + 4);
            size = func_02018ac4(q);
            mem = _ZN6Memory8AllocateEj(size);
            DecompressLZ16(q, (void *)mem);
        } else {
            struct Obj obj;
            func_020185c0(&obj, fileID);
            size = obj.end - obj.cur;
            func_0205d4cc(&obj);
            mem = _ZN6Memory8AllocateEji(size, 0x20);
            func_0205a61c((void *)p, (void *)mem, size);
        }
        func_02018770();
        _ZN4CP1514FlushDataCacheEjj((unsigned int)mem, size);
        filePtr = (void *)mem;
        return filePtr;
    }

    struct Obj obj2;
    void *cp;
    unsigned int csize;
    int raw;
    int fsize;

    func_020185c0(&obj2, fileID);
    fsize = obj2.end - obj2.cur;
    if (fsize <= 8)
        Crash();
    raw = _ZN6Memory8AllocateEji(fsize, 0x20);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj((unsigned int)raw, fsize);
    int r = func_02018d48(&obj2, (void *)raw, fsize);
    if (r != fsize)
        return 0;
    func_0205d4cc(&obj2);
    func_02018770();
    if (func_02018568(raw) != 0) {
        unsigned int t;
        cp = (void *)(raw + 4);
        t = func_02018ac4((unsigned int *)cp);
        csize = fsize - 4;
        fsize = t;
        int tmp = _ZN6Memory8AllocateEj(csize);
        func_0205a61c(cp, (void *)tmp, csize);
        _ZN6Memory10DeallocateEPv((void *)raw);
        raw = _ZN6Memory8AllocateEj(fsize);
        DecompressLZ16((void *)tmp, (void *)raw);
        _ZN6Memory10DeallocateEPv((void *)tmp);
    }
    _ZN4CP1514FlushDataCacheEjj((unsigned int)raw, fsize);
    filePtr = (void *)raw;
    return filePtr;
}
