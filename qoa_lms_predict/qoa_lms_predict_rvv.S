.section .text
.globl qoa_lms_predict_rvv

qoa_lms_predict_rvv:
    li t0, 8                # Set QOA_LMS_LEN constant to 8
    li t1, 0                # Initialize prediction to 0

    # Load addresses of lms->weights and lms->history
    mv t2, a0               # The address of the LMS structure is stored in a0
    addi t3, t2, 0          # t3 = address of lms->weights (offset 0 from the start of the structure)
    addi t4, t2, 32         # t4 = address of lms->history (weights array occupies 32 bytes)

    # Configure vector registers
    vsetvli t5, t0, e32     # Set vector length with element size of 32 bits
    vle32.v v0, (t3)        # Load weights into vector register v0
    vle32.v v1, (t4)        # Load history into vector register v1

    # Perform dot product operation
    vmul.vv v2, v0, v1      # v2 = element-wise multiplication of v0 and v1
    vredsum.vs v3, v2, v0   # v3 = reduction sum of v2

    # Extract the result into t1
    vmv.x.s t1, v3          # Copy the scalar value from v3 to t1

    # Right-shift by 13 bits to scale the result
    srai t1, t1, 13

    # Return the result
    mv a0, t1
    ret
