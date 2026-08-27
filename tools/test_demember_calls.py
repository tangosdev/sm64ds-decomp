"""A rewrite that redirects a call cannot be checked by comparing bytes.

`demember_calls.py` changes WHICH SYMBOL a call resolves to. A call is a
relocation and `match.compare` wildcards every relocated word, so sending a call
to the wrong function compares clean -- the byte gate is blind, by construction,
to the one thing this tool edits. `build_pin.verify` closes most of that with a
reloc-destination check, but only for the function being verified; the pass that
CHOOSES call sites has to be right on its own.

It was not. `find_call` matches the next call spelled with a given NAME, on any
object at all, and files routinely declare the same name in two local classes:

    struct ModelBase          { int SetFile(BMD_File*, int, int); };
    struct MovingMeshCollider { int SetFile(KCL_File*, ...); };

Only the second is missing from the ROM. A guard bailed out of any file where
the name occurred twice, which was safe and blocked eleven files. These tests
cover what replaced it: attribute the call to a class, then rewrite only that
class's. Everything here is text in, text out -- no compiler, no ROM.
"""
import pathlib
import sys

TOOLS = pathlib.Path(__file__).resolve().parent
sys.path.insert(0, str(TOOLS))

import demember_calls as D     # noqa: E402


# ------------------------------------------------------------- attributing a receiver

def test_receiver_class_reads_the_cast():
    assert D.receiver_class("((Actor*)self)") == "Actor"
    assert D.receiver_class("((Actor *) self)") == "Actor"
    assert D.receiver_class("((MovingMeshCollider *)(c + 0x124))") == "MovingMeshCollider"
    assert D.receiver_class("((const Actor*)self)") == "Actor"
    assert D.receiver_class("((struct Actor*)self)") == "Actor"
    assert D.receiver_class("((Ns::Actor*)self)") == "Actor"
    assert D.receiver_class("((Actor**)self)") == "Actor"


def test_the_outermost_cast_is_the_receivers_type():
    """`((ModelAnim*)((char*)c + 0x108))` is a ModelAnim. The inner cast is
    address arithmetic and naming it would attribute the call to `char`."""
    assert D.receiver_class("((ModelAnim*)((char*)c + 0x108))") == "ModelAnim"
    assert D.receiver_class("((WithMeshClsn*)&((Obj*)this)->f144)") == "WithMeshClsn"


def test_a_receiver_that_names_nothing_is_not_evidence():
    for recv in ("self", "obj->field", "o", "arr[i]", "(*(Actor**)x)", "this"):
        assert D.receiver_class(recv) is None, recv


def test_unwrap_does_not_mistake_two_groups_for_one():
    """`(a) + (b)` starts and ends with a paren without being wrapped in one."""
    assert D._unwrap("((Actor*)self)") == "(Actor*)self"
    assert D._unwrap("(a) + (b)") == "(a) + (b)"
    assert D._unwrap("self") == "self"


def test_find_static_call_reports_the_owner():
    src = "  OAM::Render(0, 1);\n  View::Render();\n"
    owner, args, _ = D.find_static_call(src, "Render")
    assert owner == "OAM" and args == ["0", "1"]
    owner, _, _ = D.find_static_call(src, "Render", src.index("View"))
    assert owner == "View"
    # A qualified name preceded by a type token is a definition, not a call.
    assert D.find_static_call("System* System::New(unsigned a) { return 0; }", "New") is None


# ------------------------------------------------- the property the guard used to buy

# src/func_ov022_02111bdc.cpp in miniature, including the ordering that matters:
# the call that must NOT move comes first, so "rewrite the next SetFile" is wrong.
TWO_CLASSES = """\
struct ModelBase { int SetFile(BMD_File *f, int a, int b); };
struct MovingMeshCollider { int SetFile(KCL_File *f, const Matrix4x3 &m, int c); };

void f(char *c) {
    ((ModelBase *)(c + 0xd4))->SetFile(f, 1, -1);
    ((MovingMeshCollider *)(c + 0x124))->SetFile(k, *(Matrix4x3 *)(c + 0x2ec), 0x1000);
}
"""


