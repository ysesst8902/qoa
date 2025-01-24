#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define QOA_LMS_LEN 4
#define NUM_TESTS 10000000

typedef struct {
    int weights[QOA_LMS_LEN];
    int history[QOA_LMS_LEN];
} qoa_lms_t;

void qoa_lms_update_original(qoa_lms_t *lms, int sample, int residual) {
    int delta = residual >> 4;
    for (int i = 0; i < QOA_LMS_LEN; i++) {
        lms->weights[i] += lms->history[i] < 0 ? -delta : delta;
    }
    for (int i = 0; i < QOA_LMS_LEN - 1; i++) {
        lms->history[i] = lms->history[i + 1];
    }
    lms->history[QOA_LMS_LEN - 1] = sample;
}

// 模擬加速版本（實際測試時用匯編版本替換）
void qoa_lms_update_accelerated(qoa_lms_t *lms, int sample, int residual) {
    // 模擬加速：直接調用原函數，實際應使用加速實現
    qoa_lms_update_original(lms, sample, residual);
}

void generate_random_data(qoa_lms_t *lms, int *sample, int *residual) {
    for (int i = 0; i < QOA_LMS_LEN; i++) {
        lms->weights[i] = rand() % 200 - 100; // 隨機值在 [-100, 100]
        lms->history[i] = rand() % 200 - 100;
    }
    *sample = rand() % 200 - 100;
    *residual = rand() % 200 - 100;
}

int main() {
    qoa_lms_t lms;
    int sample, residual;
    double time_original = 0, time_accelerated = 0;

    srand(time(NULL)); // 初始化隨機數種子

    for (int test = 0; test < NUM_TESTS; test++) {
        generate_random_data(&lms, &sample, &residual);

        clock_t start, end;

        // 測試原始實現
        start = clock();
        qoa_lms_update_original(&lms, sample, residual);
        end = clock();
        time_original += (double)(end - start) / CLOCKS_PER_SEC;

        // 測試加速實現
        start = clock();
        qoa_lms_update_accelerated(&lms, sample, residual);
        end = clock();
        time_accelerated += (double)(end - start) / CLOCKS_PER_SEC;
    }
    printf("Test case numbers:%d\n", NUM_TESTS);
    printf("Original Method: %.6f 秒\n", time_original);
    printf("RVV Optimized Method: %.6f 秒\n", time_accelerated);

    return 0;
}

