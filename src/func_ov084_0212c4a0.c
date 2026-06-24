extern void* _ZN5Actor15FindWithActorIDEjPS_(unsigned int, void*);
void func_ov084_0212c4a0(char* r5) {
    void* r1;
    r1 = _ZN5Actor15FindWithActorIDEjPS_(0xe, 0);
    while (r1) {
        if ((*(int*)((char*)r1 + 8) & 0xff) == 1) {
            if (*(unsigned char*)((char*)r1 + 0x32e) == 0) {
                *(int*)(r5 + 0x198) = *(int*)((char*)r1 + 4);
            }
        }
        r1 = _ZN5Actor15FindWithActorIDEjPS_(0xe, r1);
    }
    *(unsigned char*)(r5 + 0x1e8) = 0;
}
