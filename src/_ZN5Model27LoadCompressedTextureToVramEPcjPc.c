// NONMATCHING: tail RMW-pair schedule (div=5, was 14). Logic verified correct vs ROM.
// #pragma opt_common_subs off + volatile RMWs give the per-block reloads; residual is
// the last two global increments: ROM batches both loads (c8->r0, e0->r3) then
// add r4/str/add r2/str, mwccarm either fully serializes (volatile, div 5) or batches
// adds before both stores (plain/temps, div 8). Swept: fake-dep ternary, na-reuse,
// mixed volatile, volatile loads+stores. Counts as decompiled, not matched.
extern unsigned int data_020a4be8;
extern unsigned int data_020a4bc8;
extern unsigned int data_020a4be4;
extern unsigned int data_020a4be0;
void Crash();
void _ZN2GX12BeginLoadTexEv();
void _ZN2GX7LoadTexEPKvjj(const void*, unsigned int, unsigned int);
void _ZN2GX10EndLoadTexEv();

#pragma opt_common_subs off
void _ZN5Model27LoadCompressedTextureToVramEPcjPc(char* src, unsigned int size, char* dst){
  if ((data_020a4be8 - data_020a4bc8) < size) Crash();
  _ZN2GX12BeginLoadTexEv();
  _ZN2GX7LoadTexEPKvjj(src, data_020a4bc8, size);
  data_020a4be4 += size;
  _ZN2GX7LoadTexEPKvjj(dst, data_020a4be0, size >> 1);
  data_020a4be4 += size >> 1;
  _ZN2GX10EndLoadTexEv();
  *(volatile unsigned int*)&data_020a4bc8 += size;
  *(volatile unsigned int*)&data_020a4be0 += size >> 1;
}
