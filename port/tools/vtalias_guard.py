"""vtalias_guard -- a vtable alternatename must be an ADDRESS match, not a name hunch.

WHY THIS EXISTS (run link100, lane CRASH12). port/hal/actor_classes.cpp carried

    /alternatename:  __ZTV12daStarGate_c  ->  __ZTV4Door

with a careful, correct derivation attached: the star door's table at ov100
0x021483cc really does carry the RTTI name "12daStarGate_c". What the
derivation never did was check the OTHER name. _ZTV4Door is ov100 0x02148188,
a different table 0x244 bytes away whose own RTTI reads "8daDoor_c". The two
classes differ in size (0x148 against 0x118) and in what they keep at +0xd4 (a
ModelAnim against a CommonModel), and their tables differ in exactly the seven
slots a subclass overrides. Because the other twenty-four slots are identical,
the alias looked harmless for weeks. It was not: src/d_a_door.c stores
_ZTV4Door into every plain door, hal_fill_star_door_vtable had written the star
door's faces into that array, and the castle grounds died the first time a door
was drawn.

Proving a name for ONE table is not proving that TWO tables are one. The check
that was missing is mechanical and is this file: for every vtable alternatename
in port/, ask config what address each name has, and refuse when the two
answers differ.

ON ITS FIRST RUN IT FOUND FIVE MORE, across three unrelated class pairs, and
lane ALIAS5 settled and fixed all five. Every one had the same shape as the
doors: the join stole the name of the table on the RIGHT and put it on the
array holding the table on the LEFT, so the class spawning under the stolen
name wore the other class's methods while its own filled array was installed on
nothing. The worst of them, ov091's stump, was dispatching a Render that reads
a ModelAnim at +0x300 in an object 0x330 bytes long.

WHAT IT READS. Only real directives. C and C++ comments are stripped before
matching, so a commented-out pragma or a lane note quoting one is not a linker
input -- the same scoping mistake alternatename_guard carries its own fixture
for. Aliases are resolved against every config/arm9/**/symbols.txt.

WHAT IT CANNOT SAY. A name config has never heard of is UNKNOWN, not a pass and
not a failure: many host arrays are port scaffolding with no ROM symbol. Those
are counted and listed under --verbose so the unchecked set stays visible
instead of quietly reading as green.

SELF-VALIDATION, ON EVERY RUN (lane ALIAS5, in lane BSSFIX's shape).
--------------------------------------------------------------------
A gate that has only ever passed proves nothing. Before this guard looks at the
tree it drives its own decision path over twelve built-in fixtures, six that
MUST convict and six that MUST NOT, plus a line-number fixture and the name
undecoration, and refuses the build if any comes back with the wrong verdict. Deleting or weakening a rule then breaks the build
immediately instead of silently disarming the check, which is the failure this
whole file exists to prevent.

THE FIXTURES DRIVE THE REAL FUNCTIONS. As first written the conviction fixture
did its own set intersection inline:

    syms = {"_ZTV12daStarGate_c": {0x021483CC}, "_ZTV4Door": {0x02148188}}
    la, ra = syms["_ZTV12daStarGate_c"], syms["_ZTV4Door"]
    if la & ra: fail

which would still have passed with the entire conviction branch deleted from
check(). That is lane TAILPAD2's control lesson exactly: a test of a check must
break something by more than the thing that would hide it, or a working tool
and a broken one give the same answer. Every fixture now runs scan_text() and
classify(), the same two functions the real run uses, and asserts the counts in
all three buckets, so the reader, the comment stripper, the undecoration and
the address comparison are each load-bearing.

WHAT NO SYNTHETIC FIXTURE CAN COVER is the directory walk itself: a fixture
feeds text in directly, so a walk that stopped visiting .cpp files would leave
every fixture green. So the tree scan also refuses when it finds NO vtable
alternatename at all, which has never been true of this tree.

  python port/tools/vtalias_guard.py <root> [--verbose]
  python port/tools/vtalias_guard.py --selftest
"""
import os
import re
import sys

ALIAS = re.compile(r"/alternatename:\s*(__ZTV[A-Za-z0-9_]+)\s*=\s*(__ZTV[A-Za-z0-9_]+)")
SYMROW = re.compile(r"^\s*(\S+)\s+kind:\S+\s+addr:(0x[0-9a-fA-F]+)")
SRC_EXT = (".cpp", ".c", ".h", ".inc", ".hpp", ".txt")


