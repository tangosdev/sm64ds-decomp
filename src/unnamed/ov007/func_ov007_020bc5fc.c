extern void *func_ov007_020c92ac(void);
extern void func_02046d50(void *a, int b);

/* The 0xa0-byte model record func_ov007_020bc6d4 allocates; names as in
 * func_ov007_020bc6d4, only the slots this file touches. */
typedef struct Ov007Model {
    void **model; /* 0x00: the ModelComponents; its first word is the BMD file */
    char pad_04[0x24];
    int numTexSeqAnims; /* 0x28 */
    int *texSeqAnims;   /* 0x2c: BTP_File table (func_02046bbc, dExtAnmTexPat_c) */
    void *texSeqAnim;   /* 0x30 */
} Ov007Model;

typedef char Ov007Model_size_must_be_0x34[sizeof(Ov007Model) == 0x34 ? 1 : -1];

/* Binds the model's texture-sequence animations: n BTP files in arr, a
 * fresh func_ov007_020c92ac player, and each nonzero file prepared against
 * the model's BMD file. `m` is the typed view of o. */
void func_ov007_020bc5fc(char *o, int *arr, int n)
{
    Ov007Model *m = (Ov007Model *)o;
    int i;

    m->numTexSeqAnims = n;
    m->texSeqAnims = arr;
    m->texSeqAnim = func_ov007_020c92ac();
    for (i = 0; i < n; i++) {
        if (m->texSeqAnims[i])
            func_02046d50(*m->model, m->texSeqAnims[i]);
    }
}
