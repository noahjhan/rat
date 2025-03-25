
; ModuleID = 'data/compile.ll'
target triple = "arm64-apple-macosx15.0.0"

declare i32 @printf(ptr)


define i32 @foo(i32 noundef %0) {
	%2 = alloca i32, align 4
	store i32 %0, ptr 2, align 4
	%3 = load i32, ptr %2, align 4
	ret i32 %3
}

define i32 @main() {
	%1 = alloca i32, align 4
	%2 = alloca i32, align 4
	store i32 20, ptr %1, align 4
	%3 = load i32, ptr %1, align 4
	%4 = call i32 @foo(i32 noundef %3)
	store i32 %4, ptr %2, align 4
	ret i32 0
}

