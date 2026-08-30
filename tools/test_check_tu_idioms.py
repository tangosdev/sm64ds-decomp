"""Regression tests for tools/check_tu_idioms.py.

The census scores the axis the byte gate cannot see, so its failure modes are
all of the form "a number quietly went wrong": the extern "C" token vanishing
under comment stripping (the whole pilot census once read externC=0 from
exactly that), a hex literal's tail reading as a member base, a #define
gluing itself into the next function head, a claim in the manifest the code
no longer backs. Each test here plants one of those breaks and asserts the
census goes red or the number moves -- including the empty work list, which
this tree has repeatedly shipped as a silent pass.
"""
import json
import pathlib
import shutil
import subprocess
import sys
import tempfile
import unittest
from collections import Counter

TOOLS = pathlib.Path(__file__).resolve().parent
REPO = TOOLS.parent
sys.path.insert(0, str(TOOLS))

import check_tu_idioms as I  # noqa: E402
import tu_manifest  # noqa: E402  (the manifest reader under test's default)

# The two banked pilot TUs -- the calibration contract lives against these.
BSC_ID = "ov006/dScMgBSC_c+dScMgBSC_c"
MCARLO_ID = "ov006/dMgMCarloCardObj_c+dScMgMCarlo_c+dScMgMCarlo_c"
# A landed real-class TU, for the coverage contrast the pilots are measured
# against (member-majority, named access beating raw offsets).
POLELIFT_ID = "ov045/PoleLift"


def census_of(text, filename="t.cpp"):
    """Run the full classification pipeline over bare source text."""
    is_c = filename.endswith(".c")
    stripped = I.prepare(text)
    frames, segments = I.scan_frames(stripped)
    decls = I.collect_declarations(stripped, segments, is_c)
    extern_c = {d.name for d in decls if d.linkage == "c"}
    funcs = I.census_functions(stripped, frames, extern_c, is_c)
    return funcs, decls


def kinds_of(text, filename="t.cpp"):
    return Counter(f["kind"] for f in census_of(text, filename)[0])


def sites_of(text, filename="t.cpp"):
    sites = Counter()
    for f in census_of(text, filename)[0]:
        sites.update(off for off, _line in f["sites"])
    return sites


def banked_entry(tu_id):
    for e in tu_manifest.load(I.DEFAULT_MANIFEST)["entries"]:
        if e["id"] == tu_id:
            return e
    raise AssertionError(f"{tu_id} is not in the banked manifest")


class Scratch:
    """A throwaway census tree INSIDE the repo (build/ is gitignored).

    The tool resolves sources, headers and the manifest against --root, so
    the scratch dir gets its own src_tu/, include/ and config/tu_manifest.d/
    and the banked manifest is never read for writing and never modified.
    """

    def __enter__(self):
        (REPO / "build").mkdir(exist_ok=True)
        self.dir = pathlib.Path(tempfile.mkdtemp(prefix="check_tu_idioms_",
                                                 dir=REPO / "build"))
        (self.dir / "config" / "tu_manifest.d").mkdir(parents=True)
        return self

    def __exit__(self, *exc):
        shutil.rmtree(self.dir, ignore_errors=True)

    def src(self, name, body):
        (self.dir / "src_tu").mkdir(exist_ok=True)
        (self.dir / "src_tu" / name).write_text(body, encoding="utf-8")
        return self

    def header(self, name, body):
        (self.dir / "include").mkdir(exist_ok=True)
        (self.dir / "include" / (name + ".h")).write_text(body, encoding="utf-8")
        return self

    def entry(self, tu_id, source, **extra):
        entry = {"id": tu_id, "source": source}
        entry.update(extra)
        path = (self.dir / "config" / "tu_manifest.d"
                / (tu_id.replace("/", "__") + ".json"))
        path.write_text(json.dumps(entry, indent=2) + "\n", encoding="utf-8")
        return entry

    def run(self, *args):
        rel = self.dir.relative_to(REPO).as_posix()
        return subprocess.run(
            [sys.executable, str(TOOLS / "check_tu_idioms.py"), "--root", rel, *args],
            capture_output=True, text=True, cwd=REPO)


