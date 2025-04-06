#include "marker.h"
#include <iostream>
#include <thread>
#include <cstdlib>
#include <chrono>

void markerThread(int id, SharedData* shared, MarkerControl* control) {
    {
        std::unique_lock<std::mutex> lock(shared->start_mutex);
        shared->cv_start.wait(lock, [shared] { return shared->start_flag; });
    }
    
    srand(id);
    
    while (true) {
        int index = rand() % shared->arr.size();
        bool canMark = false;
        
        {
            std::unique_lock<std::mutex> lock(shared->arr_mutex);
            if (shared->arr[index] == 0) {
                canMark = true;
            }
        }
        
        if (canMark) {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            {
                std::unique_lock<std::mutex> lock(shared->arr_mutex);
                if (shared->arr[index] == 0) {
                    shared->arr[index] = id;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            continue;
        } else {
            int count = 0;
            {
                std::unique_lock<std::mutex> lock(shared->arr_mutex);
                for (int val : shared->arr) {
                    if (val == id)
                        count++;
                }
            }
            std::cout << "Marker " << id 
                      << ": Marked count = " << count 
                      << ", failed index = " << index << std::endl;
            
            {
                std::unique_lock<std::mutex> lock(shared->wait_mutex);
                shared->waiting_count++;
                shared->cv_all_waiting.notify_one();
            }
            
            std::unique_lock<std::mutex> lock(control->m);
            control->cv.wait(lock, [control] { return control->resume || control->terminate; });
            
            if (control->terminate) {
                {
                    std::unique_lock<std::mutex> arrLock(shared->arr_mutex);
                    for (auto &val : shared->arr) {
                        if (val == id)
                            val = 0;
                    }
                }
                break;
            } else {
                control->resume = false;
            }
        }
    }
}
