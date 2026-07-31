/* func_ov004_020b1a5c at 0x020b1a5c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov004).
 */
typedef int s32;
s32 GetGameLanguage(void);
int RenderOamMainScreen(int a, int b, int c, int d, int e);
int func_ov004_020b1ea4(int a, int b, int c, int d, int e, int f, int g);
extern int data_ov004_020b944c[];
extern char* data_ov004_020bbfa8[];
void func_ov004_020b1a5c(int a0, int a1){
  int lim = data_ov004_020b944c[a1];
  int c = a0;
  int r4 = 0xf4;
  int i;
  if (c >= lim) c = lim;
  func_ov004_020b1ea4(0xf4, 0xc, c, 1, -1, 1, 0xa);
  r4 = r4 - (0x44 - (6 - a1) * 8);
  i = GetGameLanguage();
  RenderOamMainScreen(*(int*)(data_ov004_020bbfa8[i]+0x10), r4, 0xc, -1, -1);
}
