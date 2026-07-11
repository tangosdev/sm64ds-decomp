/* func_ov098_0213b520 at 0x0213b520 (ov098), size 0x64
 * Compiler mwccarm 1.2/sp2p3 */
extern int func_0201267c(int, void*);
void func_ov098_0213b520(char* c){
  *(int*)(c+0x3b8) = 0x600;
  *(int*)(c+0x3bc) = *(int*)(c+0x84);
  (*(unsigned char*)(int)(((long long)(int)(c + 0x3b6)) & 0xFFFFFFFFFFFFFFFFLL))++;
  *(short*)(c+0x3b4) = 0;
  *(int*)(c+0xa8) = 0;
  *(int*)(c+0x98) = 0;
  *(int*)(c+0x3c0) = 0;
  func_0201267c(0xd9, c+0x74);
}
