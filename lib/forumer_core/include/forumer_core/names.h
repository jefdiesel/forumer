#pragma once

// Logos names (LNS) as verified aliases.
//
// A post claims a name by carrying it as its alias (Disclosure::Alias), so the
// wire format is unchanged and older clients show it as an ordinary alias:
// "jef · fr:7Q4K-M2XD". A client that has the `names` module checks the claim:
// the name's record must list the post's persona key under kPersonaEntry.
// Only the holder of a name can write its record, so a match proves the holder
// vouches for that persona. The holder can be a public or a private account;
// the check never looks at who holds the name, only at what it lists.
//
//   forumer.persona = "<64 hex chars> <64 hex chars> ..."   (persona public keys)

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "forumer_core/bytes.h"

namespace forumer::names {

/// The name's text entry that lists the personas allowed to post as it.
inline constexpr std::string_view kPersonaEntry = "forumer.persona";

/// The LNS name an alias claims, or nullopt. Only a bare, valid LNS name
/// counts: 1-32 of [a-z0-9-], no hyphen at either end. "jef" claims jef;
/// "Jef", "jef.lez", "@jef" and "night owl" claim nothing (the composer writes
/// names bare, so anything else is an ordinary alias).
std::optional<std::string> claimedName(std::string_view alias);

/// The persona keys a kPersonaEntry value lists: 32-byte keys as 64 hex
/// characters, separated by spaces, commas or newlines. Anything else in the
/// value is ignored.
std::vector<Bytes> listedKeys(std::string_view entry);

/// `entry` with `publicKey` added (unchanged if already listed), normalised to
/// lowercase hex separated by single spaces.
std::string withKey(std::string_view entry, const Bytes& publicKey);

/// The kPersonaEntry value in a `names.resolve` reply for `name`, "" when the
/// record has none, or nullopt when the reply is not a record for `name`.
std::optional<std::string> personaEntry(std::string_view resolveReply, const std::string& name);

/// Whether a `names.resolve` reply proves `publicKey` may post as `name`:
/// {"success": true, "value": {"name": <name>, "texts": {kPersonaEntry: ...}}}
/// with the key listed. Any other reply (an error, an unregistered name, a
/// record for another name, a missing entry) proves nothing.
bool replyVouches(std::string_view resolveReply, const std::string& name, const Bytes& publicKey);

} // namespace forumer::names
