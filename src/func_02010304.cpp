//cpp
// func_02010304 at 0x02010304
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
extern "C" {
extern void *_ZN5Actor10FindWithIDEj(unsigned);
extern int _ZN6Player7TryGrabER5Actor(void *, void *);

void *func_02010304(void *target, char *p) {
    unsigned id = *(unsigned *)(p + 0x24);
    void *found;
    if (id != 0 && (found = _ZN5Actor10FindWithIDEj(id)) != 0 && (*(int *)(p + 0x20) & 0x1000)) {
        int t = *(unsigned short *)((char *)found + 0xc) == 0xbf;
        if (t && _ZN6Player7TryGrabER5Actor(found, target))
            return found;
    }
    return 0;
}
}
