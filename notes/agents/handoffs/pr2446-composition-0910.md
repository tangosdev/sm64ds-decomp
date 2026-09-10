# PR #2446 composition checkpoint

This checkpoint composes independently accepted Bombking source
`f881d8406706fc76cea95a8fe04853d834a009c2` after the reviewed Snowman
composition `3b71825d7da2b969c4d962fd82b189090743757f`. The latter's GitHub PR
is still awaiting terminal private validation at this checkpoint. This is a
local staging order, not a claim that either commit has landed on main.

The accepted Bombking source, header and manifest are unchanged. In particular,
the two state handlers retain calls to the local ov078 helper
`func_ov078_02123804`, rather than the unrelated ov080 destructor at the same
numeric address. The prior independent review covered all 52 functions and all
427 emitted reference identities, including this module distinction.

The ledger conflict comprised 39 preceding records and nine ov078 records.
All are preserved; the complete resulting record multiset matches the two
parents and their merge base. Attribution was compared recursively, and both
matching-history JSONL files retain the exact combined records and historical
source paths. The converted baseline's merged identity set passed its ratchet
before regeneration; the generator then banked 14 existing gains and lost none.

The promotion queue was regenerated from fresh RTTI, vtable and TU-map inputs,
after identifying its retired KingBobOmb row as daBombking_c. This preserves
main's Door row and derives the Bombking row from the actual tree. The C++ TU
summary was regenerated as well.

This remains a text-only promotion: 52 functions cover 12,064 bytes. Compiler
metadata remains outside production data ownership. BKG-01 through BKG-05 and
BKG-07 are fixed; BKG-06 remains explicit unfinished reconstruction under #2479,
including address-named handlers, raw field accesses and manual factory/ABI
code. No complete-class reconstruction or recovered original helper names are
claimed.

Before publication, integrate the actual live main after #2445 lands, record
the final exact candidate and target base, and obtain independent source
acceptance plus the applicable byte, metadata, consumer, credit, port and static
checks. The existing PR is retained to preserve its author's attribution.

The staged review also found BKG-08: the current enemy-provenance row still
called the +0x420 field `void *mState` and omitted the dispatcher's null-entry
return. That single row now describes the actual `StateFunction *` member, the
first-entry return flow and discarded second-entry result, and the remaining
signature inference. This is a documentation correction; source is unchanged.
