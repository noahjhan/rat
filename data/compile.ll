
; ModuleID = 'data/compile.ll'
target triple = "arm64-apple-macosx15.0.0"

declare i32 @printf(ptr)

@.str.1 = private unnamed_addr constant [8 x i8] c"ratiel\0A\00", align 1

define i32 @main() {
	%1 = alloca i32, align 4
	%2 = alloca i32, align 4
	%3 = add i32 2, 5
	store i32 %3, ptr %1, align 4
	%4 = load i32, ptr %1, align 4
	%5 = add i32 %4, 5
	%6 = mul i32 5, 5
	%7 = srem i32 %5, %6
	%8 = load i32, ptr %1, align 4
	%9 = mul i32 %7, %8
	store i32 %9, ptr %2, align 4
	call i32 @printf(ptr @.str.1)
	ret i32 0
}

