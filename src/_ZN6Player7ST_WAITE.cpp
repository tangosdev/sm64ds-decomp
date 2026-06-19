//cpp
extern "C" {
void data_ov034_02114738(void);
struct P {
  char p0[0x34];
  struct { int a,b; } A[3];
  struct { int c,d; } B[3];
  int C[3];
  char D[3];
  char e73,e74,e75;
  char pad[2];
  int F78,F7c,F80,F84;
};
void _ZN6Player7ST_WAITE(void* c){
  struct P* r4=(struct P*)c;
  data_ov034_02114738();
  for(int i=0;i<3;i++){
    r4->A[i].a=0;
    r4->A[i].b=0;
    r4->B[i].c=0;
    r4->B[i].d=0;
    r4->C[i]=0;
    r4->D[i]=0;
  }
  r4->e73=0;
  r4->e74=0;
  r4->e75=0;
  r4->F78=0;
  r4->F7c=0;
  r4->F80=0;
  r4->F84=0;
}
}
