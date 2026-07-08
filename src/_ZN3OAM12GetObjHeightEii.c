extern unsigned char _ZN3OAM9objHeightE[];
unsigned char _ZN3OAM12GetObjHeightEii(int a, int b){
  return *(unsigned char*)&((int*)((char*)_ZN3OAM9objHeightE + b))[a];
}
