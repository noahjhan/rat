.text
.global _main

_main:
    mov x0, #12         // Move exit code 12 into register x0
    mov x16, #0x2000001  // System call umber for exit() on macOS ARM64
    svc #0              // Make the system call
