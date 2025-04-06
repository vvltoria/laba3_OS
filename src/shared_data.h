
#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include <vector>
#include <mutex>
#include <condition_variable>

struct SharedData {
    std::vector<int> arr;
    std::mutex arr_mutex;

    std::mutex start_mutex;
    std::condition_variable cv_start;
    bool start_flag = false;

    std::mutex wait_mutex;
    std::condition_variable cv_all_waiting;
    int waiting_count = 0;
};

#endif // SHARED_DATA_H
