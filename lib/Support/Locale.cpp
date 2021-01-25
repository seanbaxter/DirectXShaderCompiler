
#include "llvm37/Support/Locale.h"
#include "llvm37/Support/Unicode.h"

namespace llvm37 {
namespace sys {
namespace locale {

int columnWidth(StringRef Text) {
#if LLVM37_ON_WIN32
  return Text.size();
#else
  return llvm37::sys::unicode::columnWidthUTF8(Text);
#endif
}

bool isPrint(int UCS) {
#if LLVM37_ON_WIN32
  // Restrict characters that we'll try to print to the lower part of ASCII
  // except for the control characters (0x20 - 0x7E). In general one can not
  // reliably output code points U+0080 and higher using narrow character C/C++
  // output functions in Windows, because the meaning of the upper 128 codes is
  // determined by the active code page in the console.
  return ' ' <= UCS && UCS <= '~';
#else
  return llvm37::sys::unicode::isPrintable(UCS);
#endif
}

} // namespace locale
} // namespace sys
} // namespace llvm37
