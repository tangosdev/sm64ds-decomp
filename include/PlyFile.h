#ifndef PLYFILE_H
#define PLYFILE_H

#include "SharedFilePtr.h"

/* The player's file handles, one class per kind of file. ov002 defines them
 * all (src/game/player/d_a_ply_methods.cpp) and its static initializer
 * constructs each with its file ID; d_a_luigi.cpp shares five of them. The
 * constructors and destructors are the shared-file helpers each kind
 * registers, so ov002's manifest aliases them onto those arm9 symbols.
 * Parameters are unnamed: a named one renumbers @NNN in every includer. */
struct PlyModelFile : SharedFilePtr {
    PlyModelFile(unsigned int);
    ~PlyModelFile();
};

struct PlyAnimFile : SharedFilePtr {
    PlyAnimFile(unsigned int);
    ~PlyAnimFile();
};

struct PlyTexSeqFile : SharedFilePtr {
    PlyTexSeqFile(unsigned int);
    ~PlyTexSeqFile();
};

#endif
