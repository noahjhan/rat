
; ModuleID = 'data/compile.ll'
target triple = "arm64-apple-macosx15.0.0"

declare i32 @printf(ptr, ...)


define i32 @main() {
	%1 = alloca double, align 8
	%2 = alloca float, align 4
	store double 3.400000e+00, double* %1, align 8
	store float 3.4000000953674316e+00, float* %2, align 4
	ret i32 0
}

