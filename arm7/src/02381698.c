/* storage 0x02381698 runtime 0x037F9530 size 0x20 wram ARM. No pool. */

typedef struct {
    unsigned short unk_00;
    unsigned short unk_02;
    unsigned int unk_04;
    unsigned int unk_08;
    unsigned int unk_0C;
    unsigned int unk_10;
} P1_02381698;

P1_02381698 *fn_02381698(P1_02381698 *result, int a2, int a3) {
    result->unk_00 = 0;
    result->unk_02 = 0;
    result->unk_04 = a2;
    result->unk_08 = a3;
    result->unk_0C = 0;
    result->unk_10 = 0;
    return result;
}
