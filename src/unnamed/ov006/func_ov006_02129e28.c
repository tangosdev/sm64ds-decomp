void func_ov006_02129e28(char *c, int i){
  int o = i*0x24;
  char *b9c = c + 0xbe9c;
  char *b94 = c + 0xbe94;
  char *b98 = c + 0xbe98;
  int v;
  *(int*)(b94 + o) = *(int*)(b94 + o) + *(int*)(b9c + o);
  *(int*)(b98 + o) = *(int*)(b98 + o) + *(int*)(c + o + 0xbea0);
  v = *(int*)(b9c + o);
  if(v > 0){
    v -= 0x20;
    *(int*)(b9c + o) = v;
    v = *(int*)(b9c + o);
    v = (short)v;
    if(v < 0){ v = 0; *(int*)(b9c + o) = v; }
  } else if(v < 0){
    v += 0x20;
    *(int*)(b9c + o) = v;
    v = *(int*)(b9c + o);
    if(v > 0){ v = 0; *(int*)(b9c + o) = v; }
  } else {
    *(unsigned char*)(c + o + 0xbeb0) = 0;
  }
}
