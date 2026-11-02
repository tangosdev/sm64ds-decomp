// Unit tests for hal/mod_values.h (run after093, lane CHARPALG1): reading a
// catalog mod's settings.json spec and settings.values.json the way
// hal/fs_mods.cpp does for character-palettes and loves-me-swap.
// Build: cl /nologo /std:c++14 /EHsc /I port/hal port/tools/mod_values_test.cpp
#include <cstdio>
#include <cstring>

#include "mod_values.h"

static int g_fail;

#define CHECK(c) do { if (!(c)) { ++g_fail; std::printf("FAIL %s:%d %s\n", __FILE__, __LINE__, #c); } } while (0)

static const char kSpec[] =
    "[\n"
    "  {\"key\": \"mario\", \"label\": \"Mario\", \"type\": \"choice\",\n"
    "   \"choices\": [\"Original\", \"Custom\"], \"default\": \"Original\"},\n"
    "  {\"key\": \"mario_hat\", \"label\": \"Hat\", \"type\": \"color\", \"default\": \"DE0000\",\n"
    "   \"showWhen\": {\"key\": \"mario\", \"equals\": \"Custom\"}},\n"
    "  {\"key\": \"yoshi\", \"label\": \"Yoshi\", \"type\": \"choice\",\n"
    "   \"choices\": [\"Original\", \"Green\", \"Red\", \"Blue\", \"Yellow\", \"Custom\"],\n"
    "   \"default\": \"Original\"},\n"
    "  {\"showWhen\": {\"key\": \"yoshi\", \"equals\": \"Custom\"}, \"key\": \"yoshi_body\",\n"
    "   \"type\": \"color\", \"default\": \"31A431\"},\n"
    "  {\"key\": \"odd\", \"label\": \"a \\\"quoted\\\" {brace}\", \"default\": \"x\\\\y\"}\n"
    "]\n";

static void test_top_str()
{
    char out[64];
    CHECK(modval::top_str("{\"a\": \"1\", \"b\": \"2\"}", "b", out, sizeof out) && !std::strcmp(out, "2"));
    CHECK(!modval::top_str("{\"a\": {\"b\": \"in\"}}", "b", out, sizeof out));
    // the setting's own key, never showWhen's, whichever comes first
    CHECK(modval::top_str("{\"showWhen\": {\"key\": \"mario\"}, \"key\": \"mario_hat\"}", "key", out, sizeof out)
          && !std::strcmp(out, "mario_hat"));
    // a value that looks like a name is not one
    CHECK(!modval::top_str("{\"x\": \"key\"}", "key", out, sizeof out));
    // too long for cap
    CHECK(!modval::top_str("{\"a\": \"123456\"}", "a", out, 4));
    // not a string
    CHECK(!modval::top_str("{\"a\": true}", "a", out, sizeof out));
    CHECK(!modval::top_str(0, "a", out, sizeof out));
}

static void test_spec()
{
    char out[64];
    CHECK(modval::spec_default(kSpec, "mario", out, sizeof out) && !std::strcmp(out, "Original"));
    CHECK(modval::spec_default(kSpec, "mario_hat", out, sizeof out) && !std::strcmp(out, "DE0000"));
    CHECK(modval::spec_default(kSpec, "yoshi_body", out, sizeof out) && !std::strcmp(out, "31A431"));
    CHECK(modval::spec_default(kSpec, "odd", out, sizeof out) && !std::strcmp(out, "x\\y"));
    CHECK(!modval::spec_default(kSpec, "luigi", out, sizeof out));
    CHECK(!modval::spec_default(kSpec, "equals", out, sizeof out));
}

static void test_value()
{
    char out[64];
    const char *vals = "{\"mario\":\"Custom\",\"mario_hat\":\"FFBF7F\"}";
    CHECK(modval::value(kSpec, vals, "mario", out, sizeof out) == 2 && !std::strcmp(out, "Custom"));
    CHECK(modval::value(kSpec, vals, "mario_hat", out, sizeof out) == 2 && !std::strcmp(out, "FFBF7F"));
    CHECK(modval::value(kSpec, vals, "yoshi", out, sizeof out) == 1 && !std::strcmp(out, "Original"));
    CHECK(modval::value(kSpec, 0, "yoshi_body", out, sizeof out) == 1 && !std::strcmp(out, "31A431"));
    CHECK(modval::value(0, 0, "yoshi", out, sizeof out) == 0 && out[0] == '\0');
    CHECK(modval::value(kSpec, "{\"mario\": \"\"}", "mario", out, sizeof out) == 2 && out[0] == '\0');
}

