#!/usr/bin/env python3
"""Build the evidence-only actor/profile source-naming reconstruction pilot.

This tool does not rewrite symbols, source files, or delinks.  It joins the ROM's
391-entry actor table and literal debug-name table to the existing RTTI, vtable,
operator-new, and recovered-TU analyses, then emits a deliberately small pilot.

The output keeps three claims separate:

* Tier A: observations present in the SM64DS ROM or directly derived from it;
* Tier B: source-style spellings supported by later EAD lineage;
* Tier C: current semantic/community aliases.

Usage:
    python tools/profile_reconstruction.py
    python tools/profile_reconstruction.py --refresh-derived
    python tools/profile_reconstruction.py --check

Outputs:
    symbols/profile_reconstruction_pilot.tsv
    symbols/profile_reconstruction_pilot.json
    symbols/profile_reconstruction_renames.tsv
    symbols/profile_reconstruction_registry.tsv
    symbols/profile_reconstruction_registry.json
"""

from __future__ import annotations

import argparse
import collections
import csv
import json
import pathlib
import re
import struct
import subprocess
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
TOOLS = REPO / "tools"
sys.path.insert(0, str(TOOLS))

import actor_names as AN  # noqa: E402
import srcpath as SP  # noqa: E402

TABLE = AN.TABLE
N_ACTORS = AN.N_ACTORS
ARM9_BASE = AN.ARM9_BASE

OUT_TSV = REPO / "symbols" / "profile_reconstruction_pilot.tsv"
OUT_JSON = REPO / "symbols" / "profile_reconstruction_pilot.json"
OUT_RENAMES = REPO / "symbols" / "profile_reconstruction_renames.tsv"
OUT_FULL_TSV = REPO / "symbols" / "profile_reconstruction_registry.tsv"
OUT_FULL_JSON = REPO / "symbols" / "profile_reconstruction_registry.json"

RTTI = REPO / "build" / "rtti.json"
RTTI_VTABLES = REPO / "build" / "rtti_vtables.json"
TU_MAP = REPO / "build" / "tu_map.json"
TU_NAMES = REPO / "build" / "tu_names.json"
OPNEW = REPO / "build" / "opnew_sizes.json"
SINIT_OWNERS = REPO / "build" / "sinit-owners.json"

NSMBW_REV = "2e010f8708d8232c736b1ece507400dfd76aaa9c"
NSMBW_PROFILE_URL = (
    "https://github.com/NSMBW-Community/NSMBW-Decomp/blob/"
    f"{NSMBW_REV}/include/game/framework/f_profile.hpp"
)
NSMBW_BASE_URL = (
    "https://github.com/NSMBW-Community/NSMBW-Decomp/blob/"
    f"{NSMBW_REV}/include/game/framework/f_base.hpp"
)
NSMBW_FILENAME_URL = (
    "https://github.com/NSMBW-Community/NSMBW-Decomp/blob/"
    f"{NSMBW_REV}/source/d_basesNP/bases/d_a_obj_fruit_tree.cpp"
)

# The module preference is part of the sample definition, not a general actor-table
# resolver.  Overlay virtual addresses overlap.  Actor 375 is the intentional edge
# case: 0x0213c434 is named as descriptor data in both ov006 and ov098, but only
# ov006 points at an operator-new factory.  The stricter join rejects ov098's stale
# MgShellSmash_SpawnInfo alias rather than turning address overlap into false identity.
PILOT_SELECTION = [
    (0, "arm9", "base process; scene/process side of the shared registry"),
    (3, "arm9", "normal stage scene; scene/process side of the registry"),
    (5, "ov005", "minigame root scene/process; tracked overlay gloss is stale"),
    (14, "ov002", "object actor with migrated readable C++"),
    (36, "ov010", "true overlay-multiplexed registry pointer; paired with CT_MECHA12L"),
    (177, "ov002", "base-vtable-only factory exception; class identity unresolved"),
    (191, "ov002", "factory delegates construction to a standalone constructor"),
    (197, "ov002", "canonical WATERFALL worked example"),
    (200, "ov084", "ordinary enemy with ROM RTTI name unlike English gloss"),
    (209, "ov063", "first of two profiles constructing daTrs_c"),
    (210, "ov063", "second daTrs_c profile; tests classInit name collision"),
    (219, "ov014", "enemy/actor with established RTTI and vtable"),
    (229, "ov085", "required OBJ_MIP_KEY; current community RabbitKey naming"),
    (232, "ov070", "enemy with existing migrated C++"),
    (319, "ov002", "object-tag actor with prior evidence-bounded factory rename"),
    (325, "ov020", "profile in a recovered multi-class, multi-profile TU"),
    (328, "ov020", "related profile in the same recovered TU"),
    (361, "ov006", "minigame scene/process entry"),
    (374, "ov006", "required MG_CURLING profile/class mismatch"),
    (375, "ov006", "required MG_CURLING_J; tests same-address false-profile rejection"),
]

# Ten actor ids share five registry pointers.  Every overlay involved loads at
# 0x021111a0 and they are mutually exclusive, so one numeric pointer decodes as a
# valid profile descriptor in two overlays at once.  The pointer alone therefore
# cannot say which overlay's descriptor an actor uses, and SM64DS offers no
# actor-id -> overlay table to ask: these overlays are selected per level, and a
# full scan of arm9_dec.bin for a 391-entry table of overlay ids finds none.
#
# Four independent cartridge lines of evidence nevertheless agree on one 1:1
# assignment for every pair, so the join is recorded here rather than guessed:
#
#   1. each overlay holds its OWN descriptor at the shared address, and that
#      descriptor's factory word equals the overlay's own operator-new factory
#      (verified: all 20 candidate rows, desc_word0 == registry factory_address);
#   2. each overlay contains exactly one Itanium-mangled RTTI name string in the
#      cartridge -- "14daObjC1_Trap_c" in ov010 against "16daObjCtMecha10_c" in
#      ov035, and so on for all ten;
#   3. the arm9 debug profile-id string transliterates onto that class name
#      (C1_TRAP/daObjC1_Trap_c, MC_METALNET/daObjMc_Metalnet_c, ...);
#   4. names already carried in the tree agree, and were applied independently of
#      this table: ov026 g_profile_WL_POLELIFT, ov033 g_profile_TT_FUTA, plus the
#      surviving coined spawn-info names ov009 MetalNet_SpawnInfo, ov027
#      SlidingIce_SpawnInfo and ov043 StairsBdw_SpawnInfo (Bdw is the KM1 stage
#      tag used across the seesaw and sliding-platform families).
#
# This is a name-correspondence join over ROM-proven endpoints, not a byte proof
# of overlay selection.  registry_candidate_count and overlay_ambiguous stay at
# their measured values so the underlying ambiguity is never erased.
FACTORY_SETTLED = (
    "unique_class_factory",
    "disambiguated_by_profile_id",
)

OVERLAY_SETTLED = (
    "unique_registry_context",
    "resolved_by_rtti_name_correspondence",
)

MULTIPLEXED_RESOLUTION = {
    36: ("ov010", "daObjC1_Trap_c"),
    121: ("ov035", "daObjCtMecha10_c"),
    339: ("ov009", "daObjMc_Metalnet_c"),
    89: ("ov026", "daObjWlPolelift_c"),
    93: ("ov027", "daObjSlIceBlock_c"),
    228: ("ov032", "daBakubaku_c"),
    104: ("ov033", "daObjTtFuta_c"),
    156: ("ov047", "daObjKm3_Kaitendai_c"),
    134: ("ov043", "daObjKm1_Dorifu_c"),
    174: ("ov052", "daObjEmmLog_c"),
}

SYM_RE = re.compile(
    r"^(\S+)\s+kind:(function|data|bss)\([^)]*\)\s+addr:(0x[0-9a-fA-F]+)"
)
PLACEHOLDER_RE = re.compile(r"^(?:func|data|bss)_(?:ov\d+_)?[0-9a-fA-F]{8}$")
SIZE_ASSERT_RE = re.compile(
    r"typedef\s+char\s+(\w+)_size_must_be_0x([0-9a-fA-F]+)\s*\["
)

