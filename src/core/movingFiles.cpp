#include "movingFiles.hpp"

void movingFiles::findAndMoveFiles(const filesystem::path &source,
                                   const filesystem::path &target,
                                   const string &extension) noexcept {
    for (const auto &entry : filesystem::recursive_directory_iterator(source)) {
        if (entry.path().extension() != extension || !entry.is_regular_file())
            continue;

        filesystem::copy_file(entry.path(), target / entry.path().filename());
    }
}
