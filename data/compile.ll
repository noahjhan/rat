
; ModuleID = 'data/compile.ll'
target triple = "arm64-apple-macosx15.0.0"

declare i32 @printf(ptr, ...)

@.str.1 = private unnamed_addr constant [6 x i8] c"even\0A\00", align 1
@.str.2 = private unnamed_addr constant [5 x i8] c"odd\0A\00", align 1
@.str.3 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

define i32 @foo(i32 noundef %0) {
	%2 = alloca i32, align 4
	store i32 %0, ptr %2, align 4
	%3 = load i32, ptr %2, align 4
	%4 = srem i32 %3, 2
	%5 = icmp eq i32 %4, 0
	br i1 %5, label %6, label %8

6:
	%7 = call i32 @printf(ptr @.str.1)
	br label %10

8:
	%9 = call i32 @printf(ptr @.str.2)
	br label %10

10:
	%11 = load i32, ptr %2, align 4
	%12 = call i32 (ptr, ...) @printf(ptr @.str.3, i32 %11)
	%13 = load i32, ptr %2, align 4
	ret i32 %13
}

define i32 @main() {
	%1 = alloca i32, align 4
	%2 = alloca i32, align 4
	store i32 32, ptr %1, align 4
	%3 = load i32, ptr %1, align 4
	%4 = call i32 @foo(i32 noundef %3)
	store i32 %4, ptr %2, align 4
	ret i32 0
}