class LinkageTests(unittest.TestCase):
    def test_extern_c_block_definitions_count_as_c(self):
        """strip_comments_and_strings blanks "C" like any string literal; if
        prepare()'s placeholder swap regresses, these bodies silently stop
        being C linkage and the pilot census reads externC=0 -- the invisible
        failure shape this tool exists to end."""
        text = ('extern "C" {\n'
                "void stepA(int *c) { *(int *)(c + 0x10) = 1; }\n"
                "void stepB(int *c) { *(int *)(c + 0x20) = 2; }\n"
                "}\n")
        self.assertEqual(kinds_of(text), Counter({"externC": 2}))

    def test_extern_c_prefixed_definition_counts_as_c(self):
        text = 'extern "C" int stepC(int *c) { return *(int *)(c + 0x30); }\n'
        self.assertEqual(kinds_of(text), Counter({"externC": 1}))

    def test_a_plain_definition_inherits_c_linkage_from_its_declaration(self):
        """A name declared extern "C" keeps C linkage at its later plain
        definition -- BSC's `int func_ov006_02125248(void *thisPtr)` shape."""
        text = ('extern "C" void stepD(int *c);\n'
                "void stepD(int *c) { *(int *)(c + 0x40) = 3; }\n"
                "void stepE(int x) { }\n")
        self.assertEqual(kinds_of(text), Counter({"externC": 1, "free": 1}))

    def test_c_file_functions_have_c_linkage_by_language(self):
        text = ("int stepF(int *c) { return *(int *)(c + 0x50); }\n"
                "static int stepG(int x) { return x; }\n")
        self.assertEqual(kinds_of(text, "t.c"), Counter({"externC": 1, "free": 1}))

    def test_class_inline_and_out_of_line_definitions_count_as_members(self):
        text = ("class Wrap_c {\n"
                "public:\n"
                "    Wrap_c() { mCount = 1; }\n"
                "    int stepH(int x);\n"
                "    int mCount;\n"
                "};\n"
                "int Wrap_c::stepH(int x) { return x + mCount; }\n")
        self.assertEqual(kinds_of(text), Counter({"member": 2}))

    def test_control_flow_braces_are_not_function_definitions(self):
        """C has no nested functions: control-flow braces and labels (BSC
        spells one `do1:`) must not inflate the function count or re-count
        the raw sites inside their bodies."""
        text = ("void stepI(int *c) {\n"
                "    int acc = 0;\n"
                "    for (int i = 0; i < 2; i++) {\n"
                "        if (c[0]) { acc += *(int *)(c + 0x60); }\n"
                "        do { acc += 2; } while (0);\n"
                "        switch (acc) { case 1: acc = 0; break; default: break; }\n"
                "    }\n"
                "do1:\n"
                "    if (acc) { acc = *(int *)(c + 0x70); }\n"
                "}\n")
        funcs, _ = census_of(text)
        self.assertEqual([f["name"] for f in funcs], ["stepI"])
        self.assertEqual(sites_of(text), Counter({0x60: 1, 0x70: 1}))

    def test_a_define_directive_does_not_glue_into_the_next_head(self):
        """FlyGuy's `#define M(p) (p)`: before preprocessor lines were dropped,
        the directive glued into the next function's head, the body classified
        as flow, and its statements -- including this extern declaration --
        leaked to file scope, where check 3 would read them as declarations."""
        text = ("#define M(p) (p)\n"
                "int stepJ(int *c) {\n"
                "    extern void helper(int *c);\n"
                "    helper(c + 0x80);\n"
                "    return 0;\n"
                "}\n")
        funcs, decls = census_of(text)
        self.assertEqual([f["name"] for f in funcs], ["stepJ"])
        self.assertNotIn("helper", [d.name for d in decls])
        self.assertEqual(sites_of(text), Counter({0x80: 1}))

    def test_enum_bodies_are_not_functions(self):
        text = "enum State { ST_A, ST_B };\nint stepK(void) { return ST_A; }\n"
        self.assertEqual(kinds_of(text), Counter({"free": 1}))


