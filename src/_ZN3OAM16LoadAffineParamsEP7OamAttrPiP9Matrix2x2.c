struct OamAttr { unsigned short a, b, c, param; };

#pragma opt_strength_reduction off
#pragma opt_common_subs off
int _ZN3OAM16LoadAffineParamsEP7OamAttrPiP9Matrix2x2(struct OamAttr *oam, int *count, int *m)
{
    unsigned int k0 = (unsigned short)((unsigned int)m[0] >> 4);
    unsigned int k1 = (unsigned short)((unsigned int)m[1] >> 4);
    unsigned int k2 = (unsigned short)((unsigned int)m[2] >> 4);
    unsigned int k3 = (unsigned short)((unsigned int)m[3] >> 4);
    int n = *count;
    int i;

    for (i = 0; i < n; i += 4) {
        struct OamAttr *e = &oam[i];
        if (k0 == e[0].param && k1 == e[1].param &&
            k2 == e[2].param && k3 == e[3].param)
            return i >> 2;
    }
    if (n >= 0x80)
        return -1;
    {
        struct OamAttr *e = &oam[n];
        e[0].param = m[0] >> 4;
        e[1].param = m[1] >> 4;
        e[2].param = m[2] >> 4;
        e[3].param = m[3] >> 4;
    }
    {
        int t = *count;
        *count = t + 4;
        return t >> 2;
    }
}
