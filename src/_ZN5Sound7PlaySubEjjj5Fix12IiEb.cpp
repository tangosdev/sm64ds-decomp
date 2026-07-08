//cpp
extern "C" void func_02011dcc(unsigned int* g, unsigned int j);
extern int data_0208e430;
extern int MESSAGE_SOUND_VOLUME_LSL_12;
extern signed char data_0208e42c;
extern int MUSIC_VOLUME_LSL_12;
extern unsigned int data_0209b4b0[];
extern signed char data_0209b470;
extern int data_0209b494;

extern "C" bool _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int a, unsigned int b, unsigned int c, int d, int e){
  int x;
  if(c==0){
    if(e) goto end;
    x = data_0208e430;
    if(x >= 0 && x != (int)a) goto ret1;
    if(MESSAGE_SOUND_VOLUME_LSL_12 != 0) goto end;
    if(data_0208e42c != (int)b) goto end;
    if(MUSIC_VOLUME_LSL_12 != (int)(b<<12)) goto end;
ret1:
    return true;
  }
  x = data_0208e430;
  if(x >= 0){
    if(!e) goto ld4;
  }
  if(x == (int)a) goto end;
  data_0208e430 = a;
  if(a != 0x35 && a != 0x36){
    if(a != 0x2e) MESSAGE_SOUND_VOLUME_LSL_12 = c & 0xff;
  }
  func_02011dcc(data_0209b4b0, a);
  goto end;
ld4:
  if(x != (int)a) return false;
end:
  data_0208e42c = (signed char)b;
  data_0209b470 = (signed char)c;
  data_0209b494 = d;
  if(MUSIC_VOLUME_LSL_12 != (int)(b<<12)) goto ret0;
  if(MESSAGE_SOUND_VOLUME_LSL_12 == (int)(c<<12)) return true;
ret0:
  return false;
}
