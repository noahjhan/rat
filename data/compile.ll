
; ModuleID = 'data/compile.ll'
target triple = "arm64-apple-macosx15.0.0"

declare i32 @printf(ptr, ...)

@.str.1 = private unnamed_addr constant [6 x i8] c"start\00", align 1
@.str.2 = private unnamed_addr constant [7 x i8] c"start\0A\00", align 1
@.str.3 = private unnamed_addr constant [5 x i8] c"done\00", align 1
@.str.4 = private unnamed_addr constant [6 x i8] c"done\0A\00", align 1

define i32 @main() {
	%1 = alloca i32, align 4
	%2 = call i32 @printf(ptr @.str.2)
	store i32 10, ptr %1, align 4
	%3 = call i32 @printf(ptr @.str.4)
	ret i32 0
}

