
; ModuleID = 'data/compile.ll'
target triple = "arm64-apple-macosx15.0.0"

declare i32 @printf(ptr, ...)

@.str.1 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

define i32 @main() {
	%1 = alloca i64, align 8
	store i64 0, ptr %1, align 8
	%2 = load i64, ptr %1, align 8
	%3 = sub i64 %2, 1
	%4 = call i32 (ptr, ...) @printf(ptr @.str.1, i64 %3)
	ret i32 0
}

