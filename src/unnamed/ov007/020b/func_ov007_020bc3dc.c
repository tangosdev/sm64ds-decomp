/* func_ov007_020bc3dc at 0x020bc3dc (ov007), size 0x58
 * Compiler mwccarm 1.2/sp2p3, flags:
 * -O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc */
extern int func_ov007_020c9214();
enum Flag { F0, F1 };
void func_ov007_020bc3dc(void* c, int i, int a2){
  unsigned short** arr = *(unsigned short***)((char*)c+0x14);
  unsigned short* p = arr[i];
  enum Flag flag;
  if(p==0) return;
  flag = (*(unsigned char*)((char*)p+4) == 0) ? F0 : F1;
  func_ov007_020c9214(*(void**)((char*)c+0x18), i, flag, *(unsigned short*)((char*)p+2), a2, 0x1000);
}
