struct SharedFilePtr { unsigned short fileID; unsigned char numRefs; void *filePtr; };

extern unsigned int data_0209d3bc;
extern int _ZN13SharedFilePtr4LoadEv(struct SharedFilePtr *self);

void *_ZN13SharedFilePtr8LoadFileEv(struct SharedFilePtr *self)
{
    data_0209d3bc = self->fileID;

    if (self->numRefs == 0) {
        if (!_ZN13SharedFilePtr4LoadEv(self))
            return 0;
    }

    if (self->numRefs >= 0xff) {
        return 0;
    }

    *(unsigned char *)(((long long)(int)((char *)self + 2)) & 0xFFFFFFFFFFFFFFFFLL) += 1;
    return self->filePtr;
}
