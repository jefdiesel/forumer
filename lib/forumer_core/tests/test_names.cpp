#include "test.h"

#include <cctype>

#include "forumer_core/bytes.h"
#include "forumer_core/identity.h"
#include "forumer_core/names.h"
#include "forumer_core/post.h"

using namespace forumer;
using namespace forumer::names;

namespace {

Bytes keyOf(uint8_t fill) { return Bytes(32, fill); }

std::string reply(const std::string& name, const std::string& entry) {
    return R"({"success":true,"error":null,"value":{"name":")" + name +
           R"(","texts":{"bio":"hi","forumer.persona":")" + entry + R"("}}})";
}

} // namespace

TEST(names_claimed_name_is_a_bare_lns_name) {
    CHECK(claimedName("jef") == std::optional<std::string>("jef"));
    CHECK(claimedName("bob-123") == std::optional<std::string>("bob-123"));
    CHECK(claimedName(std::string(32, 'a')).has_value());
    CHECK(!claimedName(std::string(33, 'a')).has_value());
    CHECK(!claimedName("").has_value());
    CHECK(!claimedName("Jef").has_value());
    CHECK(!claimedName("jef.lez").has_value());
    CHECK(!claimedName("@jef").has_value());
    CHECK(!claimedName("night owl").has_value());
    CHECK(!claimedName("-jef").has_value());
    CHECK(!claimedName("jef-").has_value());
    CHECK(!claimedName("\xe2\x84\xaa" "ate").has_value());  // U+212A KELVIN SIGN
}

TEST(names_listed_keys_parse_and_skip_junk) {
    const std::string a = toHex(keyOf(0xab));
    const std::string b = toHex(keyOf(0x01));
    std::string upper = a;
    for (auto& c : upper) c = char(std::toupper(static_cast<unsigned char>(c)));
    const auto keys = listedKeys(upper + ", junk\n" + b + "  " + a.substr(0, 10));
    CHECK_EQ(keys.size(), size_t(2));
    CHECK(keys[0] == keyOf(0xab));
    CHECK(keys[1] == keyOf(0x01));
    CHECK(listedKeys("").empty());
}

TEST(names_with_key_adds_once) {
    const std::string a = toHex(keyOf(0xab));
    CHECK_EQ(withKey("", keyOf(0xab)), a);
    CHECK_EQ(withKey(a, keyOf(0xab)), a);
    CHECK_EQ(withKey(a, keyOf(0x01)), a + " " + toHex(keyOf(0x01)));
}

TEST(names_reply_vouches_only_for_listed_key_and_matching_name) {
    const Bytes key = keyOf(0xab);
    const std::string entry = toHex(keyOf(0x01)) + " " + toHex(key);
    CHECK(replyVouches(reply("jef", entry), "jef", key));
    CHECK(!replyVouches(reply("jef", entry), "jef", keyOf(0x02)));
    CHECK(!replyVouches(reply("bob", entry), "jef", key));
    CHECK(!replyVouches(R"({"success":true,"value":null,"error":null})", "jef", key));
    CHECK(!replyVouches(R"({"success":false,"value":null,"error":"no"})", "jef", key));
    CHECK(!replyVouches(R"({"success":true,"value":{"name":"jef","texts":{}}})", "jef", key));
    CHECK(!replyVouches("not json", "jef", key));
}

TEST(names_persona_entry_reads_the_record_for_the_name) {
    CHECK(personaEntry(reply("jef", "aa"), "jef") == std::optional<std::string>("aa"));
    CHECK(personaEntry(R"({"success":true,"value":{"name":"jef","texts":{}}})", "jef") ==
          std::optional<std::string>(""));
    CHECK(!personaEntry(reply("bob", "aa"), "jef").has_value());
    CHECK(!personaEntry(R"({"success":true,"value":null})", "jef").has_value());
}

TEST(names_alias_carrying_a_name_is_valid_on_the_wire) {
    // A name rides as an ordinary alias, so clients without this feature
    // accept the post and show it as "jef · fr:...".
    CHECK(post::aliasProblem("jef") == nullptr);
    CHECK(post::aliasProblem(std::string(32, 'a')) == nullptr);
}
