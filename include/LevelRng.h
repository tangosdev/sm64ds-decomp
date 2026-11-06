#ifndef SM64DS_LEVEL_RNG_H
#define SM64DS_LEVEL_RNG_H

/* The actor random state: one s32 that RandomIntInternal(int *) steps, so
   callers pass &data_0209e650.value. func_0201fec8.cpp defines the object
   and the constructor, which seeds it to 1. The destructor is the empty
   func_0201973c. Both are only declared here, because an inline body in a
   shared header renumbers every consumer's compiler temporaries. */
struct LevelRng {
    int value;
    LevelRng();
    ~LevelRng();
};

extern LevelRng data_0209e650;

#endif
