#pragma once

#include <functional>
#include <thread>
#include <string>

#include "../core/movingFiles.hpp"

using namespace std;

class controller {
public:
    struct Callback {
        function<void()> successFinished;
        function<void(int, int)> progressWork;
    };

    void start(const string& from,
                const string& to,
                const string& format,
                Callback callback);

    void cancel();
private:
    thread fileThread;
    atomic<bool> cancelled = false; // Need realization in UI qt framework
};