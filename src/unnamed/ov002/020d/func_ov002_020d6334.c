struct Obj { char pad[0x588]; int *arr; /* 0x588 */ };

int func_ov002_020d6334(struct Obj *o, int val)
{
    int i;
    int *arr;
    arr = o->arr;
    for (i = 0; i < 5; i++) {
        if (arr[i] == 0) {
            arr[i] = val;
            return 1;
        }
    }
    return 0;
}
