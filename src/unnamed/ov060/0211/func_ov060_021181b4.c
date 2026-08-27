extern void* _ZN8dActor_c15FindWithActorIDEjPS_(unsigned int id, void* p);
int func_ov060_021181b4(char* c) {
    void* a = _ZN8dActor_c15FindWithActorIDEjPS_(0x117, 0);
    if (a) {
        *(int*)(c + 0x320) = *(int*)((char*)a + 4);
        *(unsigned char*)(c + 0x328) = 1;
        return 1;
    }
    return (int)a;
}
