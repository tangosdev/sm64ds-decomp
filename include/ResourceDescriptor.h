#ifndef RESOURCE_DESCRIPTOR_H
#define RESOURCE_DESCRIPTOR_H

struct SharedFilePtr;
struct CLPS_Block;

/* The model/collision/CLPS triple a platform actor's InitResourcesFrom and
 * CleanupResourcesFrom take (daObjFallBlock_c, daObjGuragura_c,
 * daObjKaitendai_c, daObjSwdoor_c and the maruta pair). Each overlay keeps its
 * own copy in .data: the two words relocate to that overlay's model and
 * collision SharedFilePtrs and the third to its CLPS block. */
struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};

#ifndef SM64DS_PLATFORM_PC
typedef char ResourceDescriptor_size_must_be_0x0c[sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];
#endif

#endif