def test_only_the_named_classs_calls_are_rewritten():
    todo = [("SetFile", "_ZN18MovingMeshCollider7SetFileE_", False,
             "MovingMeshCollider", True)]
    new, n, why = D.rewrite_calls(TWO_CLASSES, todo)
    assert why is None and n == 1, (n, why)
    assert "((ModelBase *)(c + 0xd4))->SetFile(f, 1, -1);" in new, \
        "a working call to a different class's method was redirected"
    assert "_ZN18MovingMeshCollider7SetFileE_((MovingMeshCollider *)(c + 0x124)," in new
    assert "->SetFile" not in new.split("MovingMeshCollider *)(c + 0x124)")[1]


def test_two_classes_of_the_same_name_go_to_two_different_symbols():
    """src/game/actors/WaterBomb/_ZN9WaterBomb13InitResourcesEv.cpp: `Init` on MovingCylinderClsn and on
    WithMeshClsn, interleaved. Sending all four to one symbol still byte-matches."""
    src = """\
struct MovingCylinderClsn { void Init(Actor *a, int r); };
struct WithMeshClsn { void Init(Actor *a, int b); };
void f(void *t) {
    ((MovingCylinderClsn*)&((Obj*)t)->f110)->Init((Actor*)t, 0x14000);
    ((WithMeshClsn*)&((Obj*)t)->f144)->Init((Actor*)t, 0x1e000);
}
"""
    todo = [("Init", "_ZN18MovingCylinderClsn4InitE_", False, "MovingCylinderClsn", True),
            ("Init", "_ZN12WithMeshClsn4InitE_", False, "WithMeshClsn", True)]
    new, n, why = D.rewrite_calls(src, todo)
    assert why is None and n == 2, (n, why)
    assert "_ZN18MovingCylinderClsn4InitE_((MovingCylinderClsn*)&((Obj*)t)->f110, " in new
    assert "_ZN12WithMeshClsn4InitE_((WithMeshClsn*)&((Obj*)t)->f144, " in new
    assert "->Init(" not in new


def test_a_qualified_call_on_another_class_is_left_alone():
    """src/game/camera/Camera/_ZN6Camera6RenderEv.cpp: three `OAM::Render(..)` and one `View::Render()`."""
    src = """\
struct OAM { static void Render(bool, int); };
struct View { int Render(); };
void f() {
    OAM::Render(0, 1);
    OAM::Render(0, 2);
    View::Render();
}
"""
    new, n, why = D.rewrite_calls(src, [("Render", "_ZN3OAM6RenderE_", True, "OAM", True)])
    assert why is None and n == 2, (n, why)
    assert "View::Render();" in new, "a base-class call was redirected to OAM's symbol"
    assert "OAM::Render(" not in new


def test_an_unattributable_call_of_an_ambiguous_name_stops_the_file():
    """Two declarations and a receiver that names neither: there is no evidence
    which is meant, so the file is not touched -- not guessed at."""
    src = """\
struct A { int run(int); };
struct B { int run(int); };
void f(void *p) { p->run(1); }
"""
    new, n, why = D.rewrite_calls(src, [("run", "_ZN1A3runE_", False, "A", True)])
    assert n == 0 and why and "ambiguous" in why
    assert new == src, "the bail path must not leave a half-rewritten file behind"


def test_an_unambiguous_name_still_needs_no_cast():
    """The common case, and the reason the check is not unconditional: one
    declaration in the file means a bare receiver has only one thing it can mean."""
    src = """\
struct Actor { short ReflectAngle(int, int, short); };
void f(void *self) { self->ReflectAngle(a, b, c); }
"""
    new, n, why = D.rewrite_calls(src, [("ReflectAngle", "_ZN5ActorE_", False,
                                         "Actor", False)])
    assert why is None and n == 1
    assert "_ZN5ActorE_(self, a, b, c)" in new


