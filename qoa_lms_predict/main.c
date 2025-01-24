#include <stdio.h>
#include <stdint.h>

// define QOA_LMS_LEN value is 8
#define QOA_LMS_LEN 8

typedef struct {
    int weights[QOA_LMS_LEN];
    int history[QOA_LMS_LEN];
} qoa_lms_t;

// define rvv function
extern int qoa_lms_predict_rvv(qoa_lms_t *lms);

// origin qoa_lms_predict function
int qoa_lms_predict(qoa_lms_t *lms) {
    int prediction = 0;
    for (int i = 0; i < QOA_LMS_LEN; i++) {
        prediction += lms->weights[i] * lms->history[i];
    }
    return prediction >> 13;
}

static inline uint64_t get_cycle_count() {
    uint64_t cycles;
    asm volatile("rdcycle %0" : "=r"(cycles));
    return cycles;
}

int main() {
    
    const double cpu_frequency_hz = 1e9;

    qoa_lms_t lms = {
        .weights = {1, 2, 3, 4, 5, 6, 7, 8},
        .history = {8, 7, 6, 5, 4, 3, 2, 1}
    };

    uint64_t start = get_cycle_count();
    int result = qoa_lms_predict(&lms);
    uint64_t end = get_cycle_count();
    double time_seconds = (end - start) / cpu_frequency_hz;
    printf("Non-optimized result: %d, Time: %.9f seconds\n", result, time_seconds);

    start = get_cycle_count();
    int result_rvv = qoa_lms_predict_rvv(&lms);
    end = get_cycle_count();
    time_seconds = (end - start) / cpu_frequency_hz;
    printf("RVV-optimized result: %d, Time: %.9f seconds\n", result_rvv, time_seconds);

    return 0;
}
