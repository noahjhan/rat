
; ModuleID = 'data/compile.ll'
target triple = "arm64-apple-macosx15.0.0"

declare i32 @printf(ptr, ...)

@.str.1 = private unnamed_addr constant [6 x i8] c"here\0A\00", align 1

define i32 @foo(i32 noundef %0) {
	%2 = alloca i32, align 4
	store i32 %0, ptr %2, align 4
	%3 = call i32 @printf(ptr @.str.1)
	%4 = load i32, ptr %2, align 4
	%5 = icmp eq i32 %4, 0
	br i1 %5, label %6, label %8

6:
	%7 = load i32, ptr %2, align 4
	ret i32 %7
8:
	br label %9

9:
	%10 = load i32, ptr %2, align 4
	%11 = sub i32 %10, 1
	%12 = call i32 @foo(i32 noundef %11)
	ret i32 %12
}

define i32 @main() {
	%1 = alloca i32, align 4
	%2 = alloca i32, align 4
	store i32 10, ptr %1, align 4
	%3 = load i32, ptr %1, align 4
	%4 = call i32 @foo(i32 noundef %3)
	store i32 %4, ptr %2, align 4
	ret i32 0
}

