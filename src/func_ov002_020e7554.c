typedef unsigned char u8;
typedef unsigned int u32;

extern char* _ZN5Actor15FindWithActorIDEjPS_(u32 actorID, char* prev);
extern char* _ZN5Actor10FindWithIDEj(u32 id);
extern int RandomIntInternal(int* seed);
extern void LinkSilverStarAndStarMarker(char* a, char* b);
extern int data_0209e650;

void func_ov002_020e7554(char* c)
{
    char* found;
    char* arr[5] = {0};
    int cnt;
    unsigned int idx;

    found = 0;
    cnt = 0;
    do {
        found = _ZN5Actor15FindWithActorIDEjPS_(0xb4, found);
        if (found == 0) break;
        if ((*(u8*)(found + 0x1d8) == 3 && _ZN5Actor10FindWithIDEj(*(u32*)(found + 0x1cc)) == 0) ||
            (*(u8*)(found + 0x1d8) != 0 && (unsigned int)(*(u8*)(found + 0x1db) << 0x1f) >> 0x1f &&
             _ZN5Actor10FindWithIDEj(*(u32*)(found + 0x1cc)) == 0)) {
            arr[cnt] = found;
            cnt++;
        }
    } while (cnt < 5);

    idx = ((unsigned int)RandomIntInternal(&data_0209e650) >> 16) % (unsigned int)cnt;
    found = arr[idx];
    if (found == 0) return;
    *(u32*)(c + 0x434) = *(u32*)(found + 4);
    LinkSilverStarAndStarMarker(found, c);
}
