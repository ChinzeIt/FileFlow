#include "controller.hpp"

void controller::start(const string &from, const string &to, const string &format, Callback callback) {
    cancelled = false;

    fileThread = thread([this, from, to, format, callback] {
        movingFiles::findAndMoveFiles(from, to, format, callback.progressWork);
        if (!cancelled && callback.successFinished)
            callback.successFinished();
    });

    fileThread.detach();
}

void controller::cancel() {
    cancelled = true;
}
