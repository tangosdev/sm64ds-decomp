extern int func_ov006_0210dbb0(int a, void* p);
struct V2 { int x; int y; };
/* Both reads of +0x4780 use POINTER arithmetic on a `char*`, not integer arithmetic
 * cast to a pointer. Under 2004/b56 that choice decides the addressing mode: written
 * as `*(int*)(base + 0x4000 + 0x780)` b56 folds the constant, parks 0x4780 in the
 * literal pool and emits a register-offset `ldr`, while the ROM computes
 * `add r2,r2,#0x4000` and loads `[r2, #0x780]`. `ldr` carries only a 12-bit
 * displacement, so 0x4780 cannot be encoded and the address must be split either way;
 * the pointer-arithmetic spelling is the one that lets b56 split it the ROM's way.
 * Both reads must use it -- respelling only one leaves the other sharing.
 */
int func_ov006_02111e90(char* c, int* src){
  int g = *(int*)((char*)*(int*)(c+4)+0x4780);
  if(g == 0) return 0;
  struct V2 v;
  v.x = src[0];
  v.y = src[1];
  return func_ov006_0210dbb0(*(int*)((char*)*(int*)(c+4)+0x4780), &v);
}
