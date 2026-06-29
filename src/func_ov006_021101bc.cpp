//cpp
extern "C" {
extern void func_ov006_0211474c(char* c, int a1, int a2, int* a3);
extern void func_020733a8(void* arr, int count, int size, void(*ctor)(void*), void(*dtor)(void*));
extern void *data_ov006_0213ed74[];
extern void func_0203d738(void*);
extern void func_0203d47c(void*);
void* func_ov006_021101bc(char* c, int a1, int a2, int* src);
void* func_ov006_021101bc(char* c, int a1, int a2, int* src){
    int tmp[2];
    tmp[0] = src[0];
    tmp[1] = src[1];
    func_ov006_0211474c(c, a1, a2, tmp);
    *(void***)c = (void**)data_ov006_0213ed74;
    func_020733a8(c + 0x34, 3, 8, func_0203d738, func_0203d47c);
    func_020733a8(c + 0x4c, 3, 8, func_0203d738, func_0203d47c);
    *(int*)(c + 0x28) = 0x28000;
    return c;
}
}
