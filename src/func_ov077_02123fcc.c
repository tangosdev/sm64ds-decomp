extern int _ZN9Animation7AdvanceEv();
extern int _ZN5Actor9UpdatePosEP12CylinderClsn();
extern int WithMeshClsn_UpdateDiscreteNoLava_veneer();
extern int _ZNK12WithMeshClsn13JustHitGroundEv();
extern int DecIfAbove0_Byte();
extern void func_ov077_02123a1c(char* c);
int func_ov077_02123fcc(char* c){
    *(short*)((char*)c+0x8c)=*(short*)((char*)c+0x8c)-0x1000;
    _ZN9Animation7AdvanceEv((char*)c+0x124);
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, (char*)c+0x1c4);
    WithMeshClsn_UpdateDiscreteNoLava_veneer((char*)c+0x204);
    if(!_ZNK12WithMeshClsn13JustHitGroundEv((char*)c+0x204)){
        if(DecIfAbove0_Byte((char*)c+0x41c)) goto end;
    }
    func_ov077_02123a1c(c);
end:
    return 1;
}
