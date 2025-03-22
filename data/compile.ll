
; ModuleID = 'data/compile.ll'
target triple = "arm64-apple-macosx15.0.0"

declare i32 @printf(ptr)

@.str2 = private unnamed_addr constant [8 x i8] c"wordle\0A\00", align 1
@.str1 = private unnamed_addr constant [15 x i8] c"hello, world!\0A\00", align 1
@.str0 = private unnamed_addr constant [28 x i8] c"ratty ratty rat faced rat!\0A\00", align 1
define i32 @main() {
	call i32 @printf(ptr @.str0)
	call i32 @printf(ptr @.str1)
	call i32 @printf(ptr @.str2)
	ret i32 0
}

