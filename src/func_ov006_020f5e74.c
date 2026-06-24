extern unsigned short data_ov006_0213d344[];
void func_ov006_020f5e74(char* base, int idx){
  unsigned short* cnt = (unsigned short*)(base + 0x51b6 + idx*0x18);
  unsigned char* st = (unsigned char*)(base + 0x51bd + idx*0x18);
  *cnt = *cnt + 1;
  if (*cnt < (data_ov006_0213d344[*st] & 0xff)) return;
  *cnt = 0;
  *st = *st + 1;
  if (*st > 4) {
    *st = 4;
    *(unsigned char*)(base + idx*0x18 + 0x5000 + 0x1bc) = 4;
  }
}
