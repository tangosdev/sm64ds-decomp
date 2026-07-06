/* func_ov062_021161a8 at 0x021161a8 (ov062), size 0x90
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 * flags: -O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */
extern int _ZNK9Animation12WillHitFrameEi(char *anim, int f);
extern int func_ov002_020db54c(int p, int a, int b, int s);
extern int func_02012694(int id, void *pos);
extern int _ZN9Animation8FinishedEv(char *anim);
extern int func_ov062_02116cd8(char *c, void *p);
extern int data_ov062_0211ded0[];

int func_ov062_021161a8(char *c)
{
  if (*(int *)(c + 0x3f8) != 0 && _ZNK9Animation12WillHitFrameEi(c + 0x350, 0x14))
  {
    func_ov002_020db54c(*(int *)(c + 0x3f8), 0x28000, 0x50000, *(short *)(c + 0x8e));
    *(int *)(c + 0x3f8) = 0;
    func_02012694(0x126, c + 0x74);
  }
  if (_ZN9Animation8FinishedEv(c + 0x350))
  {
    int *q = (int *)(((long long)(int)(c + 0x128)) & 0xFFFFFFFFFFFFFFFFLL);
    *q &= ~2;
    func_ov062_02116cd8(c, data_ov062_0211ded0);
  }
  return 1;
}
