#ifndef MARKER_H
#define MARKER_H

#include "shared_data.h"
#include <mutex>
#include <condition_variable>

struct MarkerControl {
    std::mutex m;
    std::condition_variable cv;
    bool resume = false;
    bool terminate = false;
};

void markerThread(int id, SharedData* shared, MarkerControl* control);

#endif // MARKER_H
