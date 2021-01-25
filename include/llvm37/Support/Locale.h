
#ifndef LLVM37_SUPPORT_LOCALE_H
#define LLVM37_SUPPORT_LOCALE_H

#include "llvm37/ADT/StringRef.h"

namespace llvm37 {
namespace sys {
namespace locale {

int columnWidth(StringRef s);
bool isPrint(int c);

}
}
}

#endif // LLVM37_SUPPORT_LOCALE_H
