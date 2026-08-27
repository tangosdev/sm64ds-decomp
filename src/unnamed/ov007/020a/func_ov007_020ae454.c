extern int _ZN4cstd3divEii(int,int);
extern short data_02082214[];
void func_ov007_020ae454(char* c){
  int* s = *(int**)c;
  if(*(unsigned short*)s != 1) return;
  if(*(short*)(*(int*)((char*)s+4)) != 0) return;
  int a = *(int*)((char*)s+0x18);
  if(a>0){
    int b = *(int*)((char*)s+0x1c);
    int d = b - a;
    int r4;
    if(d<=0) r4=0;
    else if(d>=b) r4=0x1000;
    else r4=_ZN4cstd3divEii(d<<0xc, b);
    int t = ((r4 * (*(int*)((char*)s+0x1c)*0x2d)) >> 0xc) << 0x10;
    int r = _ZN4cstd3divEii(t, 0x168);
    r = (unsigned short)r;
    int sv = data_02082214[(r>>4)<<1];
    int inv = 0x1000 - r4;
    int prod = (int)(((long long)sv*inv + 0x800) >> 0xc);
    int e = *(short*)(*(int*)((char*)s+0x24) + 4);
    *(int*)((char*)s+0xc) = (e<<0xc) + (prod<<2);
  } else {
    int e = *(short*)(*(int*)((char*)s+0x24) + 4);
    *(int*)((char*)s+0xc) = e<<0xc;
  }
}
