
; ModuleID = 'data/compile.ll'
target triple = "arm64-apple-macosx15.0.0"

declare i32 @printf(ptr, ...)

@.str.1 = private unnamed_addr constant [3 x i8] c"%f\00", align 1

define i32 @main() {
	%1 = alloca i32, align 4
	%2 = alloca float, align 4
	store i32 15, ptr %1, align 4
	%3 = load i32, ptr %1, align 4
	%4 = sitofp i32 %3 to float
	store float %4, float* %2, align 4
	%5 = load float, ptr %2, align 4
	%6 = fpext float %5 to double
	%7 = call i32 (ptr, ...) @printf(ptr @.str.1, double %6)
	ret i32 0
}