def strip_comments(text):
    """Remove /* */ and // comments, keeping string literals intact.

    A commented-out directive is not a directive. Done as a single scan rather
    than a regex so a // inside a string literal does not eat the rest of the
    line.

    NEWLINES ARE PRESERVED, one for one. A /* */ block used to collapse to a
    single space, newlines and all, so every line number this guard printed
    after the first block comment in a file was too small: it named
    actor_classes_ov091.cpp:379 for a directive that is on line 600, and
    bowserpuzzle.cpp:154 for one on line 183. A guard that names a line the
    reader cannot find costs the next person the time the guard was meant to
    save, so the block now emits its own newlines back.
    """
    out = []
    i, n = 0, len(text)
    while i < n:
        c = text[i]
        if c == '"':
            j = i + 1
            while j < n:
                if text[j] == "\\":
                    j += 2
                    continue
                if text[j] == '"':
                    j += 1
                    break
                j += 1
            out.append(text[i:j])
            i = j
        elif text.startswith("/*", i):
            j = text.find("*/", i + 2)
            end = n if j < 0 else j + 2
            out.append(" " + "\n" * text.count("\n", i, end))
            i = end
        elif text.startswith("//", i):
            j = text.find("\n", i)
            i = n if j < 0 else j
            out.append(" ")
        else:
            out.append(c)
            i += 1
    return "".join(out)


def load_config_symbols(root):
    """name -> {address, ...} over every config/arm9 symbols.txt.

    A name may legitimately appear in several overlays; the set is kept whole so
    a genuine multi-module name is reported as ambiguous rather than convicted.
    """
    syms = {}
    base = os.path.join(root, "config", "arm9")
    for dirpath, _dirnames, filenames in os.walk(base):
        for fn in filenames:
            if fn != "symbols.txt":
                continue
            path = os.path.join(dirpath, fn)
            with open(path, encoding="utf-8", errors="replace") as f:
                for line in f:
                    m = SYMROW.match(line)
                    if m:
                        syms.setdefault(m.group(1), set()).add(int(m.group(2), 16))
    return syms


def scan_text(relpath, raw):
    """THE READER. [(file, lineno, lhs, rhs)] for one file's text.

    Pure, so the fixtures below drive the same comment stripping, the same
    pattern and the same line arithmetic the tree scan uses.
    """
    found = []
    if "/alternatename:" not in raw:
        return found
    text = strip_comments(raw)
    for m in ALIAS.finditer(text):
        line = text.count("\n", 0, m.start()) + 1
        found.append((relpath, line, m.group(1), m.group(2)))
    return found


def collect_aliases(root):
    """Every vtable alternatename in port/ that is code. I/O around scan_text."""
    found = []
    port = os.path.join(root, "port")
    for dirpath, dirnames, filenames in os.walk(port):
        dirnames[:] = [d for d in dirnames if d not in ("build", "__pycache__")]
        for fn in filenames:
            if not fn.endswith(SRC_EXT):
                continue
            # A .txt is lane prose, never a linker input. Named explicitly so the
            # scoping is a decision in this file and not an accident of the walk.
            if fn.endswith(".txt"):
                continue
            path = os.path.join(dirpath, fn)
            try:
                with open(path, encoding="utf-8", errors="replace") as f:
                    raw = f.read()
            except OSError:
                continue
            found.extend(scan_text(
                os.path.relpath(path, root).replace("\\", "/"), raw))
    return found


def itanium(decorated):
    """__ZTV4Door -> _ZTV4Door. MSVC prefixes a leading underscore on C names."""
    return decorated[1:] if decorated.startswith("__ZTV") else decorated


def classify(aliases, syms):
    """THE DECISION, and the only place it is made.

    An alternatename asserts that two names are one object. config knows what
    address each name has. If both are known and the sets are disjoint, the
    assertion is false and the alias joins two different classes.

    Pure, and every fixture drives it, so a rule weakened here fails a fixture
    rather than quietly passing a defect.
    """
    ok, bad, unknown = [], [], []
    for path, line, lhs, rhs in aliases:
        la, ra = syms.get(itanium(lhs)), syms.get(itanium(rhs))
        if not la or not ra:
            unknown.append((path, line, lhs, rhs, la, ra))
        elif la & ra:
            ok.append((path, line, lhs, rhs, sorted(la & ra)))
        else:
            bad.append((path, line, lhs, rhs, sorted(la), sorted(ra)))
    return ok, bad, unknown


# ------------------------------------------------------------------ fixtures

