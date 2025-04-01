
; ModuleID = 'data/compile.ll'
target triple = "arm64-apple-macosx15.0.0"

declare i32 @printf(ptr, ...)

@.str.1 = private unnamed_addr constant [4 x i8] c"%lf\00", align 1

define i32 @printer(i32 noundef %0) {
	%2 = alloca i32, align 4
	store i32 %0, ptr %2, align 4
	%3 = load i32, ptr %2, align 4
	%4 = icmp eq i32 %3, 0
	br i1 %4, label %5, label %6

5:
	ret i32 0
6:
	br label %7

7:
	%8 = load i32, ptr %2, align 4
	%9 = sub i32 %8, 1
	%10 = call i32 @printer(i32 noundef %9)
	ret i32 %10
}

define i32 @main() {
	%1 = call i32 (ptr, ...) @printf(ptr @.str.1, double 9.5)
	ret i32 0
}

