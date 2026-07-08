//cpp
extern "C" {
extern void Player_PlaySoundEffect(int x, unsigned int a, unsigned int b);
extern int data_0209b4a4[];
void _ZN5Sound6Play2DEjj(unsigned int j1, unsigned int j2){
  Player_PlaySoundEffect((int)data_0209b4a4, j1, j2);
}
}
