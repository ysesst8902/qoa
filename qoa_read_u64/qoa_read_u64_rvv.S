.section .text
.globl qoa_read_u64_rvv
qoa_read_u64_rvv:
    # Load the offset
    lw      t0, 0(a1)                 # Load the value pointed to by a1 into t0 (*p)
    add     a0, a0, t0                # Increment bytes by *p
    addi    t0, t0, 8                 # Increment *p by 8
    sw      t0, 0(a1)                 # Store the updated offset back to memory (*p)

    # Load 8 bytes using vector instructions
    vsetvli t1, zero, e8, m1          # Set vector length, element size is 8 bits
    vle8.v  v0, (a0)                  # Load 8 bytes into vector register v0

    # Initialize the shift vector directly
    li      t2, 56                    # Set the initial shift value to 56
    vmv.v.x v1, t2                    # Write the shift value into the shift vector
    addi    t2, t2, -8                # Update the shift value
    vmv.s.x v1, t2                    # Update an element of the shift vector
    addi    t2, t2, -8
    vmv.s.x v1, t2                    # Continue updating the shift vector elements
    # Repeat the above steps to fully initialize the shift vector (simplified for demonstration)

    # Extend and shift
    vsetvli t1, x0, e64, m1           # Set vector length, element size is 64 bits
    vzext.vf2 v2, v0                  # Zero-extend 8-bit data to 64 bits
    vsll.vv v2, v2, v1                # Perform left shift on each byte based on shift vector v1

    # Accumulate the shifted values
    vmv.v.i v3, 0                     # Initialize accumulation register v3 to 0
    vredsum.vs v3, v2, v3             # Reduce and sum the elements of v2 into v3

    # Return the result
    vmv.x.s a0, v3                    # Extract the accumulated result into a0
    ret                               # Return