def fixtures():
    """(label, source text, config symbols, expected ok/bad/unknown, note).

    Synthetic rather than the real defects, because the real defects are fixed
    now and a gate that validates itself against the tree stops validating the
    day the tree is correct. The addresses are the real ones, so a reader can
    check them against config, but the source text is invented.
    """
    DOOR = {"_ZTV4Door": {0x02148188}, "_ZTV12daStarGate_c": {0x021483CC},
            "_ZTV8daDoor_c": {0x02148188}}
    OV064 = {"_ZTV17BowserPuzzlePiece": {0x0211C25C},
             "_ZTV16daObjFl_Puzzle_c": {0x0211C25C},
             "_ZTV9JetStream": {0x0211C334},
             "_ZTV18daWater_Hakidasi_c": {0x0211C334}}
    OV030 = {"_ZTV7daMky_c": {0x02115BFC}, "_ZTV13RollingLogTtm": {0x02115A48}}
    OV091 = {"_ZTV5Stump": {0x021352BC}, "_ZTV6Fwoosh": {0x021353AC},
             "_ZTV10daHyuhyu_c": {0x021353AC}}
    P = '#pragma comment(linker, "/alternatename:%s=%s")\n'

    return [
        ("the star door pair convicts",
         P % ("__ZTV12daStarGate_c", "__ZTV4Door"), DOOR, (0, 1, 0),
         "the twelfth defect: two tables 0x244 apart, joined as one"),

        ("the plain door's true alias passes",
         P % ("__ZTV8daDoor_c", "__ZTV4Door"), DOOR, (1, 0, 0),
         "0x02148188 under both its names, which is what a real alias is"),

        ("ov064 puzzle piece onto JetStream convicts",
         P % ("__ZTV9JetStream", "__ZTV17BowserPuzzlePiece"), OV064, (0, 1, 0),
         "the thirteenth defect's first pair, 0x0211c334 against 0x0211c25c"),

        ("ov064 both true aliases pass",
         (P % ("__ZTV18daWater_Hakidasi_c", "__ZTV9JetStream"))
         + (P % ("__ZTV17BowserPuzzlePiece", "__ZTV16daObjFl_Puzzle_c")),
         OV064, (2, 0, 0),
         "each address under both of its own names, the shape ALIAS5 left behind"),

        ("ov030 monkey onto rolling log convicts",
         P % ("__ZTV7daMky_c", "__ZTV13RollingLogTtm"), OV030, (0, 1, 0),
         "0x02115bfc against 0x02115a48, ten of thirty-one slots apart"),

        ("ov091 Fwoosh onto Stump convicts",
         P % ("__ZTV6Fwoosh", "__ZTV5Stump"), OV091, (0, 1, 0),
         "0x021353ac against 0x021352bc, twelve of thirty-one slots apart"),

        ("ov091 daHyuhyu onto Stump convicts",
         P % ("__ZTV10daHyuhyu_c", "__ZTV5Stump"), OV091, (0, 1, 0),
         "the same wrong table reached under its other name"),

        ("a commented-out directive stays quiet",
         "/* " + (P % ("__ZTV12daStarGate_c", "__ZTV4Door")) + " */\n"
         + "// " + (P % ("__ZTV6Fwoosh", "__ZTV5Stump")),
         DOOR, (0, 0, 0),
         "a retired row and a lane note quoting one are not linker inputs"),

        ("a directive quoted inside a // note stays quiet",
         '  ok // "not a comment inside a string" '
         '/alternatename:__ZTV6Fwoosh=__ZTV5Stump',
         OV091, (0, 0, 0),
         "// wins from where it starts; the quote inside it must not reopen the line"),

        ("a directive inside a pragma's string literal IS read",
         P % ("__ZTV6Fwoosh", "__ZTV5Stump"), OV091, (0, 1, 0),
         "load-bearing: EVERY real directive lives inside a string literal, so "
         "the stripper must keep literals whole and the pattern must match inside "
         "them. A first draft of the fixture above asserted the opposite, and this "
         "self-check refused it, which is the whole point of running fixtures"),

        ("a name config never heard of is UNKNOWN, not a pass",
         P % ("__ZTV9SomeHostArray", "__ZTV4Door"), DOOR, (0, 0, 1),
         "port scaffolding has no ROM symbol; unchecked must not read as green"),

        ("a name in two overlays is ambiguous, not convicted",
         P % ("__ZTV5Shared", "__ZTV4Door"),
         {"_ZTV5Shared": {0x02148188, 0x02999999}, "_ZTV4Door": {0x02148188}},
         (1, 0, 0),
         "one address in common is enough; a genuine multi-module name passes"),
    ]


def line_fixture():
    """The line number must be the line in the FILE, not in the stripped text.

    This is a fixture because it is a bug that already happened: block comments
    used to collapse to one space, so the guard named line 379 for a directive
    on line 600 and 154 for one on line 183.
    """
    text = ("/* a block comment\n   spanning\n   four\n   lines */\n"
            "// a line comment\n"
            '#pragma comment(linker, "/alternatename:__ZTV4Door=__ZTV8daDoor_c")\n')
    got = scan_text("fixture.cpp", text)
    return len(got) == 1 and got[0][1] == 6, (got[0][1] if got else None)


