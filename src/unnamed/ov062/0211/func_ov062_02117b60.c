extern int Vec3_Dist(void* a, void* b);
int func_ov062_02117b60(void* c)
{
    void* r1 = *(void**)((char*)c + 0x3b4);
    if (!r1) return 0x61a8000;
    return Vec3_Dist((char*)c + 0x5c, (char*)r1 + 0x5c);
}