TSV_COLUMNS = [
    "actor_id",
    "profile_id",
    "profile_id_source",
    "debug_string_address",
    "class_name",
    "class_name_source",
    "rtti_address",
    "rtti_name_address",
    "inheritance_chain",
    "overlay",
    "registry_candidate_count",
    "overlay_resolution",
    "registry_candidates",
    "tu_start",
    "tu_end",
    "tu_left_boundary_confidence",
    "tu_right_boundary_confidence",
    "tu_classes",
    "tu_profiles",
    "static_initializers",
    "static_initializer_evidence",
    "factory_address",
    "factory_size",
    "registry_factory_reference_count",
    "registry_factory_references",
    "current_factory_name",
    "current_factory_file",
    "proposed_factory_name",
    "factory_shape",
    "factory_shape_evidence",
    "factory_via",
    "class_factory_count",
    "proposed_factory_collision",
    "factory_name_resolution",
    "profile_address",
    "current_profile_name",
    "proposed_profile_name",
    "descriptor_layout",
    "profile_index",
    "profile_index_matches_actor_id",
    "execute_order",
    "draw_order",
    "group_flags",
    "group_flags_evidence",
    "actor_flags",
    "clip_offset_y",
    "clip_radius",
    "clip_distance",
    "far_distance",
    "alloc_size",
    "class_size",
    "installed_vtable",
    "expected_vtable",
    "factory_ordinal_in_tu",
    "factory_is_last_in_tu",
    "profile_alignment",
    "profile_to_rtti_delta",
    "profile_to_vtable_delta",
    "data_order",
    "class_filename_candidate",
    "probable_filename",
    "tu_filename_candidates",
    "filename_contradictions",
    "filename_evidence",
    "factory_confidence",
    "profile_name_confidence",
    "filename_confidence",
    "notes",
    "selection_reason",
]

RENAME_COLUMNS = [
    "current_symbol",
    "proposed_symbol",
    "symbol_kind",
    "profile_id",
    "class_name",
    "confidence",
    "evidence",
    "apply_recommended",
]

FULL_COLUMNS = [
    "actor_id",
    "profile_id",
    "profile_id_source",
    "debug_string_address",
    "candidate_ordinal",
    "registry_candidate_count",
    "overlay_ambiguous",
    "overlay_resolution",
    "overlay",
    "profile_address",
    "current_profile_name",
    "proposed_profile_name",
    "descriptor_layout",
    "profile_index",
    "profile_index_matches_actor_id",
    "execute_order",
    "draw_order",
    "group_flags",
    "group_flags_evidence",
    "actor_flags",
    "clip_offset_y",
    "clip_radius",
    "clip_distance",
    "far_distance",
    "factory_module",
    "factory_address",
    "current_factory_name",
    "current_factory_file",
    "proposed_factory_name",
    "factory_shape",
    "alloc_size",
    "installed_vtable",
    "class_name",
    "class_name_source",
    "proposed_factory_collision",
    "factory_name_resolution",
    "class_filename_candidate",
    "factory_filename",
    "tu_start",
    "tu_end",
    "profile_rename_recommended",
    "factory_rename_recommended",
    "recommendation_evidence",
]


def hx(value: int | None) -> str:
    return "" if value is None else f"0x{value:08x}"


def signed_hx(value: int | None) -> str:
    if value is None:
        return ""
    sign = "+" if value >= 0 else "-"
    return f"{sign}0x{abs(value):x}"


def load_json(path: pathlib.Path):
    return json.loads(path.read_text(encoding="utf-8"))


def run_tool(*args: str) -> None:
    print("[derive]", " ".join(args), file=sys.stderr)
    subprocess.run([sys.executable, *args], cwd=REPO, check=True)


def ensure_derived(force: bool, no_refresh: bool) -> None:
    required = (RTTI, RTTI_VTABLES, TU_MAP, TU_NAMES, OPNEW, SINIT_OWNERS)
    if not force and all(p.is_file() for p in required):
        return
    if no_refresh:
        missing = ", ".join(str(p.relative_to(REPO)) for p in required if not p.is_file())
        raise SystemExit(f"missing derived inputs: {missing}; rerun without --no-refresh")

    run_tool("tools/rtti_extract.py")
    run_tool("tools/evidence_hierarchy.py")
    run_tool("tools/rtti_reconcile.py")
    run_tool("tools/rtti_vtables.py")
    run_tool("tools/tu_map.py")
    run_tool("tools/tu_names.py")
    run_tool("tools/opnew_sizes.py")
    run_tool("tools/sinit_owners.py", "--show", "none")


def config_paths():
    yield "arm9", REPO / "config" / "arm9" / "symbols.txt"
    for d in sorted((REPO / "config" / "arm9" / "overlays").glob("ov*")):
        yield d.name, d / "symbols.txt"


def load_symbol_aliases():
    by_addr = collections.defaultdict(list)
    by_name = collections.defaultdict(list)
    kinds = {}
    for mod, path in config_paths():
        if not path.is_file():
            continue
        for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
            m = SYM_RE.match(line)
            if not m:
                continue
            name, kind, addr_text = m.groups()
            addr = int(addr_text, 16)
            by_addr[(mod, addr)].append(name)
            by_name[name].append((mod, addr))
            kinds[(mod, addr, name)] = kind
    return by_addr, by_name, kinds


def best_symbol(names, kind: str) -> str:
    if not names:
        return ""

    def score(name: str):
        points = 0
        if not PLACEHOLDER_RE.match(name):
            points += 100
        if kind == "profile" and name.endswith("_SpawnInfo"):
            points += 30
        if kind == "factory" and name.endswith("_Spawn"):
            points += 30
        if name.startswith(".p"):
            points -= 100
        return points, name

    return max(names, key=score)


def load_debug_names():
    path = REPO / "symbols" / "actor_debug_names.tsv"
    out = {}
    with path.open(encoding="utf-8", newline="") as fh:
        for row in csv.reader(fh, delimiter="\t"):
            if not row:
                continue
            actor_id, name, addr = row
            actor_id = int(actor_id)
            if actor_id < N_ACTORS:  # row 391 is the ROM's END_OF sentinel
                out[actor_id] = (name, int(addr, 16))
    if len(out) != N_ACTORS:
        raise SystemExit(f"expected {N_ACTORS} actor debug names, found {len(out)}")
    return out


def load_community_glosses():
    """Tier-C English labels from the tracked actor/overlay report.

    actor_names.load_actor_names() reads the gitignored DynamicAllocationDecomp
    checkout, which is not one of the wired-worktree inputs.  The generated tracked
    report carries the same index join and makes this optional comparison portable.
    """
    text = (REPO / "symbols" / "overlay_actors.md").read_text(
        encoding="utf-8", errors="replace"
    )
    return {int(i): name for name, i in re.findall(r"([A-Za-z0-9_]+)\((\d+)\)", text)}


def word(blob: bytes, base: int, addr: int) -> int | None:
    off = addr - base
    if not 0 <= off <= len(blob) - 4:
        return None
    return struct.unpack_from("<I", blob, off)[0]


