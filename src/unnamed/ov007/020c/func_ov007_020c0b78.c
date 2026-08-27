/* func_ov007_020c0b78 at 0x020c0b78
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov007).
 */
struct E8 { int a, b; };

extern int func_ov007_020c0eb0(struct E8*, int, int, int, int, int, int, int);
extern void func_ov007_020c0d70(void*, void*);
extern int func_ov007_020c0b78(struct E8* p, void* s, int x, int y, int a4, unsigned char a5, int a6, int sb);

int func_ov007_020c0b78(struct E8* p, void* s, int x, int y, int a4, unsigned char a5, int a6, int sb){
  char* sp = (char*)s;
  int u, t, sum24, sum28;
  unsigned char b5 = a5;

  sum24 = *(int*)(sp + 0x00) + x;
  sum28 = *(int*)(sp + 0x04) + y;

  if (b5 > 3) b5 = *(unsigned char*)(sp + 0x14);

  if (*(int*)(sp + 0x10) != 0 || a6 != 0) t = 1; else t = 0;

  if (*(int*)(sp + 0x08) != 0 && a4 != 0) u = 1; else u = 0;

  if (u) {
    if (*(unsigned char*)(sp + 0x1c)) {
      void* r8 = ((void**)*(void***)(sp + 0x18))[*(unsigned char*)(sp + 0x1d)];
      if (r8) {
        int i;
        void* r6 = ((void**)*(void***)r8)[*(short*)(sp + 0x2c)];
        for (i = 0; i < *(int*)((char*)r6 + 4); i++) {
          func_ov007_020c0eb0(
            &p[sb],
            *(int*)r6 + i * 6,
            (sum24 >> 12) + *(unsigned short*)((char*)r8 + 8),
            (sum28 >> 12) + *(unsigned short*)((char*)r8 + 0xa),
            b5,
            *(int*)(sp + 0x0c),
            t,
            *(signed char*)(sp + 0x15));
          sb++;
        }
        func_ov007_020c0d70(s, r8);
      }
    }
  }

  if (*(void**)(sp + 0x3c)) sb = func_ov007_020c0b78(p, *(void**)(sp + 0x3c), x, y, a4, a5, a6, sb);
  if (*(void**)(sp + 0x38)) sb = func_ov007_020c0b78(p, *(void**)(sp + 0x38), sum24, sum28, u, b5, t, sb);

  return sb;
}
