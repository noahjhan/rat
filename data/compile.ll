
; ModuleID = 'data/compile.ll'
target triple = "arm64-apple-macosx15.0.0"

declare i32 @printf(ptr, ...)

@.str.1 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@.str.2 = private unnamed_addr constant [6 x i8] c"even\0A\00", align 1
@.str.3 = private unnamed_addr constant [5 x i8] c"odd\0A\00", align 1
@.str.4 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

define i32 @foo(i32 noundef %0) {
	%2 = alloca i32, align 4
	store i32 %0, ptr %2, align 4
	%3 = load i32, ptr %2, align 4
	%4 = call i32 (ptr, ...) @printf(ptr @.str.1, i32 %3)
	%5 = load i32, ptr %2, align 4
	%6 = srem i32 %5, 2
	%7 = icmp eq i32 %6, 0
	br i1 %7, label %8, label %10

8:
	%9 = call i32 @printf(ptr @.str.2)
	br label %12

10:
	%11 = call i32 @printf(ptr @.str.3)
	br label %12

12:
	%13 = load i32, ptr %2, align 4
	%14 = icmp eq i32 %13, 0
	br i1 %14, label %15, label %16

15:
	ret i32 0
16:
	br label %17

17:
	%18 = load i32, ptr %2, align 4
	%19 = sub i32 %18, 1
	%20 = call i32 @foo(i32 noundef %19)
	ret i32 %20
}

define i32 @main() {
	%1 = alloca i32, align 4
	%2 = alloca i32, align 4
	%3 = call i32 (ptr, ...) @printf(ptr @.str.4, i32 4)
	store i32 32, ptr %1, align 4
	%4 = load i32, ptr %1, align 4
	%5 = call i32 @foo(i32 noundef %4)
	store i32 %5, ptr %2, align 4
	ret i32 0
}

