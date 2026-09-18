# Synthesized ctor's vague-linkage Matrix4x3 D1 (TRAP-2709-01, PR #2709)

- tested_commit: 395312cb4 (cleanup-datrstrap at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/daTrsTrap_c/d_a_trs_trap_classinits.cpp:16-18` -- the
  synthesized ctor from `return new daTrsTrap_c()` emits a vague-linkage
  _ZN9Matrix4x3D1Ev over mClsnMat, licensed as deadstrip in this TU's manifest
  entry. No compile experiment applies (packaging disposition, not codegen),
  so this entry records the license row.
- disposition: compiler_constraint (packaging form). No source-side alternative
  exists to measure -- the symbol is emitted while lowering a synthesized ctor --
  so the durable artifact is the committed `compiler_only_output` license row
  quoted below rather than a compile log. The reviewer decides whether that
  substitution is acceptable or the entry must be re-labelled `deferred`.
- attempted_change: none (packaging check only).

```diff
 (no change -- manifest row quoted below)
```

- command:

```text
.venv/bin/python -c "import json; d=json.load(open('config/tu_manifest.d/ov063/daTrsTrap_c_classInit.json')); print(json.dumps(d['compiler_only_output'], indent=1))"
```

- result: the license row exists in
  `config/tu_manifest.d/ov063/daTrsTrap_c_classInit.json` under
  `compiler_only_output`.
- log (quoted row):

```text
[
 {
  "disposition": "deadstrip",
  "reason": "mwccarm emits Matrix4x3's vague-linkage D1 while lowering the synthesized ctor from `return new daTrsTrap_c()` over the Matrix4x3 member at 0x324 (same shape as ov070/daKpFr_c); no configured ROM symbol of that name exists in any module and no surviving reference reaches it",
  "symbol": "_ZN9Matrix4x3D1Ev"
 }
]
```

- verdict: PACKAGING-LICENSED
