
; ModuleID = 'data/compile.ll'
target triple = "arm64-apple-macosx15.0.0"

declare i32 @printf(ptr, ...)

@.str.1 = private unnamed_addr constant [6 x i8] c"hello\00", align 1
@.str.2 = private unnamed_addr constant [1 x i8] c"\00", align 1

define i32 @main() {
	%1 = alloca ptr, align 8
	store ptr @.str.1, ptr %1, align 8
	%2 = load ptr, ptr %1, align 8
	%3 = call i32 (ptr, ...) @printf(ptr @.str.2, ptr %2)
	ret i32 0
}

