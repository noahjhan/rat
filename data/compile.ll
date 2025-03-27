
; ModuleID = 'data/compile.ll'
target triple = "arm64-apple-macosx15.0.0"

declare i32 @printf(ptr, ...)


define i32 @main() {
	%1 = alloca i32, align 4
	%2 = alloca i32, align 4
	store i32 10, ptr %1, align 4
	%3 = load i32, ptr %1, align 4
	%4 = add i32 %3, 1
	store i32 %4, ptr %2, align 4
	ret i32 0
}

