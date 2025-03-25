
; ModuleID = 'data/compile.ll'
target triple = "arm64-apple-macosx15.0.0"

declare i32 @printf(ptr)


define i32 @foo(i32 noundef %0, i32 noundef %1) {
	%3 = alloca i32, align 4
	%4 = alloca i32, align 4
	store i32 %0, ptr %3, align 4
	store i32 %1, ptr %4, align 4
	%5 = load i32, ptr %3, align 4
	%6 = load i32, ptr %4, align 4
	%7 = add i32 %5, %6
	ret i32 %7
}

define void @voidy() {
	ret void
}

define i32 @main() {
	%1 = alloca i32, align 4
	%2 = alloca i32, align 4
	%3 = alloca i32, align 4
	store i32 10, ptr %1, align 4
	store i32 20, ptr %2, align 4
	%4 = load i32, ptr %1, align 4
	%5 = load i32, ptr %2, align 4
	%6 = call i32 @foo(i32 noundef %4,i32 noundef %5)
	store i32 %6, ptr %3, align 4
	ret i32 0
}

