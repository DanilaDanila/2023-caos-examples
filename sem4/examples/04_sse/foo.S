.intel_syntax noprefix

.global do_smth

// double (double x, double y)
// xmm0 := x
// xmm1 := y
do_smth:
    // x /= y
    divsd xmm0, xmm1
    ret
