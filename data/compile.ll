
; ModuleID = 'data/compile.ll'
target triple = "arm64-apple-macosx15.0.0"

declare i32 @printf(ptr)

@.str.1 = private unnamed_addr constant [9 x i8] c"help me\0A\00", align 1

define void @printer() {
	call i32 @printf(ptr @.str.1)
	call void @printer()
	ret void
}

define i32 @main() {
	%1 = alloca i32, align 4
	%2 = sdiv i32 345, 45
	%3 = srem i32 %2, 345
	%4 = shl i32 %3, 648
	store i32 %4, ptr %1, align 4
	ret i32 0
}

