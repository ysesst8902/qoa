#include <stdio.h>
#include <stdint.h>

#define ARRAY_LEN 16

// 声明 RVV 优化版本函数
extern void qoa_clamp_s16_rvv(int *src, int *dst, int len);

// 原始的非优化函数
static inline int qoa_clamp_s16(int v) {
    if ((unsigned int)(v + 32768) > 65535) {
        if (v < -32768) { return -32768; }
        if (v >  32767) { return  32767; }
    }
    return v;
}
static inline uint64_t get_cycle_count() {
    uint64_t cycles;
    asm volatile("rdcycle %0" : "=r"(cycles)); // 读取 RISC-V 的周期计数器
    return cycles;
}

void qoa_clamp_s16_c(int *src, int *dst, int len) {
    for (int i = 0; i < len; i++) {
        dst[i] = qoa_clamp_s16(src[i]);
    }
}

int main() {
    // 测试数组
    int src[ARRAY_LEN] = {40000, -40000, 0, 32767, -32768, 100, -100, 12345, -12345, 20000, -20000, 30000, -30000, 32768, -32769, 0};
    int dst_c[ARRAY_LEN] = {0};
    int dst_rvv[ARRAY_LEN] = {0};

    // 测量非优化函数的运行时间
    uint64_t start = get_cycle_count();
    qoa_clamp_s16_c(src, dst_c, ARRAY_LEN);
    uint64_t end = get_cycle_count();
    double time_c = (end - start) / 1e9;

    printf("Non-optimized (C) Time: %.9f seconds\n", time_c);

    // 测量 RVV 优化函数的运行时间
    start = get_cycle_count();
    qoa_clamp_s16_rvv(src, dst_rvv, ARRAY_LEN);
    end = get_cycle_count();
    double time_rvv = (end - start) / 1e9;

    printf("RVV-optimized Time: %.9f seconds\n", time_rvv);

    // 验证两种实现的结果是否一致
    for (int i = 0; i < ARRAY_LEN; i++) {
        printf("C: %d, RVV: %d\n", dst_c[i], dst_rvv[i]);
    }

    return 0;
}
