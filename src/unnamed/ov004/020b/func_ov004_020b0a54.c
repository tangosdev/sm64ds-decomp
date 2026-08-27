extern void func_ov004_020b87e0(void* a, void* b);
extern void* data_ov004_020beb68;
void func_ov004_020b0a54(void* c)
{
    void* g = data_ov004_020beb68;
    if (!g) return;
    func_ov004_020b87e0((char*)g + 0xcc, c);
    *(int*)((char*)data_ov004_020beb68 + 0x60) = 0;
}