class RawOffsetTests(unittest.TestCase):
    def test_chains_fold_and_hex_indexing_counts(self):
        """`c + 0x5000 + 0x1b8` reports 0x51b8 -- the offset the code MEANS
        -- and a compound expression base counts through its closing paren."""
        text = ("int patterns1(int *c, char *p) {\n"
                "    int a = *(int *)(c + 0x51b8);\n"
                "    int b = *(int *)(c + 0x5000 + 0x1b8);\n"
                "    int d = p[0x2c];\n"
                "    int e = *(int *)((char *)c + 0x51ca);\n"
                "    return a + b + d + e;\n"
                "}\n")
        self.assertEqual(sites_of(text),
                         Counter({0x51b8: 2, 0x2c: 1, 0x51ca: 1}))

    def test_fixed_point_rounding_idioms_are_not_member_access(self):
        """`s * 0x32000 + 0x800` -- the rounding idiom on ~30 sites in this
        tree -- once matched the chain scanner at the `x32000` tail of the
        hex literal and counted a member at 0x800. A member base has to be a
        whole token."""
        text = ("int patterns2(int s) {\n"
                "    int up = (int)(((long long)s * 0x32000 + 0x800) >> 12);\n"
                "    int dn = (int)(((long long)s * -0x32000 + 0x800) >> 12);\n"
                "    return up + dn;\n"
                "}\n")
        self.assertEqual(sites_of(text), Counter())

    def test_what_the_census_deliberately_does_not_count(self):
        """Documented undercounts: MMIO literals, decimal indexing, plain
        dereferences, subtraction. None of these can turn red into green."""
        text = ("int patterns3(int *c, char *p) {\n"
                "    *(volatile unsigned *)0x4000008 = 1;\n"
                "    int a = 0x10 + 0x20;\n"
                "    int b = p[0];\n"
                "    *(int *)p = 7;\n"
                "    *(int *)(c - 0x8) = 0;\n"
                "    return a + b;\n"
                "}\n")
        self.assertEqual(sites_of(text), Counter())

    def test_brackets_outside_function_bodies_do_not_count(self):
        """Only bodies are scanned, so class-member arrays and extern array
        declarations never count -- BSC's 94 whole-file sites minus its 5
        class-member brackets is the 89 the census reports."""
        text = ("extern int data_decl[0x100];\n"
                "class Hold_c {\n"
                "public:\n"
                "    u8 pad_4f38[0x1a];\n"
                "};\n"
                "int onlyBodies(int *c) { return *(int *)(c + 0x90); }\n")
        self.assertEqual(sites_of(text), Counter({0x90: 1}))


class DeclarationTests(unittest.TestCase):
    def test_extern_declarations_collect_with_their_linkage(self):
        text = ('extern "C" void cProto(int a, int b);\n'
                "void cppProto(int x);\n"
                "static void staticProto(int y);\n"
                "typedef int MyInt;\n"
                "class Decl_c {\n"
                "public:\n"
                "    void methodProto(int z);\n"
                "    int mField;\n"
                "};\n"
                "void (*fnPtr)(void);\n"
                "extern int table_x[8];\n"
                "int withInit = GetGameLanguage();\n")
        _, decls = census_of(text)
        by = {d.name: d for d in decls}
        # Statics, typedefs, class members, function pointers and initializer
        # statements are all outside the declaration census.
        self.assertEqual(set(by), {"cProto", "cppProto", "table_x"})
        self.assertEqual(by["cProto"].linkage, "c")
        self.assertEqual(by["cppProto"].linkage, "cpp")
        self.assertTrue(by["table_x"].is_data)

    def test_a_c_file_declares_everything_with_c_linkage(self):
        _, decls = census_of("int cProto(int a);\n", "t.c")
        self.assertEqual([d.linkage for d in decls], ["c"])

    def test_an_initializer_statement_is_not_a_declaration(self):
        """The sinit shape (`int i = GetGameLanguage();`) is a call in
        initializer position; collecting it put call-shaped text into the
        cross-TU signature comparison."""
        _, decls = census_of("int withInit = GetGameLanguage();\n")
        self.assertEqual([d.name for d in decls], [])