def self_check(verbose=False):
    """Drive the real reader and the real decision over every fixture."""
    ok_all = True

    good, got = line_fixture()
    ok_all = ok_all and good
    if verbose:
        print("    [%s] %-58s %s"
              % ("PASS" if good else "FAIL",
                 "a directive's line number is its line in the file",
                 "line 6 as required" if good else "line %s, wanted 6" % got))
        print("           the bug that already happened: a block comment used to")
        print("           collapse to one space and every later line number was low")

    if itanium("__ZTV4Door") != "_ZTV4Door" or itanium("_ZTV4Door") != "_ZTV4Door":
        ok_all = False
        if verbose:
            print("    [FAIL] MSVC's leading underscore is not undecorated")

    for label, text, syms, want, note in fixtures():
        aliases = scan_text("fixture.cpp", text)
        ok, bad, unknown = classify(aliases, syms)
        got = (len(ok), len(bad), len(unknown))
        good = (got == want)
        ok_all = ok_all and good
        if verbose:
            print("    [%s] %-58s %s"
                  % ("PASS" if good else "FAIL", label,
                     "ok/bad/unknown %d/%d/%d" % got
                     + ("" if good else ", wanted %d/%d/%d" % want)))
            print("           %s" % note)
    return ok_all


# ---------------------------------------------------------------------- main

def check(root, verbose=False):
    syms = load_config_symbols(root)
    aliases = collect_aliases(root)

    if not aliases:
        print("")
        print("REFUSING THE BUILD: the scan found NO vtable alternatename anywhere")
        print("in port/, which has never been true of this tree. No synthetic")
        print("fixture can catch this, because a fixture is handed its text: a walk")
        print("that stopped visiting .cpp files would leave every fixture green and")
        print("this whole guard silently disarmed. Check the walk in")
        print("collect_aliases() and the extension list before anything else.")
        return 1

    ok, bad, unknown = classify(aliases, syms)

    print("vtalias_guard: %d vtable alternatename(s) in port/ code, %d address-checked, "
          "%d not in config" % (len(aliases), len(ok) + len(bad), len(unknown)))
    if verbose:
        for path, line, lhs, rhs, la, ra in unknown:
            print("  unchecked  %-52s %s:%d" % (lhs + " = " + rhs, path, line))
            print("             %s %s   %s %s"
                  % (lhs, "0x%08x" % min(la) if la else "(not in config)",
                     rhs, "0x%08x" % min(ra) if ra else "(not in config)"))
        for path, line, lhs, rhs, hit in ok:
            print("  ok         %-52s %s  %s:%d"
                  % (lhs + " = " + rhs, " ".join("0x%08x" % a for a in hit), path, line))
    if bad:
        print("")
        print("vtalias_guard: FAIL -- %d vtable alias(es) join names config gives"
              " DIFFERENT addresses." % len(bad))
        print("An alternatename says two names are one object. Two addresses say they")
        print("are two objects, and whichever fill runs last then owns both classes.")
        for path, line, lhs, rhs, la, ra in bad:
            print("  %s:%d" % (path, line))
            print("    %-28s %s" % (lhs, " ".join("0x%08x" % a for a in la)))
            print("    %-28s %s" % (rhs, " ".join("0x%08x" % a for a in ra)))
        print("Fix: host each table under its own name and give each class its own")
        print("fill, the way gate 22 and gate 40 do in hal/actor_classes.cpp and the")
        print("three pairs lane ALIAS5 settled do in hal/actor_classes_ov030.cpp,")
        print("hal/actor_classes_ov091.cpp and hal/actor_classes_bowserpuzzle.cpp.")
        print("Settle which class owns which table FROM THE CARTRIDGE first: read")
        print("each table's typeinfo name string, then disassemble the two classInit")
        print("factories and read the allocation size and the literal each stamps.")
        return 1
    print("vtalias_guard: OK -- every address-checked vtable alias joins one address")
    return 0


def main(argv):
    verbose = "--verbose" in argv
    selftest_only = "--selftest" in argv

    if selftest_only:
        print("vtalias_guard: rule-engine self-check only")
        if self_check(verbose=True):
            print("vtalias_guard: SELFTEST OK -- reads live directives, ignores")
            print("commented and quoted ones, reports true line numbers, convicts a")
            print("two-address alias and passes a true one")
            return 0
        return 1

    # BSSFIX's shape: validate the rules BEFORE looking at the tree, every run.
    if not self_check(verbose=verbose):
        self_check(verbose=True)
        print("")
        print("REFUSING THE BUILD: the rule engine did not reproduce its own")
        print("fixtures. A rule has been deleted, weakened or broken, which silently")
        print("disarms the check -- which is the exact failure this guard exists to")
        print("prevent. Fix the engine, do not delete the fixture.")
        print("port/tools/vtalias_guard.py")
        return 1

    rest = [a for a in argv if not a.startswith("--")]
    root = rest[0] if rest else os.path.abspath(
        os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))
    return check(root, verbose)


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