def descriptor_candidates(
    ptr: int,
    modules,
    syms,
    aliases,
    opnew_factories,
):
    """All valid module interpretations of one registry pointer.

    Overlay addresses overlap, so one numeric pointer can intentionally have more
    than one valid interpretation.  This function returns all of them and leaves the
    sample definition to select the context it is testing.
    """
    out = []
    arm9 = next((x for x in modules if x[0] == "arm9"), None)
    for mod, base, blob in modules:
        off = ptr - base
        if not 0 <= off <= len(blob) - 0x08:
            continue
        fn, bp, rp = struct.unpack_from("<Ihh", blob, off)
        if fn & 3:
            continue

        possible_fmods = []
        if syms.get(mod, {}).get(fn, (None, None, None))[1] == "function":
            possible_fmods.append(mod)
        if arm9 and syms.get("arm9", {}).get(fn, (None, None, None))[1] == "function":
            possible_fmods.append("arm9")
        if not possible_fmods:
            continue
        if not (-2000 <= bp <= 2000 and -2000 <= rp <= 2000):
            continue

        flags = yoff = rng = clip_distance = far_distance = None
        layout = "base_profile_0x08"
        if off <= len(blob) - 0x1C:
            full = struct.unpack_from("<IhhIiiii", blob, off)
            _fn, _bp, _rp, flags0, yoff0, rng0, clip0, far0 = full
            if all(
                v == 0 or 0x100 <= v <= 0x10000000
                for v in (yoff0, rng0, clip0, far0)
            ):
                flags, yoff, rng, clip_distance, far_distance = (
                    flags0,
                    yoff0,
                    rng0,
                    clip0,
                    far0,
                )
                layout = "actor_profile_0x1c"

        # The short layout has no range words to validate, so require a configured
        # data symbol at the descriptor address.  This rejects incidental code/string
        # bytes in overlapping overlays while retaining unnamed data_ placeholders.
        profile_sym = syms.get(mod, {}).get(ptr)
        if layout == "base_profile_0x08" and (
            profile_sym is None or profile_sym[1] != "data"
        ):
            continue
        # Same-overlay code is the strongest interpretation.  The alternative arm9
        # interpretation is retained only when no same-overlay function exists.
        fmod = mod if mod in possible_fmods else possible_fmods[0]
        # A registry profile factory must construct a process.  Requiring the
        # repository's independently recovered operator-new site prevents an
        # overlapping overlay's ordinary virtual-method table from masquerading as
        # a profile.  If a real custom factory exists, its actor will fail closed
        # here and become an explicit exception instead of a guessed join.
        if (fmod, fn) not in opnew_factories:
            continue
        out.append(
            {
                "module": mod,
                "factory_module": fmod,
                "profile_address": ptr,
                "factory_address": fn,
                "descriptor_layout": layout,
                "execute_order": bp,
                "draw_order": rp,
                "actor_flags": flags,
                "clip_offset_y": yoff,
                "clip_radius": rng,
                "clip_distance": clip_distance,
                "far_distance": far_distance,
                "current_profile_name": best_symbol(aliases.get((mod, ptr), []), "profile"),
                "current_factory_name": best_symbol(aliases.get((fmod, fn), []), "factory"),
            }
        )
    return sorted(out, key=lambda r: (r["module"], r["factory_module"]))


def choose_candidate(candidates, preferred_module: str):
    exact = [r for r in candidates if r["module"] == preferred_module]
    if len(exact) == 1:
        return exact[0]
    if len(exact) > 1:
        same = [r for r in exact if r["factory_module"] == preferred_module]
        if len(same) == 1:
            return same[0]
    raise ValueError(
        f"cannot choose {preferred_module} from "
        f"{[(r['module'], r['factory_module']) for r in candidates]}"
    )


def candidate_summary(row):
    return {
        "overlay": row["module"],
        "factory_module": row["factory_module"],
        "profile_address": hx(row["profile_address"]),
        "current_profile_name": row["current_profile_name"],
        "factory_address": hx(row["factory_address"]),
        "current_factory_name": row["current_factory_name"],
    }


def rtti_indexes(rtti):
    by_vtable = collections.defaultdict(list)
    edges = collections.defaultdict(list)
    for key, record in rtti["records"].items():
        if record.get("vtable") and record.get("vtable_module"):
            by_vtable[(record["vtable_module"], int(record["vtable"], 16))].append(
                (key, record)
            )
    for edge in rtti["edges"]:
        edges[edge["derived_key"]].append(edge)
    return by_vtable, edges


def inheritance_chain(key: str, records, edges) -> str:
    names = []
    seen = set()
    while key and key not in seen:
        seen.add(key)
        rec = records.get(key)
        if not rec:
            break
        names.append(rec["name"])
        bases = sorted(edges.get(key, []), key=lambda e: (e.get("offset", 0), e["base_key"]))
        primary = next((e for e in bases if e.get("offset", 0) == 0), None)
        key = primary["base_key"] if primary else None
    return " -> ".join(names)


def load_header_sizes():
    sizes = {}
    for path in sorted((REPO / "include").glob("*.h")):
        text = path.read_text(encoding="utf-8", errors="replace")
        for name, size in SIZE_ASSERT_RE.findall(text):
            sizes[name] = int(size, 16)
    return sizes


def source_for(symbol: str) -> str:
    if not symbol:
        return ""
    try:
        path = SP.path_for(symbol)
    except (KeyError, ValueError):
        return ""
    if not path:
        return ""
    try:
        return path.relative_to(REPO).as_posix()
    except ValueError:
        return path.as_posix()


def unit_for(tu_map, mod: str, addr: int):
    map_mod = "main" if mod == "arm9" else mod
    units = tu_map.get("modules", {}).get(map_mod, {}).get("units", [])
    for i, unit in enumerate(units):
        if int(unit["start"], 16) <= addr < int(unit["end"], 16):
            left = ""
            right = ""
            if i:
                left = boundary_confidence(units[i - 1], unit)
            if i + 1 < len(units):
                right = boundary_confidence(unit, units[i + 1])
            return unit, left, right
    return None, "", ""


def tu_name_unit_for(tu_names, mod: str, start: str):
    map_mod = "main" if mod == "arm9" else mod
    wanted = int(start, 16) if start else None
    for unit in tu_names.get("units", []):
        if (
            unit.get("module") == map_mod
            and wanted is not None
            and int(unit.get("start", "0"), 16) == wanted
        ):
            return unit
    return None


def static_initializers_for(sinit_owners, tu_map, mod: str, unit):
    if not unit:
        return []
    map_mod = "main" if mod == "arm9" else mod
    units = tu_map.get("modules", {}).get(map_mod, {}).get("units", [])
    unit_index = next(
        (
            i
            for i, candidate in enumerate(units)
            if candidate.get("start") == unit.get("start")
            and candidate.get("end") == unit.get("end")
        ),
        None,
    )
    if unit_index is None:
        return []
    candidate_id = f"{map_mod}:{unit_index}"
    return [
        {
            "symbol": row["symbol"],
            "address": row["address"],
            "size": row["size"],
            "classification": row["classification"],
            "causes": row.get("causes", []),
            "evidence": row.get("evidence", []),
            "blockers": row.get("blockers", []),
        }
        for row in sinit_owners.get("initializers", [])
        if row.get("candidate") == candidate_id
    ]


def boundary_confidence(prev, nxt):
    a, b = bool(prev.get("classes")), bool(nxt.get("classes"))
    if a and b:
        return "high"
    if a or b:
        return "medium"
    return "low"


def expected_vtable_symbol(record) -> str:
    if not record:
        return ""
    return "_ZTV" + record["mangled"]


def is_base_vtable_only(site, rtti, opnew_classes) -> bool:
    if not site or not site.get("class"):
        return False
    bases = {edge["base"] for edge in rtti["edges"]}
    rec = opnew_classes.get(site["class"], {})
    aliases = set(rec.get("aliases", [])) | {site["class"]}
    factory_stem = re.sub(r"_Spawn$", "", site.get("factory") or "")
    return (
        site.get("attribution") == "ctor_inline_store"
        and site["class"] in bases
        and factory_stem not in aliases
    )


def factory_shape(site, base_only: bool):
    if not site:
        return "custom_factory", "no fBase_c::operator new site was attributed"
    if base_only:
        return (
            "uncertain",
            "allocation is exact, but the called base constructor leaves the dActor_c "
            "vtable installed; no most-derived RTTI identity survives",
        )
    if site["attribution"].startswith("ctor_"):
        return (
            "new_plus_ctor_call",
            f"operator new size is exact; final vptr recovered through {site.get('via') or 'called constructor'}",
        )
    if site["attribution"] == "inline_store":
        return (
            "new_plus_inlined_ctor",
            "operator new size is exact and the most-derived RTTI vtable is stored in the factory",
        )
    return "uncertain", f"opnew attribution={site.get('attribution', 'unknown')}"


