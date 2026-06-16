extern void* _ZN5Actor15FindWithActorIDEjPS_(unsigned int, void*);
void func_ov063_02119c18(void* c, unsigned int id) {
    void* r = (void*)*(int*)((char*)c + 0x498);
    if (r != 0) return;
    void* a = _ZN5Actor15FindWithActorIDEjPS_(id, 0);
    *(void**)((char*)c + 0x48c) = a;
    a = *(void**)((char*)c + 0x48c);
    if (a != 0) {
        *(void**)((char*)c + 0x498) = *(void**)((char*)a + 4);
    }
}
