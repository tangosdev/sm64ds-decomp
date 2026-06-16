extern int _ZN5Actor15FindWithActorIDEjPS_(unsigned int, void*);
void* func_ov002_020b1328(void* r5) {
    void* r1 = 0;
    while (1) {
        r1 = (void*)_ZN5Actor15FindWithActorIDEjPS_(0xb4, r1);
        if (!r1) break;
        if (*(unsigned char*)((char*)r5 + 0x3ab) == *(unsigned char*)((char*)r1 + 0x1d9))
            if (*(unsigned char*)((char*)r1 + 0x1d8) == 0)
                return r1;
    }
    return 0;
}
