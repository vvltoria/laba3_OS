#include "shared_data.h"
#include "marker.h"
#include <iostream>
#include <thread>
#include <vector>
#include <memory>
#include <algorithm>

int main() {
    int arraySize = 0;
    std::cout << "Enter array size: ";
    std::cin >> arraySize;
    if (arraySize <= 0) {
        std::cerr << "Array size must be positive." << std::endl;
        return 1;
    }
    
    SharedData shared;
    shared.arr.resize(arraySize, 0);
    
    int numMarkers = 0;
    std::cout << "Enter number of marker threads: ";
    std::cin >> numMarkers;
    if (numMarkers <= 0) {
        std::cerr << "Number of marker threads must be positive." << std::endl;
        return 1;
    }
    
    std::vector<std::thread> markerThreads;
    std::vector<std::unique_ptr<MarkerControl>> controls;
    
    for (int i = 1; i <= numMarkers; ++i) {
        controls.push_back(std::make_unique<MarkerControl>());
        markerThreads.push_back(std::thread(markerThread, i, &shared, controls.back().get()));
    }
    
    {
        std::unique_lock<std::mutex> lock(shared.start_mutex);
        shared.start_flag = true;
        shared.cv_start.notify_all();
    }
    
    std::vector<int> activeMarkers;
    for (int i = 0; i < numMarkers; ++i) {
        activeMarkers.push_back(i);
    }
    
    while (!activeMarkers.empty()) {
        {
            std::unique_lock<std::mutex> lock(shared.wait_mutex);
            shared.cv_all_waiting.wait(lock, [&shared, &activeMarkers] {
                return shared.waiting_count >= static_cast<int>(activeMarkers.size());
            });
        }
        
        {
            std::unique_lock<std::mutex> lock(shared.arr_mutex);
            std::cout << "Array contents: ";
            for (int val : shared.arr) {
                std::cout << val << " ";
            }
            std::cout << std::endl;
        }
        
        int terminateMarkerId;
        std::cout << "Enter marker thread id to terminate: ";
        std::cin >> terminateMarkerId;
        
        int indexToRemove = -1;
        for (size_t i = 0; i < activeMarkers.size(); ++i) {
            int markerIndex = activeMarkers[i];
            if (markerIndex + 1 == terminateMarkerId) {
                indexToRemove = markerIndex;
                break;
            }
        }
        if (indexToRemove == -1) {
            std::cout << "Invalid marker thread id. Please try again." << std::endl;
            {
                std::unique_lock<std::mutex> lock(shared.wait_mutex);
                shared.waiting_count = 0;
            }
            for (int idx : activeMarkers) {
                std::unique_lock<std::mutex> lock(controls[idx]->m);
                controls[idx]->resume = true;
                controls[idx]->cv.notify_one();
            }
            continue;
        }
        
        {
            std::unique_lock<std::mutex> lock(controls[indexToRemove]->m);
            controls[indexToRemove]->terminate = true;
            controls[indexToRemove]->cv.notify_one();
        }
        
        markerThreads[indexToRemove].join();
        
        std::cout << "Marker thread " << terminateMarkerId << " terminated." << std::endl;
        
        activeMarkers.erase(std::remove(activeMarkers.begin(), activeMarkers.end(), indexToRemove), activeMarkers.end());
        
        {
            std::unique_lock<std::mutex> lock(shared.wait_mutex);
            shared.waiting_count = 0;
        }
        
        for (int idx : activeMarkers) {
            std::unique_lock<std::mutex> lock(controls[idx]->m);
            controls[idx]->resume = true;
            controls[idx]->cv.notify_one();
        }
    }
    
    std::cout << "All marker threads have terminated. Final array:" << std::endl;
    {
        std::unique_lock<std::mutex> lock(shared.arr_mutex);
        for (int val : shared.arr) {
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }
    
    return 0;
}
