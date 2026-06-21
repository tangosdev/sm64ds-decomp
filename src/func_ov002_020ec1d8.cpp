//cpp
struct CylinderClsn;
struct Node;
extern "C" {

void _ZN12CylinderClsn5ClearEv(CylinderClsn *self);
void _ZN12CylinderClsn6UpdateEv(CylinderClsn *self);
extern Node *data_ov002_02110a48[5];

unsigned int func_ov002_020ec1d8(void)
{
    Node **a = data_ov002_02110a48;
    int i;
    for (i = 0; i < 5; i++) {
        char *p = (char *)*a;
        while (p) {
            _ZN12CylinderClsn5ClearEv((CylinderClsn *)(p + 0xc));
            _ZN12CylinderClsn6UpdateEv((CylinderClsn *)(p + 0xc));
            p = *(char **)(p + 0x48);
        }
        a++;
    }
    return 1;
}

}