class SignatureTests(unittest.TestCase):
    def test_parameter_names_do_not_split_spellings(self):
        self.assertEqual(I.normalize_params("int *x, int y"),
                         I.normalize_params("int *t, int u"))

    def test_reference_and_pointer_do_split(self):
        """The live drift this check exists for: _Z14ApproachLinearRiii is
        `int&` in one TU and `int *` in another, byte-identical either way."""
        self.assertNotEqual(I.normalize_params("int& v, int a, int b"),
                            I.normalize_params("int *x, int a, int b"))

    def test_type_word_suffixes_stay_whole(self):
        self.assertEqual(I.normalize_params("unsigned int n"),
                         I.normalize_params("unsigned int"))

    def test_groups_report_only_normalized_splits(self):
        def decl(params_text, norm):
            return I.Decl("F", norm, "void", "c",
                          f'extern "C" void F({params_text});', 1, False)

        recs = [
            {"id": "m/x", "_decls": [decl("int *x, int y", I.normalize_params("int *x, int y"))]},
            {"id": "m/y", "_decls": [decl("int *t, int u", I.normalize_params("int *t, int u"))]},
            {"id": "m/z", "_decls": [decl("int& v", I.normalize_params("int& v"))]},
        ]
        groups = I.signature_groups(recs)
        self.assertEqual(list(groups), ["F"])
        # `int *x, int y` and `int *t, int u` fold to ONE variant; the
        # reference spelling is the only split.
        self.assertEqual(len(groups["F"]), 2)
        report = I.report_signature_groups(groups)
        self.assertEqual(len(report), 1)
        self.assertEqual(len(report[0]["variants"]), 2)

    def test_a_split_on_an_unratified_symbol_still_never_fails(self):
        """D3 ratified five spellings, not the whole symbol space: a split on
        any OTHER symbol stays report-only truth even in gated mode -- the
        family has not spoken on F, and this tool does not speak first."""
        with Scratch() as s:
            s.src("a.cpp", 'extern "C" void F(int *x, int y);\nvoid aFn(void) { }\n')
            s.src("b.cpp", 'extern "C" void F(int& v, int a, int b);\nvoid bFn(void) { }\n')
            s.entry("m/a", "src_tu/a.cpp")
            s.entry("m/b", "src_tu/b.cpp")
            r = s.run("m/a", "m/b")
            self.assertEqual(r.returncode, 0, r.stdout + r.stderr)
            self.assertIn("F ", r.stdout)


