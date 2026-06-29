/* func_ov002_020dc09c at 0x020dc09c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
 */
struct V3 { int a, b, c; };
extern void func_ov002_020dc174(char* c, struct V3* r1, int r2, int r3, unsigned int a5, unsigned int a6);
void func_ov002_020dc09c(char* c){
  struct V3 v;
  int r2, r3;
  if(*(unsigned char*)(c+0x703) == 0){
    if(*(unsigned short*)(c+0x6a4) & 1){
      r3 = 0x28000;
      v.a = 0;
      v.b = 0x28000;
      v.c = 0x87000;
      r2 = 0x3c000;
    }else{
      r3 = 0x50000;
      v.a = 0;
      v.b = 0x28000;
      v.c = 0x10e000;
      r2 = 0x50000;
    }
  }else{
    r2 = 0x80000;
    if(*(unsigned short*)(c+0x6a4) & 1){
      v.a = 0;
      v.b = 0x80000;
      v.c = 0xc0000;
      r3 = 0x80000;
    }else{
      v.a = 0;
      v.b = 0x80000;
      v.c = 0x1c0000;
      r3 = 0x100000;
    }
  }
  func_ov002_020dc174(c, &v, r2, r3, 0x40000, 0);
}
