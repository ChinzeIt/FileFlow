#pragma once

#include <filesystem>
#include <functional>
#include <vector>

using namespace std;

class movingFiles {
public:
    static void findAndMoveFiles(const filesystem::path &source,
                        const filesystem::path &target,
                        const vector<string>& extension,
                        const function<void(int, int)>& progress) noexcept;
};