static void test_hex6()
{
    CHECK(modval::hex6("FFBF7F") == 0xFFBF7F);
    CHECK(modval::hex6("007fff") == 0x007FFF);
    CHECK(modval::hex6("FFFFFF") == 0xFFFFFF);
    CHECK(modval::hex6("FFFFF") == -1);
    CHECK(modval::hex6("FFFFFFF") == -1);
    CHECK(modval::hex6("#FFFFF") == -1);
    CHECK(modval::hex6("") == -1);
    CHECK(modval::hex6(0) == -1);
}

static void test_choice()
{
    int row;
    CHECK(modval::choice_kind("Original", 0, &row) == modval::CP_ORIGINAL && row == -1);
    CHECK(modval::choice_kind("", 1, &row) == modval::CP_ORIGINAL);
    CHECK(modval::choice_kind(0, 1, &row) == modval::CP_ORIGINAL);
    CHECK(modval::choice_kind("Custom", 0, &row) == modval::CP_CUSTOM);
    CHECK(modval::choice_kind("custom", 1, &row) == modval::CP_CUSTOM);
    CHECK(modval::choice_kind("Green", 1, &row) == modval::CP_ROW && row == 0);
    CHECK(modval::choice_kind("Red", 1, &row) == modval::CP_ROW && row == 1);
    CHECK(modval::choice_kind("Blue", 1, &row) == modval::CP_ROW && row == 2);
    CHECK(modval::choice_kind("Yellow", 1, &row) == modval::CP_ROW && row == 3);
    // only Yoshi has rows: for a bro "Red" is a palette name
    CHECK(modval::choice_kind("Red", 0, &row) == modval::CP_FILE && row == -1);
    CHECK(modval::choice_kind("combo1", 0, &row) == modval::CP_FILE);
}

static void test_lovesme()
{
    // the launcher's labels in its order = host_settings.cpp LOVESME_KEY, 1-based
    CHECK(modval::lovesme_index("Mario") == 1);
    CHECK(modval::lovesme_index("Red Yoshi") == 2);
    CHECK(modval::lovesme_index("Blue Yoshi") == 3);
    CHECK(modval::lovesme_index("Yellow Yoshi") == 4);
    CHECK(modval::lovesme_index("Luigi") == 5);
    CHECK(modval::lovesme_index("Wario") == 6);
    CHECK(modval::lovesme_index("yoshi_blue") == 3);
    CHECK(modval::lovesme_index("Metal Wario") == 0);
    CHECK(modval::lovesme_index("") == 0);
}

// With a path: the real settings.json of the character-palettes package. Every
// key the game reads must have a string default there.
static void test_package(const char *path)
{
    static char buf[1 << 16];
    std::FILE *f = std::fopen(path, "rb");
    CHECK(f != 0);
    if (!f) return;
    const size_t n = std::fread(buf, 1, sizeof buf - 1, f);
    std::fclose(f);
    buf[n] = '\0';
    static const char *const kKeys[] = {
        "mario", "luigi", "wario", "yoshi",
        "mario_hat", "mario_overalls", "mario_shoes", "luigi_hat", "luigi_overalls", "luigi_shoes",
        "wario_hat", "wario_overalls", "wario_shoes", "yoshi_body", 0 };
    for (int i = 0; kKeys[i]; ++i) {
        char out[64];
        const int got = modval::spec_default(buf, kKeys[i], out, sizeof out);
        CHECK(got);
        if (i < 4) CHECK(got && !std::strcmp(out, "Original"));
        else CHECK(got && modval::hex6(out) >= 0);
        std::printf("  package %s = %s\n", kKeys[i], got ? out : "(none)");
    }
}

int main(int argc, char **argv)
{
    test_top_str();
    test_spec();
    test_value();
    test_hex6();
    test_choice();
    test_lovesme();
    if (argc > 1) test_package(argv[1]);
    std::printf("mod_values_test: %s (%d failure%s)\n", g_fail ? "FAIL" : "PASS", g_fail, g_fail == 1 ? "" : "s");
    return g_fail ? 1 : 0;
}
