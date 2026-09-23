# Menbo provenance corrections after PR #2994

Input: `8f5b7274a11573c4ba50a41b66449da984927665`. PR #2994 was merged externally as `c6c1b71a420a20c27d7f1f845b2619b439157c75` while this repair was being reviewed. The final candidate composes main `1e80857fe45a24c974307ea9ad39a1ba6898c843`; its intervening changes are only README and contributions metadata.

Independent review read all 24 promoted functions, their predecessor bodies, the unchanged class header, relevant callees, relocation rows, enrollment and baseline migrations. It found inaccurate new explanatory text without an introduced runtime regression.

- MENBO-RTTI-01: corrected the RTTI first word to the ABI runtime type-info vtable at arm9 `0x0209a764`; the third word still identifies the enemy base.
- MENBO-RTTI-PADDING-01: corrected the manifest to ten encoded-name characters followed by a terminator and one padding byte, matching the twelve ROM bytes.
- MENBO-VTABLE-01: corrected four in-run slots to ten, with the exact indices in source and manifest.
- MENBO-MATRIX-01: named matrix declaration parameters in the actual input/input/output order and corrected the description of the scratch destination. Types, argument order and executable statements are unchanged.
- MENBO-RAYCAST-01: distinguished the pre-raycast level/mode return from the surface-flag return after collision detection.
- MENBO-STATE-01: replaced unsupported state-entry names with the evidenced updater/animation pairings.

The converted-ratchet D0 false positive is a separate classifier repair. No baseline is lowered here. This task corrects provenance; raw-offset accesses, anonymous helpers and mangled ABI bridges remain inherited reconstruction work, not newly solved methods or layouts.

Producer and independent verifier record exact object/link/static and full-ROM evidence in the task. Publication is not merge acceptance, and no main merge is requested by this handoff.
