extern int func_020126e8(int a);
extern int func_02012468(int a, int b, int c, int d, int e, int f, int g, int h);
void func_ov006_020fb0fc(char *c, int i){
  int n = i*0x1c;
  int *a = (int*)(c + 0x5bb0);
  int *b = (int*)(c + 0x5bc0);
  *(int*)((char*)b + n) = func_02012468(*(int*)((char*)b + n), 2, 0x189, 4, 0, 0, func_020126e8(*(int*)((char*)a + n)), 0);
  *(int*)((char*)a + n) = *(int*)((char*)a + n) + *(int*)(c + 0x5bb8 + n);
  {
    int v = *(int*)((char*)a + n) >> 12;
    if (v >= 0x110) goto clear;
    if (v > -0x10) return;
  }
clear:
  *(unsigned char*)(c + 0x5bc6 + n) = 0;
  *(unsigned char*)(c + 0x5bc7 + n) = 0;
  *(unsigned char*)(c + 0x5c34) = 0;
}