class RatifiedGateTests(unittest.TestCase):
    """Check 3's gated half: the adjudicated D3 spellings
    (notes/minigame-family-decisions.md, 2026-08-30) are enforced, not just
    reported. Every fixture here declares with a genuinely zero shadow
    census so a failure can only come from the signature gate -- these
    tests prove the new gate fires independently of check 4."""

    def test_the_ratified_spelling_passes_gated(self):
        with Scratch() as s:
            s.src("a.cpp", 'extern "C" int RandomIntInternal(int *seed);\n'
                           "void aFn(void) { }\n")
            s.entry("m/a", "src_tu/a.cpp")
            r = s.run("m/a")
            self.assertEqual(r.returncode, 0, r.stdout + r.stderr)

    def test_a_wrong_return_type_fails_gated(self):
        """C linkage discards the return type too -- `void RandomIntInternal`
        links byte-true against a ratified `int`. The (return, params) key is
        what makes the gate catch this; the old params-only compare could
        not."""
        with Scratch() as s:
            s.src("a.cpp", 'extern "C" void RandomIntInternal(int *seed);\n'
                           "void aFn(void) { }\n")
            s.entry("m/a", "src_tu/a.cpp")
            r = s.run("m/a")
            self.assertEqual(r.returncode, 1)
            self.assertIn("ratified spelling: int RandomIntInternal(int*)", r.stdout)

    def test_a_retired_mangled_name_fails_gated_even_without_a_split(self):
        """The heart of the retired-name rule: one TU, one spelling, no split
        -- the signature groups would never show it, but the ratified form
        for this family is the true-name declaration, so the mangled name
        is wrong regardless."""
        with Scratch() as s:
            s.src("a.cpp", 'extern "C" int _Z14ApproachLinearRiii(int *t, int a, int b);\n'
                           "void aFn(void) { }\n")
            s.entry("m/a", "src_tu/a.cpp")
            r = s.run("m/a")
            self.assertEqual(r.returncode, 1)
            self.assertIn("retired mangled-name identifier _Z14ApproachLinearRiii",
                          r.stdout)
            self.assertIn("ratified form: int ApproachLinear(int&, int, int)",
                          r.stdout)

    def test_a_retired_name_in_cpp_linkage_also_fails(self):
        """The phantom double-mangling hazard, measured on this tree: a bare
        C++ declaration of the mangled identifier emits _Z22_Z14ApproachLinearRiiiPiii
        -- invisible to text-verify, fatal at link. Any linkage fails."""
        with Scratch() as s:
            s.src("a.cpp", "int _Z14ApproachLinearRsss(s16 *t, s16 a, s16 b);\n"
                           "void aFn(void) { }\n")
            s.entry("m/a", "src_tu/a.cpp")
            r = s.run("m/a")
            self.assertEqual(r.returncode, 1)
            self.assertIn("_Z14ApproachLinearRsss", r.stdout)

    def test_retired_names_are_visible_in_report_mode(self):
        """Census mode stays report-only, but a retired name with no split
        never reaches the signature groups -- if the report did not surface
        it separately it would be invisible until someone gates."""
        with Scratch() as s:
            s.src("a.cpp", 'extern "C" int _Z15ApproachLinear2Riii(int *t, int a, int b);\n'
                           "void aFn(void) { }\n")
            s.entry("m/a", "src_tu/a.cpp")
            r = s.run()
            self.assertEqual(r.returncode, 0, r.stdout + r.stderr)
            self.assertIn("retired mangled-name declarations", r.stdout)
            self.assertIn("_Z15ApproachLinear2Riii", r.stdout)
            self.assertIn("m/a", r.stdout)


COV_BASE_H = ("class CovBase_c {\n"
              "public:\n"
              "    int mHudScore;           /* 0x4 */\n"
              "    void baseTick();\n"
              "};\n")
COV_A_H = ('#include "CovBase_c.h"\n'
           "class CovA_c : public CovBase_c {\n"
           "public:\n"
           "    void tick();\n"
           "    int mState;              /* 0x20 */\n"
           "    short mTimer;            /* 0x24 */\n"
           "    u8 unk_28[0x4];\n"
           "};\n")
COV_A_SRC = ('#include "CovA_c.h"\n'
             'extern "C" void covA_step(void *selfPtr) {\n'
             "    CovA_c *self = (CovA_c *)selfPtr;\n"
             "    self->mState = 1;\n"
             "    self->mTimer = 2;\n"
             "    self->mHudScore = 3;\n"
             "    self->tick();\n"
             "    *(int *)((char *)self + 0x20) = 4;\n"
             "    *(short *)((char *)self + 0x24) = 5;\n"
             "}\n")


