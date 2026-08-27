extern void* _ZN8dActor_c15FindWithActorIDEjPS_(unsigned int, void*);
int func_ov002_020b9f00(char* r5) {
    void* r1;
    if (*(int*)(r5 + 0x344)) return 0;
    r1 = _ZN8dActor_c15FindWithActorIDEjPS_(0xb2, 0);
    while (r1) {
        if (*(unsigned char*)(r5 + 0x351) == *(unsigned char*)((char*)r1 + 0x49d)) {
            *(int*)(r5 + 0x344) = *(int*)((char*)r1 + 4);
            return 1;
        }
        r1 = _ZN8dActor_c15FindWithActorIDEjPS_(0xb2, r1);
    }
    return 0;
}
