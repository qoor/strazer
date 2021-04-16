#ifndef _STRAZER_UTILS_H
#define _STRAZER_UTILS_H

#include <cinttypes>
#include <string_view>

namespace strazer {
namespace charinfo {
extern const uint16_t g_info_table[256];

enum {
  CHAR_HORZ_WS = 0x0001,  // '\t', '\f', '\v'.  Note, no '\0'
  CHAR_VERT_WS = 0x0002,  // '\r', '\n'
  CHAR_SPACE = 0x0004,    // ' '
  CHAR_DIGIT = 0x0008,    // 0-9
  CHAR_XLETTER = 0x0010,  // a-f,A-F
  CHAR_UPPER = 0x0020,    // A-Z
  CHAR_LOWER = 0x0040,    // a-z
  CHAR_UNDER = 0x0080,    // _
  CHAR_PERIOD = 0x0100,   // .
  CHAR_RAWDEL = 0x0200,   // {}[]#<>%:;?*+-/^&|~!=,"'
  CHAR_PUNCT = 0x0400     // `$@()
};

enum {
  CHAR_XUPPER = CHAR_XLETTER | CHAR_UPPER,
  CHAR_XLOWER = CHAR_XLETTER | CHAR_LOWER
};

}  // namespace charinfo

inline bool IsASCII(char c) { return static_cast<unsigned char>(c) <= 127; }

inline bool IsIdentifierHead(unsigned char c) {
  using namespace charinfo;
  return (CHAR_UPPER | CHAR_LOWER | CHAR_UNDER) & g_info_table[c];
}

inline bool IsIdentifierBody(unsigned char c) {
  using namespace charinfo;
  return (CHAR_UPPER | CHAR_LOWER | CHAR_DIGIT | CHAR_UNDER) & g_info_table[c];
}

inline bool IsHorizontalWhitespace(unsigned char c) {
  using namespace charinfo;
  return ((CHAR_HORZ_WS | CHAR_SPACE) & g_info_table[c]) != 0;
}

inline bool IsVerticalWhitespace(unsigned char c) {
  using namespace charinfo;
  return (CHAR_VERT_WS & g_info_table[c]) != 0;
}

inline bool IsWhitespace(unsigned char c) {
  using namespace charinfo;
  return ((CHAR_HORZ_WS | CHAR_VERT_WS | CHAR_SPACE) & g_info_table[c]) != 0;
}

inline bool IsDigit(unsigned char c) {
  using namespace charinfo;
  return (CHAR_DIGIT & g_info_table[c]) != 0;
}

inline bool IsLowercase(unsigned char c) {
  using namespace charinfo;
  return (CHAR_LOWER & g_info_table[c]) != 0;
}

inline bool IsUppercase(unsigned char c) {
  using namespace charinfo;
  return (CHAR_UPPER & g_info_table[c]) != 0;
}

inline bool IsLetter(unsigned char c) {
  using namespace charinfo;
  return ((CHAR_UPPER | CHAR_LOWER) & g_info_table[c]) != 0;
}

inline bool IsAlphanumeric(unsigned char c) {
  using namespace charinfo;
  return ((CHAR_DIGIT | CHAR_UPPER | CHAR_LOWER) & g_info_table[c]) != 0;
}

inline bool IsHexDigit(unsigned char c) {
  using namespace charinfo;
  return ((CHAR_DIGIT | CHAR_XLETTER) & g_info_table[c]) != 0;
}

inline bool IsPunctuation(unsigned char c) {
  using namespace charinfo;
  return ((CHAR_UNDER | CHAR_PERIOD | CHAR_RAWDEL | CHAR_PUNCT) &
          g_info_table[c]) != 0;
}

inline bool IsPrintable(unsigned char c) {
  using namespace charinfo;
  return ((CHAR_UPPER | CHAR_LOWER | CHAR_PERIOD | CHAR_PUNCT | CHAR_DIGIT |
           CHAR_UNDER | CHAR_RAWDEL | CHAR_SPACE) &
          g_info_table[c]) != 0;
}

inline bool IsValidIdentifier(std::string_view s) {
  if (s.empty() || !IsIdentifierBody(s[0])) return false;

  for (auto it : s) {
    if (!IsIdentifierBody(it)) return false;
  }

  return true;
}

}  // namespace strazer

#endif
