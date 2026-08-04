//cpp
// func_ov006_02106eb8 at 0x02106eb8
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov006).
struct C; typedef void (C::*PMF)(int);
extern PMF data_ov006_02142840[];
extern "C" {
extern void func_ov006_021050bc(void* c);
extern void func_ov006_021057f0(void* c);
}
extern "C" void func_ov006_02106eb8(char* c) {
  int i;
  func_ov006_021050bc(c);
  func_ov006_021057f0(c);
  for (i = 0; i < *(int*)(c + 0x4cb8); i++) {
    unsigned char idx = *(unsigned char*)(c + i + 0x4efa);
    (((C*)c)->*data_ov006_02142840[idx])(i);
  }
}
