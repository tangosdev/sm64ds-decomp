struct SharedFilePtr { unsigned short fileID; unsigned char numRefs; void *filePtr; };

extern unsigned int LAST_USED_FILE_ID;
extern void func_02017c24(struct SharedFilePtr *self);

void _ZN13SharedFilePtr7ReleaseEv(struct SharedFilePtr *self)
{
    LAST_USED_FILE_ID = self->fileID;

    if (self->numRefs == 0)
        return;

    *(unsigned char *)(((long long)(int)((char *)self + 2)) & 0xFFFFFFFFFFFFFFFFLL) -= 1;

    if (self->numRefs != 0)
        return;

    func_02017c24(self);
}
