int func_ov006_020ebeb4(char *c){
    int r=0;
    int v=*(int*)(c+0x20);
    if(v>=0xe8000){ r|=1; v=0xe8000; *(int*)(c+0x20)=v; r&=0xff; }
    else if(v<=0x18000){ r|=2; v=0x18000; *(int*)(c+0x20)=v; r&=0xff; }
    v=*(int*)(c+0x24);
    if(v>=0xa8000){ r|=8; v=0xa8000; *(int*)(c+0x24)=v; r&=0xff; }
    else if(v<=0x18000){ r|=4; v=0x18000; *(int*)(c+0x24)=v; r&=0xff; }
    return r;
}