class CoverageTests(unittest.TestCase):
    def test_named_access_raw_sites_and_the_worklist(self):
        with Scratch() as s:
            s.header("CovBase_c", COV_BASE_H).header("CovA_c", COV_A_H)
            s.src("CovA_c.cpp", COV_A_SRC)
            entry = s.entry("m/CovA_c", "src_tu/CovA_c.cpp")
            rec = I.census_tu(s.dir, entry)
        cov = rec["coverage"]
        self.assertEqual(cov["header"], "CovA_c")
        # Four named hits: mState, mTimer, mHudScore (in the BASE header, via
        # the include chain) and the tick() method call.
        self.assertEqual(cov["named"], 4)
        self.assertEqual(cov["raw"], 2)
        self.assertEqual(cov["fraction"], round(4 / 6, 3))
        # The worklist is the conversion backlog: each raw offset tagged with
        # the header member that lives there.
        self.assertEqual([(w["offset"], w["member"], w["count"])
                          for w in cov["worklist"]],
                         [("0x20", "mState", 1), ("0x24", "mTimer", 1)])

    def test_bare_method_names_and_spaced_access_do_not_inflate(self):
        with Scratch() as s:
            s.header("CovBase_c", COV_BASE_H).header("CovA_c", COV_A_H)
            src = ('#include "CovA_c.h"\n'
                   "void covB_step(CovA_c &r) {\n"
                   "    tick();\n"
                   "    r. mState = 1;\n"
                   "}\n")
            # The header is found by the source STEM, so the file is named
            # for the class even though this TU is a different entry.
            s.src("CovA_c.cpp", src)
            entry = s.entry("m/CovB_c", "src_tu/CovA_c.cpp")
            rec = I.census_tu(s.dir, entry)
        # `tick()` bare is a method with no offset, not a field access; the
        # spaced `r. mState` counts exactly once (the dot hit; the bare hit
        # after the space is suppressed).
        self.assertEqual(rec["coverage"]["named"], 1)
        self.assertEqual(rec["freeDefs"], 1)

    def test_a_class_without_a_header_reports_no_header(self):
        """Honesty over a false zero: a TU whose class has no header reports
        no-header rather than a coverage fraction that would read converted."""
        with Scratch() as s:
            s.src("Shadow.cpp",
                  'extern "C" void sh_step(int *c) { *(int *)(c + 0x8) = 1; }\n')
            entry = s.entry("m/Shadow", "src_tu/Shadow.cpp")
            rec = I.census_tu(s.dir, entry)
        self.assertIsNone(rec["coverage"]["header"])
        self.assertNotIn("named", rec["coverage"])
        self.assertEqual(rec["rawOffsets"], 1)


def goal_entry(externC, rawOffsets, justification=None):
    goal = {"idiomCensus": {"externC": externC, "rawOffsets": rawOffsets}}
    if justification is not None:
        goal["justification"] = justification
    return goal


def census_rec(externC, rawOffsets):
    return {"externC": externC, "rawOffsets": rawOffsets}


class GoalGateTests(unittest.TestCase):
    def test_nonzero_without_a_goal_block_fails_as_unjustified(self):
        v = I.gate_goal({}, census_rec(14, 89))
        self.assertEqual(v["verdict"], "fail")
        self.assertIn("unjustified", v["reason"])

    def test_matching_claim_with_justification_passes(self):
        entry = {"goal": goal_entry(14, 89, "banked pilot, conversion tracked")}
        v = I.gate_goal(entry, census_rec(14, 89))
        self.assertEqual(v["verdict"], "pass")
        self.assertIn("justification", v["reason"])
        self.assertEqual(v["justification"], "banked pilot, conversion tracked")

    def test_a_stale_claim_fails(self):
        """The vtable-slot-17 class of error: prose the code does not back."""
        entry = {"goal": goal_entry(12, 89, "numbers from an older census")}
        v = I.gate_goal(entry, census_rec(14, 89))
        self.assertEqual(v["verdict"], "fail")
        self.assertIn("stale claim", v["reason"])

    def test_matching_numbers_without_justification_still_fail(self):
        entry = {"goal": goal_entry(14, 89)}
        v = I.gate_goal(entry, census_rec(14, 89))
        self.assertEqual(v["verdict"], "fail")

    def test_a_claim_missing_a_number_fails(self):
        entry = {"goal": {"idiomCensus": {"externC": 14}, "justification": "x"}}
        v = I.gate_goal(entry, census_rec(14, 89))
        self.assertEqual(v["verdict"], "fail")
        self.assertIn("rawOffsets", v["reason"])

    def test_a_non_integer_claim_fails(self):
        entry = {"goal": goal_entry("14", 89, "x")}
        v = I.gate_goal(entry, census_rec(14, 89))
        self.assertEqual(v["verdict"], "fail")

    def test_a_claim_that_is_not_an_object_fails(self):
        entry = {"goal": {"idiomCensus": [14, 89], "justification": "x"}}
        v = I.gate_goal(entry, census_rec(14, 89))
        self.assertEqual(v["verdict"], "fail")

    def test_genuinely_zero_passes_without_a_goal_block(self):
        v = I.gate_goal({}, census_rec(0, 0))
        self.assertEqual(v["verdict"], "pass")
        self.assertEqual(v["reason"], "genuinely zero census")


