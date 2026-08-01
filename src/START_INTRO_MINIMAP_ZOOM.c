extern void func_ov006_02110e28(char* c);

void START_INTRO_MINIMAP_ZOOM(char* c){
  *(int*)(c+0x10) = *(int*)(c+8);
  *(int*)(c+0x14) = *(int*)(c+0xc);
  if (*(unsigned char*)(*(char**)(c+4) + 0x595d) != 0)
    return;
  if (*(short*)(c+0x36) < *(short*)(c+0x34)) {
    short* p = (short*)(((int)c + 0x36));
    *p = *p + 8;
    if (*(short*)(c+0x36) > *(short*)(c+0x34))
      *(short*)(c+0x36) = *(short*)(c+0x34);
  } else {
    short* p = (short*)(((int)c + 0x36));
    *p = *p - 8;
    if (*(short*)(c+0x36) < *(short*)(c+0x34))
      *(short*)(c+0x36) = *(short*)(c+0x34);
  }
  {
    short* q = (short*)(((int)c + 0x32));
    *q = *q + *(short*)(c+0x36);
  }
  func_ov006_02110e28(c);
}
