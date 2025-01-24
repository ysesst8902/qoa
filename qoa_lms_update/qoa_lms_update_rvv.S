.section .text
.globl qoa_lms_update_rvv
qoa_lms_update_rvv:
    # a0 = pointer to lms structure, a1 = sample, a2 = residual
    # Set vector length to QOA_LMS_LEN (assuming QOA_LMS_LEN = 4)
    li      t0, 4                   # t0 = QOA_LMS_LEN
    vsetvli t0, t0, e32             # Set vector length to 4, with element width 32 bits

    # delta = residual >> 4
    srai t1, a2, 4                  # t1 = residual >> 4

    # Load weights and history
    vle32.v v1, (a0)                # Load weights into vector register v1
    addi t3, a0, 16                 # t3 points to the history (assuming history follows weights in memory)
    vle32.v v2, (t3)                # Load history into vector register v2

    # Broadcast delta and calculate -delta
    vmv.v.x v3, t1                  # Broadcast delta to all elements in vector register v3
    vneg.v v4, v3                   # Calculate -delta and store in vector register v4

    # Compare history < 0, generate mask
    vmslt.vi v0, v2, 0              # Mask register v0 = (v2 < 0)

    # Select -delta or delta based on mask
    vmerge.vvm v5, v3, v4, v0       # v5 = (v2 < 0) ? -delta : delta

    # Update weights
    vadd.vv v1, v1, v5              # v1 = v1 + v5

    # Store updated weights
    vse32.v v1, (a0)                # Save v1 back to weights in memory

    # Shift history array (loop unrolling because QOA_LMS_LEN = 4 is small)
    lw t2, 20(a0)                   # t2 = lms->history[1]
    sw t2, 16(a0)                   # lms->history[0] = lms->history[1]

    lw t2, 24(a0)                   # t2 = lms->history[2]
    sw t2, 20(a0)                   # lms->history[1] = lms->history[2]

    lw t2, 28(a0)                   # t2 = lms->history[3]
    sw t2, 24(a0)                   # lms->history[2] = lms->history[3]

    # Set lms->history[QOA_LMS_LEN - 1] = sample
    sw a1, 28(a0)                   # lms->history[3] = sample

    # Return
    ret

