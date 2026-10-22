#include "forumer_core/names.h"

#include <algorithm>

#include <nlohmann/json.hpp>

namespace forumer::names {

using json = nlohmann::json;

namespace {

constexpr size_t kMaxNameLength = 32;
constexpr size_t kKeyHexLength = 64;

bool isNameChar(char c) { return (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '-'; }

bool isSeparator(char c) { return c == ' ' || c == ',' || c == '\n' || c == '\r' || c == '\t'; }

std::string asciiLower(std::string_view text) {
    std::string out(text);
    std::transform(out.begin(), out.end(), out.begin(),
                   [](char c) { return (c >= 'A' && c <= 'Z') ? char(c - 'A' + 'a') : c; });
    return out;
}

} // namespace

std::optional<std::string> claimedName(std::string_view alias) {
    if (alias.empty() || alias.size() > kMaxNameLength) return std::nullopt;
    if (alias.front() == '-' || alias.back() == '-') return std::nullopt;
    if (!std::all_of(alias.begin(), alias.end(), isNameChar)) return std::nullopt;
    return std::string(alias);
}

std::vector<Bytes> listedKeys(std::string_view entry) {
    std::vector<Bytes> keys;
    size_t i = 0;
    while (i < entry.size()) {
        while (i < entry.size() && isSeparator(entry[i])) ++i;
        size_t end = i;
        while (end < entry.size() && !isSeparator(entry[end])) ++end;
        const std::string_view token = entry.substr(i, end - i);
        if (token.size() == kKeyHexLength) {
            if (auto key = fromHex(asciiLower(token)))
                keys.push_back(std::move(*key));
        }
        i = end;
    }
    return keys;
}

std::string withKey(std::string_view entry, const Bytes& publicKey) {
    auto keys = listedKeys(entry);
    if (std::find(keys.begin(), keys.end(), publicKey) == keys.end())
        keys.push_back(publicKey);
    std::string out;
    for (const auto& key : keys) {
        if (!out.empty()) out += ' ';
        out += toHex(key);
    }
    return out;
}

std::optional<std::string> personaEntry(std::string_view resolveReply, const std::string& name) {
    const json reply = json::parse(resolveReply, nullptr, /*allow_exceptions=*/false);
    if (!reply.is_object() || reply.value("success", false) != true) return std::nullopt;
    const auto value = reply.find("value");
    if (value == reply.end() || !value->is_object()) return std::nullopt;
    const auto recordName = value->find("name");
    if (recordName == value->end() || !recordName->is_string() || *recordName != name)
        return std::nullopt;
    const auto texts = value->find("texts");
    if (texts == value->end() || !texts->is_object()) return std::string();
    const auto entry = texts->find(std::string(kPersonaEntry));
    if (entry == texts->end() || !entry->is_string()) return std::string();
    return entry->get<std::string>();
}

bool replyVouches(std::string_view resolveReply, const std::string& name, const Bytes& publicKey) {
    const auto entry = personaEntry(resolveReply, name);
    if (!entry) return false;
    const auto keys = listedKeys(*entry);
    return std::find(keys.begin(), keys.end(), publicKey) != keys.end();
}

} // namespace forumer::names
