extern int func_ov081_02127708(char* c);
extern void _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(char* c, char* clsn);
extern int _ZNK12WithMeshClsn14GetResultFlag1Ev(char* clsn);
extern int _ZNK12WithMeshClsn12TouchesWaterEv(char* clsn);
extern void func_ov081_02126700(char* c);
extern void func_ov081_02126a20(char* c);

int _ZN8Moneybag8BehaviorEv(char* c) {
    func_ov081_02127708(c);
    _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(c, c + 0x1b0);
    if (_ZNK12WithMeshClsn14GetResultFlag1Ev(c + 0x1e4) != 0) {
        if (_ZNK12WithMeshClsn12TouchesWaterEv(c + 0x1e4) != 0) {
            func_ov081_02126700(c);
        }
    }
    func_ov081_02126a20(c);
    return 1;
}
