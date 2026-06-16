typedef unsigned int u32;
extern char* LoadFile(int id);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(u32 a, u32 b);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void* p, u32 a, u32 b);
extern void Deallocate(void* ptr);
extern u32 data_ov005_020c2310[];
void func_ov005_020c1688(char* r6, int r5){
  char* r4 = LoadFile(data_ov005_020c2310[r5]);
  _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)r4, 0x200);
  u32 v = *(u32*)(r6 + (r5/4)*4 + 0x68);
  _ZN3GXS10LoadBGPlttEPKvjj(r4+0x20, v << 5, 0x20);
  Deallocate(r4);
}
