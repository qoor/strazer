#ifndef _STRAZER_UTILS_H
#define _STRAZER_UTILS_H

#include <cinttypes>
#include <memory>
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

inline bool IsPreprocessingNumberBody(unsigned char c) {
  using namespace charinfo;
  return ((CHAR_UPPER | CHAR_LOWER | CHAR_DIGIT | CHAR_UNDER | CHAR_PERIOD) &
          g_info_table[c]) != 0;
}

class MemoryBuffer {
 public:
  enum BufferKind { kBufferMalloc, kBufferMmap };

  static std::unique_ptr<MemoryBuffer> GetFileOrSTDIN(std::string_view file,
                                                      bool text = false,
                                                      bool null = true);

  static std::unique_ptr<MemoryBuffer> GetFile(std::string_view file,
                                               bool text = false,
                                               bool null = true);

  virtual ~MemoryBuffer();

  const char* GetBufferStart() const { return buf_start_; }
  const char* GetBufferEnd() const { return buf_end_; }
  size_t GetBufferSize() const { return buf_end_ - buf_start_; }

  std::string_view GetBuffer() const {
    return std::string_view(GetBufferStart(), GetBufferSize());
  }

  virtual std::string_view GetBufferIdentifier() const {
    return "Unknown buffer";
  }

  virtual BufferKind GetBufferKind() const = 0;

 protected:
  MemoryBuffer() = default;

  void Init(const char* buf_start, const char* buf_end, bool null);

 private:
  const char* buf_start_;
  const char* buf_end_;
};

class WritableMemoryBuffer : public MemoryBuffer {
 public:
  static std::unique_ptr<WritableMemoryBuffer> GetNewUninitMemBuffer(
      size_t size, std::string_view buf_name = "");
  static std::unique_ptr<WritableMemoryBuffer> GetNewMemBuffer(
      size_t size, std::string_view buf_name = "");

  char* GetBufferStart() {
    return const_cast<char*>(MemoryBuffer::GetBufferStart());
  }
  char* GetBufferEnd() {
    return const_cast<char*>(MemoryBuffer::GetBufferEnd());
  }

  std::string_view GetBuffer() {
    char* buf_start = GetBufferStart();
    return std::string_view(buf_start, GetBufferSize());
  }

 protected:
  WritableMemoryBuffer() = default;

 private:
  using MemoryBuffer::GetFile;
  using MemoryBuffer::GetFileOrSTDIN;
};

namespace process {
size_t GetPageSize();
}

}  // namespace strazer

#endif
