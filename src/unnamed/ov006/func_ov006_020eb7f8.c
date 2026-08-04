extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);

typedef struct { int a, b; } P;
extern P data_ov006_0213c9b4;
extern P data_ov006_0213c9a4;
extern P data_ov006_0213c99c;
extern P data_ov006_0213c9fc;

void func_ov006_020eb7f8(char* c){
  P* d = &data_ov006_0213c9b4;
  int c0 = *(int*)c;
  if(c0 == d->a && (*(int*)(c+4) == d->b || c0 == 0)){
    *(unsigned char*)(c+0x93) = 0;
    *(short*)(c+0x78) = 0x60;
    if(*(int*)(c+0x70) == 0){
      _ZN5Sound12PlayBank2_2DEj(0x1e9);
    }
    *(P*)c = data_ov006_0213c99c;
  }else{
    P* d2 = &data_ov006_0213c9a4;
    if(c0 != d2->a || (*(int*)(c+4) != d2->b && c0 != 0)){
      *(short*)(c+0x90) = 0x31;
      *(P*)(c+0x10) = *(P*)(c+8);
      _ZN5Sound12PlayBank2_2DEj(0x1ed);
    }
    *(int*)(c+0x60) = 0;
    *(P*)c = data_ov006_0213c9fc;
  }
}
