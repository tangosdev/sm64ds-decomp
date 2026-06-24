extern void* _ZN5Actor15FindWithActorIDEjPS_(unsigned int, void*);
void func_ov098_0213a0e8(char* r5) {
    void* r1;
    r1 = _ZN5Actor15FindWithActorIDEjPS_(0xb2, 0);
    while (r1) {
        if (*(unsigned char*)((char*)r1 + 0x49d) == (*(int*)(r5 + 8) & 0xf)) {
            *(int*)(r5 + 0x330) = *(int*)((char*)r1 + 4);
        }
        r1 = _ZN5Actor15FindWithActorIDEjPS_(0xb2, r1);
    }
}
