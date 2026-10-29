extern void func_ov007_020c6d90(char* self, int i);
extern void func_ov007_020c28ac(void* a);

/* The 0xd0-byte touch picker func_ov007_020c690c builds; names as in that
 * file and func_ov007_020c5a6c, only the slots this function touches. */
typedef struct Ov007Picker {
    char pad_00[8];
    int groupCount; /* 0x08 */
    int maxGroups;  /* 0x0c: a0; func_ov007_020c6550 stops committing at it */
    char pad_10[4];
    /* 0x14: points over the stroke and every group; func_ov007_020c65bc
     * counts it up per appended point and stops at maxPoints (+0x10),
     * func_ov007_020c6784 seeds it */
    int totalPoints;
    char pad_18[8];
    void *path; /* 0x20: the stroke being drawn (func_ov007_020c2c68) */
} Ov007Picker;

typedef char Ov007Picker_size_must_be_0x24[sizeof(Ov007Picker) == 0x24 ? 1 : -1];

/* Resets the picker: when flag is set frees every group slot
 * (func_ov007_020c6d90), then clears the path and both counts. The
 * parameter keeps the callers' char * spelling; `picker` is the typed view. */
void func_ov007_020c6724(char* self, int flag) {
    Ov007Picker *picker = (Ov007Picker *)self;
    if (flag != 0) {
        int i;
        for (i = 0; i < picker->maxGroups; i++) {
            func_ov007_020c6d90(self, i);
        }
    }
    func_ov007_020c28ac(picker->path);
    picker->groupCount = 0;
    picker->totalPoints = 0;
}
