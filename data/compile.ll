
; ModuleID = 'data/compile.ll'
target triple = "arm64-apple-macosx15.0.0"

declare i32 @printf(ptr, ...)

@.str.1 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

define i32 @fibbonacci_sequence(i32 noundef %0, i32 noundef %1, i32 noundef %2) {
	%4 = alloca i32, align 4
	%5 = alloca i32, align 4
	%6 = alloca i32, align 4
	store i32 %0, ptr %4, align 4
	store i32 %1, ptr %5, align 4
	store i32 %2, ptr %6, align 4
	%7 = load i32, ptr %4, align 4
	%8 = icmp eq i32 %7, 0
	br i1 %8, label %9, label %10

9:
	ret i32 0
10:
	br label %11

11:
	%12 = load i32, ptr %5, align 4
	%13 = call i32 (ptr, ...) @printf(ptr @.str.1, i32 %12)
	%14 = load i32, ptr %4, align 4
	%15 = sub i32 %14, 1
	%16 = load i32, ptr %6, align 4
	%17 = load i32, ptr %5, align 4
	%18 = add i32 %16, %17
	%19 = load i32, ptr %5, align 4
	%20 = call i32 @fibbonacci_sequence(i32 noundef %15,i32 noundef %18,i32 noundef %19)
	ret i32 %20
}

define i32 @main() {
	%1 = alloca i32, align 4
	store i32 20, ptr %1, align 4
	%2 = load i32, ptr %1, align 4
	%3 = call i32 @fibbonacci_sequence(i32 noundef %2,i32 noundef 0,i32 noundef 1)
	ret i32 0
}

