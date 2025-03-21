
; ModuleID = 'data/output.ll'
target triple = "arm64-apple-macosx15.0.0"

declare i32 @printf(ptr)

@.str = private unnamed_addr constant [18 x i8] c"ratty ratty rat!\0A\00", align 1

define i32 @main() {
	call i32 @printf(ptr @.str)
	ret i32 0
}

