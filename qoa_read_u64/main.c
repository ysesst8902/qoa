#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

typedef uint64_t qoa_uint64_t;

// 原始函數
static inline qoa_uint64_t qoa_read_u64(const unsigned char *bytes, unsigned int *p) {
    bytes += *p;
    *p += 8;
    return
        ((qoa_uint64_t)(bytes[0]) << 56) | ((qoa_uint64_t)(bytes[1]) << 48) |
        ((qoa_uint64_t)(bytes[2]) << 40) | ((qoa_uint64_t)(bytes[3]) << 32) |
        ((qoa_uint64_t)(bytes[4]) << 24) | ((qoa_uint64_t)(bytes[5]) << 16) |
        ((qoa_uint64_t)(bytes[6]) <<  8) | ((qoa_uint64_t)(bytes[7]) <<  0);
}

// 加速函數接口
extern qoa_uint64_t qoa_read_u64_rvv(const unsigned char *bytes, unsigned int *p);

void generate_test_data(unsigned char *data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        data[i] = rand() % 256; // 隨機生成字節數據
    }
}

int main() {
    const size_t data_size = 2048 * 2048; // 測試數據大小 1MB
    unsigned char *data = (unsigned char *)malloc(data_size);
    unsigned int offset = 0;

    generate_test_data(data, data_size);

    const int num_iterations = 10000000;
    clock_t start, end;
    qoa_uint64_t result;

    // 測試原始函數
    offset = 0;
    printf("Data size: 2048*2048\n");
    printf("Num_iteration:%d\n", num_iterations);
    start = clock();
    for (int i = 0; i < num_iterations; i++) {
        result = qoa_read_u64(data, &offset);
        if (offset >= data_size - 8) offset = 0; // 防止越界
    }
    end = clock();
    double time_original = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Original Method: %.6f 秒\n", time_original);

    // 測試加速函數
    offset = 0;
    start = clock();
    for (int i = 0; i < num_iterations; i++) {
        result = qoa_read_u64_rvv(data, &offset);
        if (offset >= data_size - 8) offset = 0; // 防止越界
    }
    end = clock();
    double time_rvv = (double)(end - start) / CLOCKS_PER_SEC;
    printf("RVV Method: %.6f 秒\n", time_rvv);

    // 清理資源
    free(data);
    return 0;
}
