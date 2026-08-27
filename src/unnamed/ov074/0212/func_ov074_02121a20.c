typedef struct { int field0; int field4; } Inner;
typedef struct { Inner *ptr; int a; int b; } Entry;
extern Entry data_ov074_02122f34[];

int func_ov074_02121a20(void *c, int idx)
{
    Inner *inner = data_ov074_02122f34[idx].ptr;
    return *(int *)((char *)c + 0x270) == inner->field4;
}