def make_observations(refresh: bool, no_refresh: bool):
    ensure_derived(refresh, no_refresh)
    modules = AN.load_modules()
    syms, _all_names, _files, fn_sizes = AN.load_config()
    aliases, _by_name, _kinds = load_symbol_aliases()
    debug_names = load_debug_names()
    actor_glosses = load_community_glosses()
    rtti = load_json(RTTI)
    tu_map = load_json(TU_MAP)
    tu_names = load_json(TU_NAMES)
    opnew = load_json(OPNEW)
    sinit_owners = load_json(SINIT_OWNERS)
    header_sizes = load_header_sizes()
    by_vtable, edges = rtti_indexes(rtti)

    arm9 = next(blob for mod, base, blob in modules if mod == "arm9")
    site_by_factory = {
        (s["module"], int(s["factory_addr"], 16)): s for s in opnew["sites"]
    }

    all_candidates = {}
    all_chosen = []
    for actor_id in range(N_ACTORS):
        ptr = word(arm9, ARM9_BASE, TABLE + 4 * actor_id)
        candidates = (
            descriptor_candidates(ptr, modules, syms, aliases, site_by_factory)
            if ptr
            else []
        )
        all_candidates[actor_id] = candidates
        if candidates:
            # General inventory preference: a same-module factory, then module name.
            chosen = min(
                candidates,
                key=lambda r: (r["module"] != r["factory_module"], r["module"]),
            )
            all_chosen.append((actor_id, chosen))

    selected = []
    for actor_id, preferred_module, reason in PILOT_SELECTION:
        profile_id, debug_addr = debug_names[actor_id]
        candidates = all_candidates[actor_id]
        if not candidates:
            raise SystemExit(f"actor {actor_id} {profile_id}: no valid descriptor candidate")
        chosen = dict(choose_candidate(candidates, preferred_module))
        chosen.update(
            {
                "actor_id": actor_id,
                "profile_id": profile_id,
                "debug_address": debug_addr,
                "selection_reason": reason,
                "registry_candidates": [candidate_summary(r) for r in candidates],
                "community_actor_gloss": actor_glosses.get(actor_id, ""),
            }
        )
        selected.append(chosen)

    # A candidate filename/factory name cannot be reviewed in isolation from every
    # other registry entry.  Detect classInit collisions over the full inventory.
    #
    # A superseded multiplex candidate is a phantom: its factory address belongs
    # to the overlay that won the pointer, not to this actor id.  Counting it
    # here would invent a collision, so the census runs over live rows only.
    factory_proposals = collections.defaultdict(set)
    factory_addr_profiles = collections.defaultdict(lambda: collections.defaultdict(set))
    for actor_id, candidates in all_candidates.items():
        for chosen in candidates:
            if pilot_overlay_resolution(
                actor_id, chosen["module"], len(candidates)
            ).startswith("superseded_by_"):
                continue
            site = site_by_factory.get(
                (chosen["factory_module"], chosen["factory_address"])
            )
            if (
                site
                and site.get("class")
                and not is_base_vtable_only(site, rtti, opnew["classes"])
            ):
                key = f"{site['class']}_classInit"
                addr = (chosen["factory_module"], chosen["factory_address"])
                factory_proposals[key].add(addr)
                factory_addr_profiles[key][addr].add(debug_names[actor_id][0])

    factory_registry_refs = collections.defaultdict(list)
    for actor_id, candidates in sorted(all_candidates.items()):
        profile_id = debug_names[actor_id][0]
        for candidate in candidates:
            factory_registry_refs[
                (candidate["factory_module"], candidate["factory_address"])
            ].append(
                {
                    "actor_id": actor_id,
                    "profile_id": profile_id,
                    "overlay": candidate["module"],
                    "profile_address": hx(candidate["profile_address"]),
                }
            )

    full_rows = []
    for actor_id, candidates in sorted(all_candidates.items()):
        profile_id, debug_addr = debug_names[actor_id]
        for ordinal, candidate in enumerate(candidates, 1):
            fkey = (candidate["factory_module"], candidate["factory_address"])
            site = site_by_factory.get(fkey)
            base_only = is_base_vtable_only(site, rtti, opnew["classes"])
            shape, _shape_evidence = factory_shape(site, base_only)
            class_name = "" if base_only or not site else site.get("class") or ""
            vtable = int(site["vtable"], 16) if site and site.get("vtable") else None
            rtti_matches = (
                by_vtable.get((candidate["factory_module"], vtable), [])
                if vtable
                else []
            )
            if not rtti_matches and site and site.get("resolved_by") == "arm9" and vtable:
                rtti_matches = by_vtable.get(("arm9", vtable), [])
            rtti_key, rtti_record = (
                rtti_matches[0] if len(rtti_matches) == 1 else ("", None)
            )
            if rtti_record and class_name and rtti_record["name"] != class_name:
                raise SystemExit(
                    f"{profile_id}: opnew class {class_name} != RTTI {rtti_record['name']}"
                )
            if not class_name:
                rtti_key, rtti_record = "", None

            chain = (
                inheritance_chain(rtti_key, rtti["records"], edges)
                if rtti_key
                else ""
            )
            descriptor_layout = candidate["descriptor_layout"]
            if "dScene_c" in chain:
                descriptor_layout = "base_profile_0x08"
            elif "dActor_c" in chain or base_only:
                descriptor_layout = "actor_profile_0x1c"

            base_factory = f"{class_name}_classInit" if class_name else ""
            collisions = sorted(
                factory_proposals.get(base_factory, set())
            ) if base_factory else []
            # The collision stays measured even once it is resolved: this text
            # is the record of how many factories share the one class.
            collision_text = (
                ";".join(f"{m}:{hx(a)}" for m, a in collisions)
                if len(collisions) > 1
                else ""
            )
            unit, _left_conf, _right_conf = unit_for(
                tu_map, candidate["factory_module"], candidate["factory_address"]
            )
            tu_name = tu_names.get("classes", {}).get(class_name, {}) if class_name else {}
            filename_candidate = (
                f"{tu_name['stem']}.cpp" if tu_name.get("stem") else ""
            )

            resolved_module, resolved_class = MULTIPLEXED_RESOLUTION.get(
                actor_id, (None, None)
            )
            if len(candidates) == 1:
                overlay_resolution = "unique_registry_context"
            elif resolved_module is None:
                overlay_resolution = "unresolved_overlay_multiplex"
            elif candidate["module"] == resolved_module:
                # Guard the table against a regenerated class identity: the join is
                # only honoured while the overlay still carries the RTTI name it was
                # resolved on.
                if class_name != resolved_class:
                    raise SystemExit(
                        f"actor {actor_id} {profile_id}: multiplex resolution expects "
                        f"{resolved_class} in {resolved_module}, found {class_name!r}"
                    )
                overlay_resolution = "resolved_by_rtti_name_correspondence"
            else:
                overlay_resolution = f"superseded_by_{resolved_module}"

            if overlay_resolution.startswith("superseded_by_"):
                # A superseded candidate is a phantom reading of the winning
                # overlay's factory.  It is never named, so it is never
                # disambiguated either -- asking would make the census contradict
                # itself, because the census counts live rows only.
                proposed_factory = base_factory
                factory_name_resolution = "superseded_candidate"
            else:
                proposed_factory, factory_name_resolution = resolve_factory_name(
                    class_name,
                    profile_id,
                    (candidate["factory_module"], candidate["factory_address"]),
                    factory_proposals,
                    factory_addr_profiles,
                )

            recommendation_reasons = []
            if overlay_resolution == "unresolved_overlay_multiplex":
                recommendation_reasons.append("overlay_multiplexed_registry_pointer")
            elif overlay_resolution.startswith("superseded_by_"):
                recommendation_reasons.append(
                    f"overlay_multiplex_{overlay_resolution}"
                )
            if base_only or not class_name:
                recommendation_reasons.append("most_derived_class_unresolved")
            # A superseded row was never a naming candidate, so reporting the
            # class's collision against it would read as unresolvable when it is
            # in fact resolved for every live row.
            if (
                collision_text
                and factory_name_resolution not in FACTORY_SETTLED
                and factory_name_resolution != "superseded_candidate"
            ):
                recommendation_reasons.append("global_classinit_name_collision")
            if not candidate["current_factory_name"]:
                recommendation_reasons.append("current_factory_symbol_unresolved")
            if not candidate["current_profile_name"]:
                recommendation_reasons.append("current_profile_symbol_unresolved")

            overlay_settled = overlay_resolution in OVERLAY_SETTLED
            profile_recommended = overlay_settled and bool(
                candidate["current_profile_name"]
            )
            factory_recommended = (
                overlay_settled
                and bool(candidate["current_factory_name"])
                and bool(class_name)
                and factory_name_resolution in FACTORY_SETTLED
            )
            full_rows.append(
                {
                    "actor_id": actor_id,
                    "profile_id": profile_id,
                    "profile_id_source": (
                        "symbols/actor_debug_names.tsv (literal ROM string table)"
                    ),
                    "debug_string_address": hx(debug_addr),
                    "candidate_ordinal": ordinal,
                    "registry_candidate_count": len(candidates),
                    "overlay_ambiguous": len(candidates) > 1,
                    "overlay_resolution": overlay_resolution,
                    "overlay": candidate["module"],
                    "profile_address": hx(candidate["profile_address"]),
                    "current_profile_name": candidate["current_profile_name"],
                    "proposed_profile_name": f"g_profile_{profile_id}",
                    "descriptor_layout": descriptor_layout,
                    "profile_index": candidate["execute_order"],
                    "profile_index_matches_actor_id": (
                        candidate["execute_order"] == actor_id
                    ),
                    "execute_order": candidate["execute_order"],
                    "draw_order": candidate["draw_order"],
                    "group_flags": "",
                    "group_flags_evidence": (
                        "unlocated: no separate group-flags field is read from this "
                        "descriptor by the recovered fBase_c/dActor_c constructors"
                    ),
                    "actor_flags": (
                        hx(candidate["actor_flags"])
                        if descriptor_layout == "actor_profile_0x1c"
                        else ""
                    ),
                    "clip_offset_y": (
                        hx(candidate["clip_offset_y"])
                        if descriptor_layout == "actor_profile_0x1c"
                        else ""
                    ),
                    "clip_radius": (
                        hx(candidate["clip_radius"])
                        if descriptor_layout == "actor_profile_0x1c"
                        else ""
                    ),
                    "clip_distance": (
                        hx(candidate["clip_distance"])
                        if descriptor_layout == "actor_profile_0x1c"
                        else ""
                    ),
                    "far_distance": (
                        hx(candidate["far_distance"])
                        if descriptor_layout == "actor_profile_0x1c"
                        else ""
                    ),
                    "factory_module": candidate["factory_module"],
                    "factory_address": hx(candidate["factory_address"]),
                    "current_factory_name": candidate["current_factory_name"],
                    "current_factory_file": source_for(
                        candidate["current_factory_name"]
                    ),
                    "proposed_factory_name": proposed_factory,
                    "factory_shape": shape,
                    "alloc_size": hx(site["size"]) if site else "",
                    "installed_vtable": hx(vtable),
                    "class_name": class_name,
                    "class_name_source": (
                        f"SM64DS RTTI {rtti_record['module']}:{rtti_record['addr']}"
                        if rtti_record
                        else ""
                    ),
                    "proposed_factory_collision": collision_text,
                    "factory_name_resolution": factory_name_resolution,
                    "class_filename_candidate": filename_candidate,
                    # Filled in by assign_factory_filenames() once every row is
                    # known: the target stem's uniqueness is a property of the
                    # whole inventory, not of one row.
                    "factory_filename": "",
                    "tu_start": unit["start"] if unit else "",
                    "tu_end": unit["end"] if unit else "",
                    "profile_rename_recommended": (
                        "yes" if profile_recommended else "no"
                    ),
                    "factory_rename_recommended": (
                        "yes" if factory_recommended else "no"
                    ),
                    "recommendation_evidence": (
                        "eligible=unique_registry_context+rom_profile_id"
                        + (
                            "+unique_class_factory"
                            if factory_recommended
                            else ""
                        )
                        if not recommendation_reasons
                        else "not_apply=" + "+".join(recommendation_reasons)
                    ),
                }
            )

    rows = []
    for chosen in selected:
        fkey = (chosen["factory_module"], chosen["factory_address"])
        site = site_by_factory.get(fkey)
        base_only = is_base_vtable_only(site, rtti, opnew["classes"])
        shape, shape_evidence = factory_shape(site, base_only)

        class_name = "" if base_only or not site else site.get("class") or ""
        vtable = int(site["vtable"], 16) if site and site.get("vtable") else None
        rtti_matches = by_vtable.get((chosen["factory_module"], vtable), []) if vtable else []
        if not rtti_matches and site and site.get("resolved_by") == "arm9" and vtable:
            rtti_matches = by_vtable.get(("arm9", vtable), [])
        rtti_key, rtti_record = rtti_matches[0] if len(rtti_matches) == 1 else ("", None)
        if rtti_record and class_name and rtti_record["name"] != class_name:
            raise SystemExit(
                f"{chosen['profile_id']}: opnew class {class_name} != RTTI {rtti_record['name']}"
            )
        if not class_name:
            rtti_key, rtti_record = "", None

        unit, left_conf, right_conf = unit_for(
            tu_map, chosen["factory_module"], chosen["factory_address"]
        )
        static_initializers = static_initializers_for(
            sinit_owners, tu_map, chosen["factory_module"], unit
        )
        tu_name = tu_names.get("classes", {}).get(class_name, {}) if class_name else {}
        class_filename_candidate = f"{tu_name['stem']}.cpp" if tu_name.get("stem") else ""
        tu_name_unit = (
            tu_name_unit_for(tu_names, chosen["factory_module"], unit["start"])
            if unit
            else None
        )
        tu_filename_candidates = sorted(
            {f"{stem}.cpp" for stem in (tu_name_unit or {}).get("stems", [])}
        )
        filename_contradictions = ""
        if len(tu_filename_candidates) > 1:
            filename_contradictions = (
                "one recovered TU carries multiple distinct RTTI-derived filename stems"
            )
        probable_filename = (
            class_filename_candidate if class_filename_candidate and not filename_contradictions else ""
        )
        chosen_overlay_resolution = pilot_overlay_resolution(
            chosen["actor_id"], chosen["module"], len(chosen["registry_candidates"])
        )
        base_factory = f"{class_name}_classInit" if class_name else ""
        collisions = sorted(factory_proposals.get(base_factory, set())) if base_factory else []
        collision_text = ";".join(f"{m}:{hx(a)}" for m, a in collisions) if len(collisions) > 1 else ""
        if chosen_overlay_resolution.startswith("superseded_by_"):
            proposed_factory, factory_name_resolution = base_factory, "superseded_candidate"
        else:
            proposed_factory, factory_name_resolution = resolve_factory_name(
                class_name,
                chosen["profile_id"],
                (chosen["factory_module"], chosen["factory_address"]),
                factory_proposals,
                factory_addr_profiles,
            )

        class_rec = opnew["classes"].get(class_name, {}) if class_name else {}
        if not class_rec and site:
            for alias in [site.get("tree_class")] + site.get("tree_class_aliases", []):
                if alias and alias in opnew["classes"]:
                    class_rec = opnew["classes"][alias]
                    break
        class_size = header_sizes.get(class_name)
        if class_size is None:
            for alias in site.get("tree_class_aliases", []) if site else []:
                if alias in header_sizes:
                    class_size = header_sizes[alias]
                    break

        chain = inheritance_chain(rtti_key, rtti["records"], edges) if rtti_key else ""
        descriptor_layout = chosen["descriptor_layout"]
        if "dScene_c" in chain:
            descriptor_layout = "base_profile_0x08"
        elif "dActor_c" in chain or base_only:
            descriptor_layout = "actor_profile_0x1c"

        notes = []
        if len(chosen["registry_candidates"]) > 1:
            notes.append("numeric registry pointer has multiple valid overlay-local interpretations")
        if base_only:
            notes.append("most-derived class has no independently installed vtable; class name left blank")
        if collision_text:
            notes.append(
                "multiple factory addresses would collide on one global "
                "<Class>_classInit spelling"
                + (
                    "; disambiguated by ROM profile id"
                    if factory_name_resolution == "disambiguated_by_profile_id"
                    else ""
                )
            )
        if class_size is not None and site and class_size != site["size"]:
            notes.append("header size assertion differs from ROM allocation size")
        if chosen["factory_module"] == "arm9" and unit and len(unit.get("classes", [])) > 5:
            notes.append("main TU map is under-segmented; filename lacks TU-order corroboration")
        if filename_contradictions:
            notes.append(filename_contradictions + "; no probable filename selected")
        if chosen["profile_id"] != chosen["community_actor_gloss"]:
            notes.append(f"community actor gloss={chosen['community_actor_gloss']}")

        factory_conf = ""
        if shape in ("new_plus_inlined_ctor", "new_plus_ctor_call") and class_name:
            factory_conf = "B+" if not collision_text else "B"
        elif shape == "uncertain":
            factory_conf = "C"
        elif shape == "custom_factory":
            factory_conf = "C"

        profile_conf = "B+" if len(chosen["registry_candidates"]) == 1 else "B"
        filename_conf = ""
        if probable_filename and unit:
            if chosen["factory_module"] == "arm9" and len(unit.get("classes", [])) > 5:
                filename_conf = "C"
            else:
                filename_conf = "B" if left_conf == right_conf == "high" else "B-"

        filename_evidence = ""
        if probable_filename:
            filename_evidence = (
                "RTTI class-family stem from tools/tu_names.py + recovered text TU from "
                "tools/tu_map.py + later EAD d_a/d_s path convention; no SM64DS __FILE__ proof"
            )
        elif class_filename_candidate:
            filename_evidence = (
                "RTTI yields a mechanical class-family candidate, but recovered TU evidence "
                "does not select one filename; later EAD convention remains lineage-only"
            )

        functions = unit.get("functions", []) if unit else []
        factory_ordinal = (
            functions.index(chosen["current_factory_name"]) + 1
            if chosen["current_factory_name"] in functions
            else ""
        )
        factory_is_last = (
            factory_ordinal == len(functions) if factory_ordinal != "" else ""
        )
        rtti_addr = int(rtti_record["addr"], 16) if rtti_record else None
        rtti_name_addr = int(rtti_record["name_addr"], 16) if rtti_record else None
        data_points = [
            (rtti_addr, "rtti"),
            (rtti_name_addr, "rtti_name"),
            (chosen["profile_address"], "profile"),
            (vtable, "vtable"),
        ]
        data_order = " < ".join(
            name for addr, name in sorted((p for p in data_points if p[0] is not None))
        )

        row = {
            "actor_id": chosen["actor_id"],
            "profile_id": chosen["profile_id"],
            "profile_id_source": "symbols/actor_debug_names.tsv (literal ROM string table)",
            "debug_string_address": hx(chosen["debug_address"]),
            "class_name": class_name,
            "class_name_source": (
                f"SM64DS RTTI {rtti_record['module']}:{rtti_record['addr']}" if rtti_record else ""
            ),
            "rtti_address": rtti_record["addr"] if rtti_record else "",
            "rtti_name_address": rtti_record["name_addr"] if rtti_record else "",
            "inheritance_chain": chain,
            "overlay": chosen["module"],
            "registry_candidate_count": len(chosen["registry_candidates"]),
            "overlay_resolution": chosen_overlay_resolution,
            "registry_candidates": chosen["registry_candidates"],
            "tu_start": unit["start"] if unit else "",
            "tu_end": unit["end"] if unit else "",
            "tu_left_boundary_confidence": left_conf,
            "tu_right_boundary_confidence": right_conf,
            "tu_classes": unit.get("classes", []) if unit else [],
            "tu_profiles": [],
            "static_initializers": static_initializers,
            "static_initializer_evidence": (
                "directional global/consumer ownership from tools/sinit_owners.py; "
                ".init address adjacency is not used"
                if static_initializers
                else "no initializer assigned by tools/sinit_owners.py; absence is not proof"
            ),
            "factory_address": hx(chosen["factory_address"]),
            "factory_size": hx(fn_sizes.get(fkey)),
            "registry_factory_reference_count": len(factory_registry_refs[fkey]),
            "registry_factory_references": factory_registry_refs[fkey],
            "current_factory_name": chosen["current_factory_name"],
            "current_factory_file": source_for(chosen["current_factory_name"]),
            "proposed_factory_name": proposed_factory,
            "factory_shape": shape,
            "factory_shape_evidence": shape_evidence,
            "factory_via": site.get("via") or "" if site else "",
            "class_factory_count": class_rec.get("sites", ""),
            "proposed_factory_collision": collision_text,
            "factory_name_resolution": factory_name_resolution,
            "profile_address": hx(chosen["profile_address"]),
            "current_profile_name": chosen["current_profile_name"],
            "proposed_profile_name": f"g_profile_{chosen['profile_id']}",
            "descriptor_layout": descriptor_layout,
            # SM64DS uses the +0x04 halfword as its behavior/execute-list order,
            # and every selected row also stores the registry index there.  Emit
            # both interpretations so the dual use is visible rather than silently
            # choosing one source member name.
            "profile_index": chosen["execute_order"],
            "profile_index_matches_actor_id": (
                chosen["execute_order"] == chosen["actor_id"]
            ),
            "execute_order": chosen["execute_order"],
            "draw_order": chosen["draw_order"],
            "group_flags": "",
            "group_flags_evidence": (
                "unlocated: no separate group-flags field is read from this descriptor "
                "by the recovered fBase_c/dActor_c constructors"
            ),
            "actor_flags": (
                hx(chosen["actor_flags"]) if descriptor_layout == "actor_profile_0x1c" else ""
            ),
            "clip_offset_y": (
                hx(chosen["clip_offset_y"]) if descriptor_layout == "actor_profile_0x1c" else ""
            ),
            "clip_radius": (
                hx(chosen["clip_radius"]) if descriptor_layout == "actor_profile_0x1c" else ""
            ),
            "clip_distance": (
                hx(chosen["clip_distance"]) if descriptor_layout == "actor_profile_0x1c" else ""
            ),
            "far_distance": (
                hx(chosen["far_distance"]) if descriptor_layout == "actor_profile_0x1c" else ""
            ),
            "alloc_size": hx(site["size"]) if site else "",
            "class_size": hx(class_size),
            "installed_vtable": hx(vtable),
            "expected_vtable": expected_vtable_symbol(rtti_record),
            "factory_ordinal_in_tu": factory_ordinal,
            "factory_is_last_in_tu": factory_is_last,
            "profile_alignment": chosen["profile_address"] % 4,
            "profile_to_rtti_delta": signed_hx(
                rtti_addr - chosen["profile_address"] if rtti_addr is not None else None
            ),
            "profile_to_vtable_delta": signed_hx(
                vtable - chosen["profile_address"] if vtable is not None else None
            ),
            "data_order": data_order,
            "class_filename_candidate": class_filename_candidate,
            "probable_filename": probable_filename,
            "tu_filename_candidates": tu_filename_candidates,
            "filename_contradictions": filename_contradictions,
            "filename_evidence": filename_evidence,
            "factory_confidence": factory_conf,
            "profile_name_confidence": profile_conf,
            "filename_confidence": filename_conf,
            "selection_reason": chosen["selection_reason"],
            "notes": "; ".join(notes),
        }
        rows.append(row)

    # List every selected profile whose factory falls in the same recovered text TU.
    for row in rows:
        row["tu_profiles"] = [
            other["profile_id"]
            for other in rows
            if row["tu_start"]
            and row["tu_end"]
            and other["overlay"] == row["overlay"]
            and other["tu_start"] == row["tu_start"]
            and other["tu_end"] == row["tu_end"]
        ]
    inventory_context = {
        "registry_entries_resolved": len(all_chosen),
        "distinct_rom_profile_ids": len({name for name, _addr in debug_names.values()}),
        "rom_profile_id_collisions": sorted(
            name
            for name, count in collections.Counter(
                name for name, _addr in debug_names.values()
            ).items()
            if count > 1
        ),
        "registry_candidate_count_histogram": dict(
            sorted(collections.Counter(len(v) for v in all_candidates.values()).items())
        ),
        "distinct_classinit_proposals": len(factory_proposals),
        "classinit_proposals_with_multiple_factory_addresses": sum(
            1 for addresses in factory_proposals.values() if len(addresses) > 1
        ),
        "registry_entries_with_multiple_candidates": [
            {
                "actor_id": actor_id,
                "profile_id": debug_names[actor_id][0],
                "candidates": [candidate_summary(c) for c in candidates],
            }
            for actor_id, candidates in sorted(all_candidates.items())
            if len(candidates) > 1
        ],
        "registry_entries_with_profile_index_match": sum(
            any(c["execute_order"] == actor_id for c in candidates)
            for actor_id, candidates in all_candidates.items()
        ),
        "registry_entries_without_profile_index_match": [
            {
                "actor_id": actor_id,
                "profile_id": debug_names[actor_id][0],
                "candidate_execute_orders": sorted(
                    {c["execute_order"] for c in candidates}
                ),
            }
            for actor_id, candidates in sorted(all_candidates.items())
            if not any(c["execute_order"] == actor_id for c in candidates)
        ],
    }
    assign_factory_filenames(full_rows)
    return rows, full_rows, opnew, inventory_context