def test_a_call_that_never_matched_still_stops_the_file():
    """Leaving the only call site behind would keep the mangled name the ROM does
    not define, so the file could not enroll and the edit would be pure churn."""
    src = "struct A { int run(int); };\nvoid f(A *p) { }\n"
    new, n, why = D.rewrite_calls(src, [("run", "_ZN1A3runE_", False, "A", False)])
    assert n == 0 and why == "call site not in a recognised shape"
    assert new == src


# ------------------------------------------------------- the implicit-`this` shape

# src/game/player/Player/_ZN6Player12St_Land_InitEv.cpp in miniature: the file IS a Player member, and
# calls its sibling with no receiver at all.
IMPLICIT = """\
struct Player {
    int St_Land_Init();
    int IsAnim(unsigned int anim);
    void SetAnim(unsigned int a, int b, int c, unsigned int d);
};
void SetAnim(int wrong);
int Player::St_Land_Init()
{
    SetAnim(0x52, 0x40000000, 0x1000, 0);
    return IsAnim(0x2b);
}
"""


def test_an_unqualified_sibling_call_is_found():
    new, n, why = D.rewrite_calls(IMPLICIT, [("SetAnim", "_ZN6PlayerE_", False,
                                              "Player", True)])
    assert why is None and n == 1, (n, why)
    assert "_ZN6PlayerE_(this, 0x52, 0x40000000, 0x1000, 0);" in new
    # The free `void SetAnim(int wrong);` is a DECLARATION outside any member body.
    assert "void SetAnim(int wrong);" in new


def test_nothing_outside_a_member_body_is_touched():
    """The struct's own declaration of the method sits in no member body, and a
    call in an unrelated function is not this class's."""
    src = """\
struct Player { void SetAnim(unsigned int a); };
void elsewhere(void *p) { SetAnim(1); }
"""
    new, n, why = D.rewrite_calls(src, [("SetAnim", "_ZN6PlayerE_", False,
                                         "Player", False)])
    assert n == 0 and why == "call site not in a recognised shape"
    assert new == src


def test_member_bodies_skips_declarations_and_const_members():
    """A `const` member's `this` is a `const Player*` and does not convert to the
    void* receiver, so those bodies are not offered up in the first place."""
    assert D.member_bodies("int Player::f();\n", "Player") == []
    assert D.member_bodies("int Player::f() const { g(); }\n", "Player") == []
    assert len(D.member_bodies("int Player::f() { g(); }\n", "Player")) == 1


def test_an_implicit_and_an_explicit_call_of_the_same_method_both_go():
    src = """\
struct Player { void SetAnim(unsigned int a); };
int Player::f() { SetAnim(1); ((Player*)other)->SetAnim(2); return 0; }
"""
    new, n, why = D.rewrite_calls(src, [("SetAnim", "_ZN6PlayerE_", False,
                                         "Player", False)])
    assert why is None and n == 2, (n, why)
    assert "_ZN6PlayerE_(this, 1)" in new and "_ZN6PlayerE_((Player*)other, 2)" in new
    assert "SetAnim(" not in new.split("struct Player")[1].split("};")[1]


def test_a_dot_call_still_passes_the_address():
    src = "struct A { int run(int); };\nvoid f(A a) { a.run(1); }\n"
    new, n, why = D.rewrite_calls(src, [("run", "_ZN1A3runE_", False, "A", False)])
    assert why is None and n == 1 and "_ZN1A3runE_(&(a), 1)" in new


if __name__ == "__main__":
    fails = 0
    for nm, fn in sorted(globals().items()):
        if nm.startswith("test_") and callable(fn):
            try:
                fn()
                print(f"  PASS  {nm}")
            except AssertionError as e:
                fails += 1
                print(f"  FAIL  {nm}: {e}")
    print(f"\n{fails} failure(s)")
    sys.exit(1 if fails else 0)
