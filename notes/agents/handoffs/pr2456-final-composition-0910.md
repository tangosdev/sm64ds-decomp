# Yurei current-main composition review, 2026-09-10

This review continues accepted source `62f1b517ad7cbd26ab05329d01a7605f2143bbb8`
from base `f4b2ce22a3021857de2bdb51f350db8f8017a17b`, preserving all eight YUR
findings and the partial reconstruction owned in #2478. The staging base is the
independently accepted Pkn head `05231fa9f21460c5c4a803031eb04118825fc32b`.
The final composition includes actual main
`02abcd39647f5df58169bea137f2d3c048848a52`. Pkn squash
`c2e898f42e91f2e911b637f5c44c72ad3f09512d` has the exact accepted Pkn tree;
the subsequent main commit changes only three progress files. The whole composed
index was checked against reviewed Yurei staging `80c30debc6233a44f6e2a21de7abc2c32ccb36a6`
plus those three main blobs before this handoff update. Final independent review
and terminal private validation remain required.

The class keeps its 23 licensed functions: ten native method symbols, twelve
address-named helpers and one manual factory. The small neighboring
`func_ov065_021177e4` provenance correction remains unchanged from accepted source.

YUR-09 corrects the BSS explanation. `__sinit_ov065_0211c110` initializes four
shared-file handles and copies eight 8-byte PMF constants into four 16-byte State
records at ov065 `0x0211d650..0x0211d690`. The current SharedFilePtr header has no
recovered fields or declared constructor, and State only contains two PMFs.
Declaring storage does not universally imply an additional compiler initializer.
This class TU leaves the BSS storage ROM-supplied and retains the initializer in
its separately enrolled source file, `src/__sinit_ov065_0211c110.c`. Its complete
`.init` entry covers `0x0211c110..0x0211c2a8`. No BSS layout or constructor claim
is added.

YUR-10 distinguishes the historical 22-function pre-fold census from the current
23-function manifest. The srcpath factory-spelling limitation still exists, but
the retired shards are no longer the current source organization. The queue row
now records one source and its actual line count.

The composition retains all 2,716 main converted identities using the seven
Snufit-to-daYurei_Mucho_c path/symbol mappings. Attribution is the exact recursive
three-way merge; attempts (2,073), provenance (721), and rename-ledger (3,620) rows
are exact three-way multisets. No contributor credit or main identity is dropped.

Build reports retain their exact tested commits. Class source changes in this
composition are comments only; complete-object equality and full relocation proof
are recorded separately from broader ROM, consumer, static and merge checks.
