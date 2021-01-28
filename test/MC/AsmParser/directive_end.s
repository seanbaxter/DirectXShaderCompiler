# RUN: llvm37-mc -triple i386-unknown-unknown %s -filetype obj -o - \
# RUN:   | llvm37-readobj -t | FileCheck %s

	.end

its_a_tarp:
	int $0x3

# CHECK: Symbol {
# CHECK-NOT:   Name: its_a_tarp

