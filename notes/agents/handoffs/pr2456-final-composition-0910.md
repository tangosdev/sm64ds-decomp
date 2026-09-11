# Yurei current-main composition review, 2026-09-10

This review continues accepted source `35188d5d3760316b5f9f0a19575dc63badeafcb5`
from base `a0cc78fbd18f166ba67fca7c38b63bca348c8edd`, preserving all eight YUR
findings and the partial reconstruction owned in #2478. The staging base is the
independently accepted Pkn head `180b064aaa8bafaac570224a58206d922d42212c`.
Final acceptance requires the actual merged main base and terminal private validation.

The class keeps its 23 licensed functions: ten native method symbols, twelve
address-named helpers and one manual factory. The small neighboring
`func_ov065_021177e4` provenance correction remains unchanged from accepted source.

YUR-09 corrects the BSS explanation. `__sinit_ov065_0211c110` initializes four
shared-file handles and copies eight 8-byte PMF constants into four 16-byte State
records at ov065 `0x0211d650..0x0211d690`. The current SharedFilePtr header has no
recovered fields or declared constructor, and State only contains two PMFs.
Declaring storage does not universally imply an additional compiler initializer.
This TU leaves the storage and initializer with the ROM because its production
ownership covers text only; no BSS layout or constructor claim is added.

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
