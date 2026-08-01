extern void Hud_RenderSprite(int a, int b, int c, int d, int e);
extern int data_ov006_0213abc8[];
void func_ov006_020f0f7c(char *c){
    int i;
    if(*(int*)(c+0x4f78) == 0)
        return;
    for(i=0;i<0x78;i++){
        if(*(unsigned char*)(c+i+0x53dd) == 1){
            Hud_RenderSprite(
                data_ov006_0213abc8[*(unsigned char*)(c+i+0x5365)],
                ((int*)(c+0x47f8))[i] >> 12,
                ((int*)(c+0x49d8))[i] >> 12,
                -1,
                0);
        }
    }
}
