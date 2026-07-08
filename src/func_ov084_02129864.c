extern int _ZN5Actor9TrackStarEjj(void*,unsigned int,unsigned int);
extern int NUM_VS_STARS_COLLECTED[];
extern int VS_STAR_SPAWN_ORDER[];
void func_ov084_02129864(char *c){
    if(*(unsigned char*)(c+0x464)!=2) return;
    if(*(signed char*)(c+0x465)>=0) return;
    unsigned int b3=*(unsigned char*)NUM_VS_STARS_COLLECTED;
    char *p=*(char**)VS_STAR_SPAWN_ORDER;
    unsigned int m=*(unsigned char*)(p + b3);
    unsigned int v=*(unsigned char*)(c+0x466);
    if(v!=m) return;
    *(unsigned char*)(c+0x465)=(unsigned char)_ZN5Actor9TrackStarEjj(c,v,1);
}
