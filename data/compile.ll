
; ModuleID = 'data/compile.ll'
target triple = "arm64-apple-macosx15.0.0"

declare i32 @printf(ptr)


define i32 @main() {
	%1 = alloca i32, align 4
	store i32 15, ptr %1, align 4
	%2 = load i32, ptr %1, align 4
	%3 = sdiv i32 %2, 2
	ret i32 %3
}

