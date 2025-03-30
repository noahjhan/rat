
; ModuleID = 'data/compile.ll'
target triple = "arm64-apple-macosx15.0.0"

declare i32 @printf(ptr, ...)


define i32 @main() {
	%1 = alloca float, align 4
	%2 = alloca float, align 4
	store float 40.0, ptr %1, align 4
	%3 = load float, ptr %1, align 4
	%4 = fsub float %3, 10.0
	store float %4, ptr %2, align 4
	ret i32 0
}