def tsv_value(value):
    if isinstance(value, (list, dict)):
        return json.dumps(value, separators=(",", ":"), sort_keys=True)
    return value


def write_tsv(path: pathlib.Path, columns, rows) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8", newline="") as fh:
        writer = csv.DictWriter(fh, fieldnames=columns, delimiter="\t", lineterminator="\n")
        writer.writeheader()
        for row in rows:
            writer.writerow({k: tsv_value(row.get(k, "")) for k in columns})


def assign_factory_filenames(full_rows) -> None:
    """Give every recommended factory rename a unique target source filename.

    The stem is the NSMBW-convention one that ``tools/tu_names.py`` derives from
    the ROM's own RTTI class name (``daObjKm2_Ami_Bou_c`` -> ``d_a_obj_km2_ami_bou``),
    which is where ``class_filename_candidate`` comes from.  That stem is a
    property of the CLASS, and a class with several registry entries has several
    factories, so the stem alone cannot name all of their files.  Those cases take
    the same disambiguator the symbol name takes -- the ROM profile id, lowercased
    -- giving ``d_a_trs_trap_teresapit.c`` beside ``d_a_trs_trap_kuribo.c``.

    The extension is copied from the factory's CURRENT source file and never
    changed.  A ``.c`` -> ``.cpp`` rename changes the language mode the pinned
    mwccarm compiles the file under and would cost the byte match, so the
    reconstructed filename inherits the extension rather than proposing one.

    The value is empty for a row with no recommended factory rename, no derived
    class stem, or no current source file: nothing to rename, nothing to name.
    """
    claimants = collections.defaultdict(set)
    for row in full_rows:
        stem = factory_filename_stem(row)
        if stem:
            claimants[stem].add((row["factory_module"], row["factory_address"]))
    for row in full_rows:
        stem = factory_filename_stem(row)
        if not stem:
            continue
        if len(claimants[stem]) > 1:
            stem = f"{stem}_{row['profile_id'].lower()}"
        row["factory_filename"] = stem + pathlib.PurePosixPath(
            row["current_factory_file"]
        ).suffix


