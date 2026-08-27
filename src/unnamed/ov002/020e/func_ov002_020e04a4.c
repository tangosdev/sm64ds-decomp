/* func_ov002_020e04a4 at 0x020e04a4
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
 */

extern void func_ov002_020bf88c(char* c);
extern unsigned char data_020a0e40[];
extern short data_0209f4a0[];
void func_ov002_020e04a4(char* c) {
  if (*(short*)((char*)data_0209f4a0 + data_020a0e40[0]*0x18) < 0x600) return;
  func_ov002_020bf88c(c);
}
