extern void _ZN5Model8LoadFileER13SharedFilePtr(void *);
extern void LoadBlueCoinModel(void *);
extern int G0[];
extern int G1[];
int func_ov020_02112768(char *c)
{
    *(short *)(c + 0xd4) = 0;
    _ZN5Model8LoadFileER13SharedFilePtr(G0);
    _ZN5Model8LoadFileER13SharedFilePtr(G1);
    LoadBlueCoinModel(c);
    return 1;
}
