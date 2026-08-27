extern int data_ov006_0213ca04[];
extern int data_ov006_0213c9d4[];
int func_ov006_020ebb40(char *o){
  int r2 = 0;
  if (*(unsigned char*)(o + 0x93) != 0){
    int r1 = *(short*)(o + 0x8e);
    if (r1 > 0x20){ r2 = 1; goto end; }
    if (r1 > 0x10){ *(unsigned char*)(o + 0x95) = 3; r2 = 5; goto end; }
    if (r1 > 0) r2 = 4;
    goto end;
  }
  {
    int *ref = data_ov006_0213ca04;
    int ip = *(int*)o;
    if (ip == ref[0]){
      if (*(int*)(o + 4) == ref[1]) goto C;
      if (ip == 0) goto C;
    }
    ref = data_ov006_0213c9d4;
    if (ip != ref[0]) goto end0;
    if (*(int*)(o + 4) == ref[1]) goto C;
    if (ip != 0) goto end0;
  }
C:
  {
    int v = *(short*)(o + 0x90);
    if (v < 0x10){ r2 = 5; goto end; }
    if (v < 0x20) r2 = 4;
    else r2 = 0;
    goto end;
  }
end0:
  r2 = 0;
end:
  return r2;
}
