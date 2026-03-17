#include "movingFiles.hpp"

void movingFiles::findAndMoveFiles(const filesystem::path &source,
                                   const filesystem::path &target,
                                   const string &extension,
                                   const function<void(int, int)>& progress) noexcept {
    int total = 0;
    int current = 0;

    for (const auto &entry : filesystem::recursive_directory_iterator(source))
        if (entry.path().extension() == extension && entry.is_regular_file()) total++;

    for (const auto &entry : filesystem::recursive_directory_iterator(source)) {
        if (entry.path().extension() != extension || !entry.is_regular_file())
            continue;

        filesystem::copy_file(entry.path(), target / entry.path().filename());

        if (progress)
            progress(current++, total);
    }
}
