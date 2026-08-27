/* func_ov006_02120a64 at 0x02120a64
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov006).
 */

extern int func_ov004_020b2444(int,int,int,int,int,int,int);
void func_ov006_02120a64(int c){
  if(*(short*)(c+0x20)==0) return;
  func_ov004_020b2444(*(int*)(c+4)>>12,*(int*)(c+8)>>12,*(short*)(c+0x1e),-1,-1,0,0);
}