SHADOW_SRC = ('extern "C" void pilot_step(int *c) '
              '{ *(int *)(c + 0x51b8) = 1; }\n')
CLEAN_SRC = ("class Clean_c {\n"
             "public:\n"
             "    void tick();\n"
             "};\n"
             "void Clean_c::tick() { }\n")


class CliGoalTests(unittest.TestCase):
    """The gate end to end on a scratch tree -- the --strict behavior the
    plan note documents, exercised without ever touching a banked entry."""

    def test_nonzero_census_without_justification_exits_one(self):
        with Scratch() as s:
            s.src("pilot.cpp", SHADOW_SRC)
            s.entry("m/pilot", "src_tu/pilot.cpp")
            r = s.run("m/pilot")
            self.assertEqual(r.returncode, 1)
            self.assertIn("unjustified", r.stdout)

    def test_matching_claim_with_justification_exits_zero_and_echoes_it(self):
        with Scratch() as s:
            s.src("pilot.cpp", SHADOW_SRC)
            s.entry("m/pilot", "src_tu/pilot.cpp",
                    goal={"idiomCensus": {"externC": 1, "rawOffsets": 1},
                          "justification": "banked pilot; conversion is tracked"})
            r = s.run("m/pilot")
            self.assertEqual(r.returncode, 0, r.stdout + r.stderr)
            self.assertIn("goal PASS", r.stdout)
            self.assertIn("banked pilot; conversion is tracked", r.stdout)

    def test_a_stale_claim_exits_one(self):
        with Scratch() as s:
            s.src("pilot.cpp", SHADOW_SRC)
            s.entry("m/pilot", "src_tu/pilot.cpp",
                    goal={"idiomCensus": {"externC": 0, "rawOffsets": 0},
                          "justification": "claimed converted"})
            r = s.run("m/pilot")
            self.assertEqual(r.returncode, 1)
            self.assertIn("stale claim", r.stdout)

    def test_strict_gates_every_entry_and_a_converted_tu_passes_it(self):
        with Scratch() as s:
            s.src("pilot.cpp", SHADOW_SRC)
            s.src("clean.cpp", CLEAN_SRC)
            s.entry("m/pilot", "src_tu/pilot.cpp")
            s.entry("m/clean", "src_tu/clean.cpp")
            r = s.run("--strict")
            self.assertEqual(r.returncode, 1)
            self.assertIn("goal FAIL m/pilot", r.stdout)
            self.assertIn("goal PASS m/clean", r.stdout)
            self.assertIn("genuinely zero", r.stdout)


class EmptyWorkListTests(unittest.TestCase):
    """An empty check is not a pass (tools/check_src_tu.py argues this; this
    tree has shipped the silent version)."""

    def test_a_manifest_with_no_entries_fails(self):
        with Scratch() as s:
            r = s.run()
            self.assertEqual(r.returncode, 1)
            self.assertIn("no manifest entries", r.stdout)

    def test_a_missing_source_fails_the_gate(self):
        with Scratch() as s:
            s.entry("m/gone", "src_tu/gone.cpp")
            r = s.run("m/gone")
            self.assertEqual(r.returncode, 1)
            self.assertIn("does not exist", r.stdout)

    def test_an_unknown_id_fails_the_gate(self):
        with Scratch() as s:
            s.src("there.cpp", "void f(void) { }\n")
            s.entry("m/there", "src_tu/there.cpp")
            r = s.run("m/notthere")
            self.assertEqual(r.returncode, 1)
            self.assertIn("unknown TU id", r.stdout)

    def test_a_gated_source_with_no_functions_fails(self):
        with Scratch() as s:
            s.src("empty.cpp", "int one = 1;\n")
            s.entry("m/empty", "src_tu/empty.cpp")
            r = s.run("m/empty")
            self.assertEqual(r.returncode, 1)
            self.assertIn("no function definitions", r.stdout)

    def test_a_tree_of_zero_functions_fails_even_in_census_mode(self):
        with Scratch() as s:
            s.src("empty.cpp", "int one = 1;\n")
            s.entry("m/empty", "src_tu/empty.cpp")
            r = s.run()
            self.assertEqual(r.returncode, 1)
            self.assertIn("no function definitions in the whole tree", r.stdout)


