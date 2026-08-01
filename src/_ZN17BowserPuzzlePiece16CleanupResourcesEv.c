extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov002_0210da10[];
extern int data_ov002_0210d9a8[];
int _ZN17BowserPuzzlePiece16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210da10);
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9a8);
    return 1;
}
