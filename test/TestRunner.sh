#!/bin/sh
# Deprecated, use 'llvm37-lit'.

echo "warning: '$0' is deprecated, use 'llvm37-lit' instead."
exec llvm37-lit "$@"
