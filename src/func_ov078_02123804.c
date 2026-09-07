extern int _ZN8dActor_c10FindWithIDEj(unsigned int id);
extern int KingBobOmb_SetState(void *c, void *p);
extern int data_ov078_02126ffc[];
int func_ov078_02123804(char *c){
    unsigned int v=*(unsigned int*)(c+0x3a0);
    if(v==0) return 0;
    if(_ZN8dActor_c10FindWithIDEj(v)==0) return 0;
    if((*(int*)(c+0x39c) & 0x4000)==0) return 0;
    KingBobOmb_SetState(c, data_ov078_02126ffc);
    return 1;
}