def factory_filename_stem(row) -> str:
    """The undisambiguated stem a row would claim, or "" if it claims none."""
    if row["factory_rename_recommended"] != "yes":
        return ""
    if not row["class_filename_candidate"] or not row["current_factory_file"]:
        return ""
    return pathlib.PurePosixPath(row["class_filename_candidate"]).stem


def pilot_overlay_resolution(actor_id, module, candidate_count):
    """Same join as the full inventory, for the pilot sample's chosen candidate."""
    if candidate_count == 1:
        return "unique_registry_context"
    resolved = MULTIPLEXED_RESOLUTION.get(actor_id)
    if resolved is None:
        return "unresolved_overlay_multiplex"
    return (
        "resolved_by_rtti_name_correspondence"
        if module == resolved[0]
        else f"superseded_by_{resolved[0]}"
    )


def resolve_factory_name(
    class_name, profile_id, addr, factory_proposals, factory_addr_profiles
):
    """Name a per-profile factory, disambiguating a shared class by profile id.

    A class reached from several registry entries has one distinct factory
    function per entry -- seven separate seesaws, one daObjSeesaw_c -- so a bare
    <Class>_classInit cannot name them all.  Every component of the suffixed
    spelling is ROM-proven: the class from RTTI, the profile id from the arm9
    debug table, and the pairing between them from the registry descriptor's
    factory pointer.  Only the convention of joining them is ours.

    The suffix is a bijection or it is nothing.  If one factory address were
    reachable from two profile ids, the suffix would give one function two
    names, so that case raises rather than guessing which id owns it.
    """
    if not class_name:
        return "", ""
    base = f"{class_name}_classInit"
    if len(factory_proposals.get(base, set())) <= 1:
        return base, "unique_class_factory"
    owners = factory_addr_profiles.get(base, {}).get(addr, set())
    if owners != {profile_id}:
        raise SystemExit(
            f"{base} at {addr}: profile-id disambiguation is not a bijection; "
            f"row claims {profile_id!r}, census says {sorted(owners)}"
        )
    return f"{base}_{profile_id}", "disambiguated_by_profile_id"


