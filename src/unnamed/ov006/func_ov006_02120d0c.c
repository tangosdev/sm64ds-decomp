/* func_ov006_02120d0c at 0x02120d0c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov006).
 */
extern void func_ov006_02120b30(int, int, int);
extern int data_ov006_02142f70;
extern int data_ov006_02142f68;

void func_ov006_02120d0c(int arg0, int arg1) {
  int i;
  for (i = data_ov006_02142f70 - 1; i >= 0; i--) {
    if (*(short *)(data_ov006_02142f68 + i * 0x24 + 0x20) == 0) {
      func_ov006_02120b30(data_ov006_02142f68 + i * 0x24, arg0, arg1);
      return;
    }
  }
}
