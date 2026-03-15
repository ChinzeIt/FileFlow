#pragma once

#include <filesystem>
#include <vector>

using namespace std;

class movingFiles {
public:
    static void findAndMoveFiles(const filesystem::path &source,
                        const filesystem::path &target,
                        const string &extension) noexcept;
};