def build_rename_rows(rows):
    out = []
    for row in rows:
        factory_yes = (
            bool(row["proposed_factory_name"])
            and row["factory_name_resolution"] in FACTORY_SETTLED
            and row["overlay_resolution"] in OVERLAY_SETTLED
            and row["factory_shape"] in ("new_plus_inlined_ctor", "new_plus_ctor_call")
        )
        if row["current_factory_name"]:
            factory_evidence = (
                "rtti+operator_new_size+vtable+factory_shape+nsmbw_lineage"
                if row["class_name"]
                else "allocation_only;most_derived_class_unresolved"
            )
            if row["factory_name_resolution"] == "disambiguated_by_profile_id":
                # The collision is real and still recorded; the suffix resolves it.
                factory_evidence += "+classinit_disambiguated_by_rom_profile_id"
            elif row["proposed_factory_collision"]:
                factory_evidence += ";not_apply=global_name_collision"
            if row["overlay_resolution"] not in OVERLAY_SETTLED:
                factory_evidence += (
                    f";not_apply=overlay_multiplex_{row['overlay_resolution']}"
                )
            if row["factory_shape"] not in (
                "new_plus_inlined_ctor",
                "new_plus_ctor_call",
            ):
                factory_evidence += f";not_apply=factory_shape_{row['factory_shape']}"
            out.append(
                {
                    "current_symbol": row["current_factory_name"],
                    "proposed_symbol": row["proposed_factory_name"],
                    "symbol_kind": "factory",
                    "profile_id": row["profile_id"],
                    "class_name": row["class_name"],
                    "confidence": row["factory_confidence"],
                    "evidence": factory_evidence,
                    "apply_recommended": "yes" if factory_yes else "no",
                }
            )

        profile_yes = row["overlay_resolution"] in OVERLAY_SETTLED
        if row["current_profile_name"]:
            profile_evidence = (
                "rom_profile_id+registry_descriptor+factory_pointer+nsmbw_lineage"
            )
            if row["overlay_resolution"] == "resolved_by_rtti_name_correspondence":
                profile_evidence += "+overlay_multiplex_rtti_name_correspondence"
            if not profile_yes:
                profile_evidence += (
                    f";not_apply=overlay_multiplex_{row['overlay_resolution']}"
                )
            out.append(
                {
                    "current_symbol": row["current_profile_name"],
                    "proposed_symbol": row["proposed_profile_name"],
                    "symbol_kind": "profile",
                    "profile_id": row["profile_id"],
                    "class_name": row["class_name"],
                    "confidence": row["profile_name_confidence"],
                    "evidence": profile_evidence,
                    "apply_recommended": "yes" if profile_yes else "no",
                }
            )
    return out


