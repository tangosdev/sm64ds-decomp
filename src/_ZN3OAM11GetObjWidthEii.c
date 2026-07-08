extern unsigned char _ZN3OAM8objWidthE[];
unsigned char _ZN3OAM11GetObjWidthEii(int a, int b){
  return *(unsigned char*)&((int*)((char*)_ZN3OAM8objWidthE + b))[a];
}
