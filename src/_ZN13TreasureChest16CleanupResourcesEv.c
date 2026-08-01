extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov064_0211c96c[];
extern int data_ov064_0211c964[];
extern int data_ov002_0210d9a8[];
extern int data_ov002_0210da38[];
int _ZN13TreasureChest16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov064_0211c96c);
    _ZN13SharedFilePtr7ReleaseEv(data_ov064_0211c964);
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9a8);
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210da38);
    return 1;
}
