
; ModuleID = 'data/compile.ll'
target triple = "arm64-apple-macosx15.0.0"

declare i32 @printf(ptr)


define i32 @main(i32 noundef %0, i32 noundef %1) {
	%2 = alloca i32, align 4
	store i32 15, ptr %2, align 4
	%3 = load i32, ptr %2, align 4
	%4 = sdiv i32 %3, 2
	ret i32 %4
}

