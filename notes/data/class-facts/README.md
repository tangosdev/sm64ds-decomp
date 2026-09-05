# Class facts

One JSON file per class, written by the `scout` stage of the pipeline described
in `../../agents/PIPELINE.md` and consumed by the `writer` stage.

A facts file records what the **ROM** says about a class — the RTTI name and its
address, the vtable address and every slot, which slots are this class's own
overrides versus inherited, where the `_ZTI`/`_ZTS` pair actually lives (vague
linkage puts it in whichever module the linker reached first, not necessarily
the one holding the `_ZTV`), the object size, and the field offsets that could
be proved from load/store displacements.

The `unproven` array is the point of the file. It is the writer's binding list
of things they may not name. A plausible invented name is worse than `unk_38e`:
it reads as established fact and blocks later verification.

Files land here on the class's own `cpp/<Class>-tu` branch, since scout and
writer are one claim.
