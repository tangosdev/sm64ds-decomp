//cpp
// @symbol _ZN5Model23AddToCommonModelDataArrER8BMD_File
#include "Model.h"
extern "C" {
extern int data_0209cef8[];
extern int data_0209cefc[];
extern int data_0208e738[];
}

/* 0xc-byte records in the common model data array at data_0209cefc,
   counted by data_0209cef8. */
struct CommonModelData {
    BMD_File *file;
    int unk_04;
    int unk_08;
};

void *Model::AddToCommonModelDataArr(BMD_File &file)
{
    CommonModelData *p = (CommonModelData *)data_0209cefc;
    int i = 0;
    int n = data_0209cef8[0];
    while (i < n) {
        if (&file == p->file) return p;
        i++;
        p++;
    }
    p->file = &file;
    p->unk_04 = 0;
    p->unk_08 = 0;
    data_0209cef8[0] = data_0209cef8[0] + 1;
    if (data_0208e738[0] == 0) return p;
    LoadTexAndPal(file);
    return p;
}
