#include "movingFiles.hpp"

void movingFiles::findAndMoveFiles(const filesystem::path &source,
                                   const filesystem::path &target,
                                   const vector<string>& extension,
                                   const function<void(int, int)>& progress) noexcept {
    int total = 0;
    int current = 0;

    for (const auto &ext : extension)
        for (const auto &entry : filesystem::recursive_directory_iterator(source))
            if (entry.is_regular_file() && entry.path().extension().string() == ext)
                total++;

    for (const auto &ext : extension) {
        for (const auto &entry : filesystem::recursive_directory_iterator(source)) {
            if (!entry.is_regular_file() || entry.path().extension().string() != ext)
                continue;

            filesystem::copy_file(entry.path(), target / entry.path().filename());

            if (progress)
                progress(current++, total);
        }
    }
}
