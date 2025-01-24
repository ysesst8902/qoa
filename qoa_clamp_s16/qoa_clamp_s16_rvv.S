.globl qoa_clamp_s16_rvv

qoa_clamp_s16_rvv:
    li t0, 32767               # Upper limit 32767
    li t1, -32768              # Lower limit -32768

    # Set vector length (done once)
    vsetvli t2, a2, e32        # Set vector length for 32-bit elements
1:
    vle32.v v0, (a0)           # Load source array into vector register v0
    vmax.vx v1, v0, t1         # v1 = max(v0, -32768) (clamp to lower limit)
    vmin.vx v2, v1, t0         # v2 = min(v1, 32767) (clamp to upper limit)
    vse32.v v2, (a1)           # Store result from v2 to destination array

    slli t3, t2, 2             # t3 = t2 * 4 bytes (each element is 4 bytes)
    add a0, a0, t3             # Update source array pointer
    add a1, a1, t3             # Update destination array pointer
    sub a2, a2, t2             # Update remaining length
    bnez a2, 1b                # If remaining length is not zero, repeat loop

    ret                        # Return

