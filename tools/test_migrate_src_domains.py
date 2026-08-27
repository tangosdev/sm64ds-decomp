import pathlib

import migrate_src_domains as M


def test_actor_membership_beats_incidental_subsystem_words():
    actors = {"AmbientSoundEffects", "LightBeam"}
    assert M.domain_for("AmbientSoundEffects_Spawn", "AmbientSoundEffects", actors) == \
        pathlib.PurePosixPath("game/actors")
    assert M.domain_for("LightBeam_Spawn", "LightBeam", actors) == \
        pathlib.PurePosixPath("game/actors")


def test_specific_game_roles_beat_actor_membership():
    actors = {"Player", "CameraTag", "dScMgJump_c"}
    assert M.domain_for("_ZN6Player8BehaviorEv", "Player", actors) == \
        pathlib.PurePosixPath("game/player")
    assert M.domain_for("CameraTag_Spawn", "CameraTag", actors) == \
        pathlib.PurePosixPath("game/camera")
    assert M.domain_for("dScMgJump_c_Spawn", "dScMgJump_c", actors) == \
        pathlib.PurePosixPath("minigames")


def test_class_is_classified_instead_of_method_words():
    assert M.domain_for("_ZN8SaveData15GetMinigameDataEv", "SaveData", set()) == \
        pathlib.PurePosixPath("runtime/filesystem")


def test_special_class_recovers_thunk_and_class_artifact_owners():
    assert M.special_class("_ZThn80_N9Animation7AdvanceEv") == "Animation"
    assert M.special_class("_ZTV5Model") == "Model"
    assert M.special_class("_ZTI13InvisiblePole") == "InvisiblePole"


def test_literal_path_pattern_takes_the_whole_cpp_suffix():
    text = "src/runtime/graphics/Model/_ZN5ModelD1Ev.cpp: and src/func_02004000.c:"
    assert [match.group(0) for match in M.POSIX_SOURCE.finditer(text)] == [
        "src/runtime/graphics/Model/_ZN5ModelD1Ev.cpp", "src/func_02004000.c",
    ]
