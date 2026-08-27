/* func_ov007_020b8070 at 0x020b8070
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov007).
 */
extern int* data_ov007_02103478[];
extern int data_ov007_020d7a5c[];
extern void MultiCopy32Bytes(int* src, int* dst, int len);
extern void MultiStore32Bytes(unsigned val, int* dst, int len);

void func_ov007_020b8070(int n){
  volatile int z1, z2, z3, z4;
  if(n < 0x17){
    int b, a;
    a = 0x17 - n;
    b = 0x24 - a;
    if(b >= 0x17) b = 0x16;
    MultiCopy32Bytes(data_ov007_02103478[2], data_ov007_02103478[1] + n*0x100, a*0x400);
    z1 = 0;
    MultiStore32Bytes((unsigned)z1, data_ov007_02103478[1], n*0x400);
    MultiCopy32Bytes(data_ov007_02103478[2] + a*0x100, data_ov007_02103478[0] + 0x100, b*0x400);
    z2 = 0;
    MultiStore32Bytes((unsigned)z2, data_ov007_02103478[0] + (b+1)*0x100, (0x17-b)*0x400);
  } else {
    int a = n - 0x16;
    z4 = 0;
    MultiStore32Bytes((unsigned)z4, data_ov007_02103478[1], data_ov007_020d7a5c[1]);
    z3 = 0;
    MultiStore32Bytes((unsigned)z3, data_ov007_02103478[0], a*0x400);
    MultiCopy32Bytes(data_ov007_02103478[2], data_ov007_02103478[0] + a*0x100, (0x17-a)*0x400);
  }
}
