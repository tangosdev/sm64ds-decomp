#ifndef SM64DS_LEVEL_RNG_H
#define SM64DS_LEVEL_RNG_H

/* A random state: one s32 that RandomIntInternal(int *) steps, so callers
   pass &rng.value. The constructor seeds it to 1 and is defined inline in
   each file that defines an object. The destructor is the empty
   func_0201973c. Both are only declared here, because an inline body in a
   shared header renumbers every consumer's compiler temporaries. */
struct LevelRng {
    int value;
    LevelRng();
    ~LevelRng();
};

/* The actors' state, defined in func_0201fec8.cpp. */
extern LevelRng data_0209e650;
/* The minigames' state, defined in Timer.cpp. */
extern LevelRng data_0209d4b8;

#endif
