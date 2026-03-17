#pragma once

#include <filesystem>
#include <functional>
#include <vector>

using namespace std;

class movingFiles {
public:
    static void findAndMoveFiles(const filesystem::path &source,
                        const filesystem::path &target,
                        const string &extension,
                        const function<void(int, int)>& progress) noexcept;
};