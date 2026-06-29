/* func_ov006_02107c50 at 0x02107c50
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov006).
 */
extern int func_ov004_020ad674(void);
extern void func_ov004_020af68c(void* a0, int a1, int a2, int a3, int a4);
extern void func_ov004_020b0104(void* a0, int a1, int a2, int a3, int a4, void* a5);
extern void func_ov004_020b2444(int a, int b, int c, int d, int e, int f, int g);

extern void* data_ov006_0213e320[];
extern int data_ov006_0212ed14[];
extern void* data_ov006_02138c3c;

void func_ov006_02107c50(short* a0, int a1)
{
    int idx = func_ov004_020ad674();
    void* p = *(void**)((char*)data_ov006_0213e320[idx] + 0x50);
    func_ov004_020af68c(p, 0xc, a1, -1, -1);

    func_ov004_020b2444(0x1e, a1, data_ov006_0212ed14[*(short*)((char*)a0 + 0x14)], 0, -1, 2, 0);

    {
        int b = a1 + 0x10;
        func_ov004_020b0104(data_ov006_02138c3c, 0x20, b, -1, 1, (void*)0);
        func_ov004_020b2444(0x2e, b, *(short*)((char*)a0 + 0x16), 0, -1, 1, 0);
    }
}
