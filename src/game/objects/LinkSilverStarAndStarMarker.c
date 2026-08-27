void LinkSilverStarAndStarMarker(char* a, char* b){
  if (b != 0) {
    *(int*)(a + 0x1cc) = *(int*)(b + 4);
    *(unsigned char*)(a + 0x1da) = *(int*)(b + 0x440);
    short s = *(short*)(b + 0xce);
    if (s >= 0) *(short*)(a + 0x1d6) = s;
  } else {
    *(short*)(a + 0x1d6) = -1;
    *(int*)(a + 0x1cc) = 0;
  }
}
