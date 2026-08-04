extern unsigned char data_ov006_0212e324[];
extern unsigned char data_ov006_0212e31c[];
void func_ov006_020dc414(char* base, int idx){
  unsigned short* cnt = (unsigned short*)(base + 0x519e + idx*0x14);
  unsigned char* st = (unsigned char*)(base + 0x51a4 + idx*0x14);
  *cnt = *cnt + 1;
  if (*cnt < data_ov006_0212e324[*st]) return;
  *cnt = 0;
  *st = *st + 1;
  if (*st >= 8) {
    *st = 1;
  } else {
    *(unsigned char*)(base + idx*0x14 + 0x5000 + 0x1a3) = data_ov006_0212e31c[*st];
  }
}
