#include "movingFiles.hpp"

void movingFiles::findListFiles(const filesystem::path &directory, const string &extension) {
    for (const auto& entry : filesystem::recursive_directory_iterator (directory)) {
        if (entry.path().extension() == extension && entry.is_regular_file())
            files.push_back(entry.path());
    }
}

void movingFiles::moveFiles(const filesystem::path &directory) {
    for (auto file : files) {
        filesystem::path target = directory / file.filename();

        if (filesystem::exists(target))
            continue;

        filesystem::copy_file(file, target);
    }
}
