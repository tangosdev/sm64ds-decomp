extern int data_ov006_02133810[];
extern int data_ov006_02141774[];
void __sinit_ov006_021300b0(void)
{
    int *s = data_ov006_02133810;
    int *d = data_ov006_02141774;
    d[0]  = s[0];
    d[1]  = s[1];
    d[2]  = s[2];
    d[3]  = s[5];
    d[4]  = s[6];
    d[5]  = s[0xd];
    d[6]  = s[0xe];
    d[7]  = s[0xb];
    d[8]  = s[0xc];
    d[9]  = s[7];
    d[10] = s[8];
    d[11] = s[9];
    d[12] = s[0xa];
    d[13] = s[3];
    d[14] = s[4];
}
