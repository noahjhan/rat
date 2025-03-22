
; ModuleID = 'data/compile.ll'
target triple = "arm64-apple-macosx15.0.0"

declare i32 @printf(ptr)

@.str1 = private unnamed_addr constant [7 x i8] c"here!\0A\00", align 1
@.str2 = private unnamed_addr constant [7 x i8] c"there\0A\00", align 1

define i32 @main() {
	%1 = alloca i64, align 8
	%2 = alloca i32, align 4
	store i64 10, ptr %1, align 8
	store i32 10, ptr %2, align 4
	call i32 @printf(ptr @.str1)
	call i32 @printf(ptr @.str2)
	ret i32 0
}

define void @foo(i32 noundef %0, i32 noundef %1) {
	ret void
}

