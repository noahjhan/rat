
; ModuleID = 'data/compile.ll'
target triple = "arm64-apple-macosx15.0.0"

declare i32 @printf(ptr)

@.str1 = private unnamed_addr constant [7 x i8] c"here!\0A\00", align 1
define i32 @main() {
	%1 = alloca i64, align 8
	store i64 10, ptr %1, align 8
	call i32 @printf(ptr @.str1)
	ret i32 0
}

define void @foo() {
	ret void
}