class TreeTests(unittest.TestCase):
    """The committed tree is the calibration fixture.

    If the pilot pins go red, the SCANNER regressed (each number below was
    hand-counted when the tool landed) -- fix the scanner, not the test. If
    the tree moved on -- a pilot converted or justified -- update the pins;
    that is the tool doing its job.
    """

    def test_census_mode_exits_zero_on_the_committed_tree(self):
        r = subprocess.run([sys.executable, str(TOOLS / "check_tu_idioms.py")],
                           capture_output=True, text=True, cwd=REPO)
        self.assertEqual(r.returncode, 0, r.stdout + r.stderr)
        self.assertIn("report only", r.stdout)

    def test_the_bsc_pilot_measures_the_shadow_baseline(self):
        rec = I.census_tu(REPO, banked_entry(BSC_ID))
        # 18 function definitions: 14 extern "C", 4 class members, no C++
        # free functions. 89 raw sites inside bodies: 94 whole-file matches
        # minus the 5 class-member array brackets outside any body.
        self.assertEqual((rec["functions"], rec["memberDefs"], rec["externC"],
                          rec["freeDefs"], rec["rawOffsets"]),
                         (18, 4, 14, 0, 89))
        # Named=13: 9 arrow/dot hits plus 4 bare member names, hand-counted.
        self.assertEqual(rec["coverage"]["named"], 13)

    def test_the_mcarlo_pilot_measures_the_shadow_baseline(self):
        rec = I.census_tu(REPO, banked_entry(MCARLO_ID))
        self.assertEqual((rec["functions"], rec["memberDefs"], rec["externC"],
                          rec["freeDefs"], rec["rawOffsets"]),
                         (22, 4, 18, 0, 108))
        self.assertEqual(rec["coverage"]["named"], 47)

    def test_a_landed_real_class_tu_is_the_coverage_contrast(self):
        """What the goal looks like when it is met: PoleLift is member-
        majority, nearly free of extern "C", and spells more member accesses
        through names than through offsets -- against BSC's 13% coverage."""
        rec = I.census_tu(REPO, banked_entry(POLELIFT_ID))
        self.assertEqual((rec["functions"], rec["memberDefs"], rec["externC"],
                          rec["rawOffsets"]), (6, 5, 1, 13))
        self.assertEqual(rec["coverage"]["named"], 11)
        self.assertGreater(rec["coverage"]["fraction"], 0.4)

    def test_gating_the_banked_pilot_goes_red_until_justified(self):
        """The banked manifest carries no goal blocks yet, so the promotion
        prerequisite bites: gating a pilot fails it. When a pilot is
        converted or a justification lands, this flips and gets updated.
        The banked entry file is READ and wrapped into a scratch single-file
        manifest -- config/ is never written."""
        banked = pathlib.Path(
            "config/tu_manifest.d/ov006/dScMgBSC_c+dScMgBSC_c.json")
        entry = json.loads(banked.read_text(encoding="utf-8"))
        with Scratch() as s:
            scratch_manifest = s.dir / "manifest.json"
            scratch_manifest.write_text(
                json.dumps({"schema_version": 1, "entries": [entry]}) + "\n",
                encoding="utf-8")
            r = subprocess.run(
                [sys.executable, str(TOOLS / "check_tu_idioms.py"),
                 BSC_ID, "--manifest",
                 scratch_manifest.relative_to(REPO).as_posix()],
                capture_output=True, text=True, cwd=REPO)
        self.assertEqual(r.returncode, 1, r.stdout + r.stderr)
        self.assertIn("unjustified", r.stdout)


if __name__ == "__main__":
    unittest.main()