def check_rows(rows) -> None:
    if len(rows) != len(PILOT_SELECTION):
        raise SystemExit(f"CHECK FAIL: expected {len(PILOT_SELECTION)} rows, got {len(rows)}")
    required = {"WATERFALL", "OBJ_MIP_KEY", "MG_CURLING", "MG_CURLING_J"}
    got = {r["profile_id"] for r in rows}
    if not required <= got:
        raise SystemExit(f"CHECK FAIL: missing required profiles {sorted(required - got)}")
    for row in rows:
        if row["proposed_profile_name"] != f"g_profile_{row['profile_id']}":
            raise SystemExit(f"CHECK FAIL: profile proposal not ROM-ID-derived: {row['profile_id']}")
    curling_j = next(r for r in rows if r["profile_id"] == "MG_CURLING_J")
    if curling_j["registry_candidate_count"] != 1 or curling_j["overlay"] != "ov006":
        raise SystemExit("CHECK FAIL: MG_CURLING_J false overlay profile was not rejected")
    boos = [r for r in rows if r["profile_id"] in ("TERESA", "BOSS_TERESA")]
    if len(boos) == 2 and not all(r["proposed_factory_collision"] for r in boos):
        raise SystemExit("CHECK FAIL: daTrs_c classInit collision was not retained")
    print(f"CHECK OK: {len(rows)} pilot rows; required edge cases retained")


def check_full_rows(rows) -> None:
    actor_ids = {row["actor_id"] for row in rows}
    if actor_ids != set(range(N_ACTORS)):
        raise SystemExit(
            f"CHECK FAIL: full registry covers {len(actor_ids)}/{N_ACTORS} actor IDs"
        )
    keys = {
        (row["actor_id"], row["overlay"], row["profile_address"])
        for row in rows
    }
    if len(keys) != len(rows):
        raise SystemExit("CHECK FAIL: duplicate full-registry candidate rows")
    matched_ids = {
        row["actor_id"]
        for row in rows
        if row["profile_index_matches_actor_id"]
    }
    if matched_ids != set(range(N_ACTORS)):
        raise SystemExit(
            "CHECK FAIL: not every registry ID has a candidate whose +0x04 "
            "halfword equals the ID"
        )
    filenames = [row["factory_filename"] for row in rows if row["factory_filename"]]
    duplicates = sorted(
        name for name, n in collections.Counter(filenames).items() if n > 1
    )
    if duplicates:
        raise SystemExit(
            "CHECK FAIL: factory_filename is not unique: " + ", ".join(duplicates)
        )
    print(
        f"CHECK OK: {len(rows)} candidate rows cover all {N_ACTORS} registry IDs; "
        f"{len(filenames)} unique factory filenames"
    )


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--refresh-derived", action="store_true")
    ap.add_argument("--no-refresh", action="store_true")
    ap.add_argument("--check", action="store_true")
    ap.add_argument("--tsv", default=str(OUT_TSV))
    ap.add_argument("--json", default=str(OUT_JSON))
    ap.add_argument("--renames", default=str(OUT_RENAMES))
    ap.add_argument("--full-tsv", default=str(OUT_FULL_TSV))
    ap.add_argument("--full-json", default=str(OUT_FULL_JSON))
    args = ap.parse_args()

    rows, full_rows, opnew, inventory_context = make_observations(
        args.refresh_derived, args.no_refresh
    )
    rename_rows = build_rename_rows(rows)

    write_tsv(pathlib.Path(args.tsv), TSV_COLUMNS, rows)
    write_tsv(pathlib.Path(args.renames), RENAME_COLUMNS, rename_rows)
    write_tsv(pathlib.Path(args.full_tsv), FULL_COLUMNS, full_rows)
    payload = {
        "schema_version": 2,
        "generated_by": "tools/profile_reconstruction.py",
        "evidence_tiers": {
            "A": "direct SM64DS ROM or deterministic ROM-derived relationship",
            "B": "later-EAD-lineage reconstruction; exact SM64DS spelling not preserved",
            "C": "semantic/community alias or unresolved hypothesis",
        },
        "lineage_snapshot": {
            "repository": "NSMBW-Community/NSMBW-Decomp",
            "revision": NSMBW_REV,
            "profile_macro_source": NSMBW_PROFILE_URL,
            "lifecycle_source": NSMBW_BASE_URL,
            "filename_example": NSMBW_FILENAME_URL,
            "caveat": "public decomp source is lineage evidence, not direct SM64DS name proof",
        },
        "population_context": {
            "actor_registry_entries": N_ACTORS,
            "operator_new_sites": opnew["meta"]["stats"].get("call_sites", len(opnew["sites"])),
            "opnew_attribution_counts": dict(
                sorted(collections.Counter(s["attribution"] for s in opnew["sites"]).items())
            ),
            **inventory_context,
        },
        "pilot_summary": {
            "rows": len(rows),
            "descriptor_layout_counts": dict(
                sorted(collections.Counter(r["descriptor_layout"] for r in rows).items())
            ),
            "factory_shape_counts": dict(
                sorted(collections.Counter(r["factory_shape"] for r in rows).items())
            ),
            "factory_proposals_with_collision": sum(
                bool(r["proposed_factory_collision"]) for r in rows
            ),
            "profile_proposals": len(rows),
        },
        "selection": [
            {"actor_id": i, "preferred_module": mod, "reason": reason}
            for i, mod, reason in PILOT_SELECTION
        ],
        "rows": rows,
    }
    pathlib.Path(args.json).parent.mkdir(parents=True, exist_ok=True)
    pathlib.Path(args.json).write_text(
        json.dumps(payload, indent=2, sort_keys=False) + "\n",
        encoding="utf-8",
        newline="\n",
    )

    full_payload = {
        "schema_version": 1,
        "generated_by": "tools/profile_reconstruction.py",
        "row_granularity": (
            "one row per valid overlay-local interpretation of each registry pointer"
        ),
        "logical_registry_entries": N_ACTORS,
        "candidate_rows": len(full_rows),
        "lineage_snapshot": payload["lineage_snapshot"],
        "population_context": payload["population_context"],
        "rows": full_rows,
    }
    pathlib.Path(args.full_json).parent.mkdir(parents=True, exist_ok=True)
    pathlib.Path(args.full_json).write_text(
        json.dumps(full_payload, indent=2, sort_keys=False) + "\n",
        encoding="utf-8",
        newline="\n",
    )

    if args.check:
        check_rows(rows)
        check_full_rows(full_rows)
    print(f"wrote {args.tsv}: {len(rows)} rows")
    print(f"wrote {args.json}: {len(rows)} rows")
    print(f"wrote {args.renames}: {len(rename_rows)} dry-run renames")
    print(f"wrote {args.full_tsv}: {len(full_rows)} registry candidate rows")
    print(f"wrote {args.full_json}: {len(full_rows)} registry candidate rows")
    return 0


if __name__ == "__main__":
    sys.exit(main())
