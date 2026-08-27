extern int Player_PlaySoundEffect();
void func_02011b38(void* c, int a1, int a2, int a3, short a5){
  *(int*)c = a1;
  *(short*)((char*)c+4) = a5;
  *(char*)((char*)c+6) = 1;
  *(int*)((char*)c+0xc) = 0;
  *(int*)((char*)c+0x10) = 0;
  Player_PlaySoundEffect((char*)c+8, a2, a3);
}
