#pragma once

#include <filesystem>
#include <functional>
#include <vector>
#include <iostream>

using namespace std;

class movingFiles {
public:
    static void findAndMoveFiles(filesystem::path source,
                        filesystem::path target,
                        const vector<string>& extension,
                        const function<void(int, int)>& progress) noexcept;
#ifdef __linux__
    static filesystem::path resolveVirtualPath(const filesystem::path &path);
#endif
};