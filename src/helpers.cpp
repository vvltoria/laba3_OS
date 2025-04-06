#include "helpers.h"

int countMarks(const std::vector<int>& arr, int id) {
    int count = 0;
    for (int val : arr) {
        if (val == id)
            count++;
    }
    return count;
}

void cleanupMarks(std::vector<int>& arr, int id) {
    for (int &val : arr) {
        if (val == id)
            val = 0;
    }
}
