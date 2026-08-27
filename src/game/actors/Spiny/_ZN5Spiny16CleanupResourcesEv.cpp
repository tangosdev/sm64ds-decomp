//cpp
// @symbol _ZN5Spiny16CleanupResourcesEv
/* Spiny::CleanupResources -- vtable slot 3. Releases the three shared files the
 * class holds; it never touches `this`. */
#include "Spiny.h"

extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov077_02127b48[];
extern int data_ov077_02127b38[];
extern int data_ov077_02127c14[];
}

int Spiny::CleanupResources()
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov077_02127b48);
    _ZN13SharedFilePtr7ReleaseEv(data_ov077_02127b38);
    _ZN13SharedFilePtr7ReleaseEv(data_ov077_02127c14);
    return 1;
}
