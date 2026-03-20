#include "movingFiles.hpp"

void movingFiles::findAndMoveFiles(const filesystem::path &source,
                                   const filesystem::path &target,
                                   const vector<string>& extension,
                                   const function<void(int, int)>& progress) noexcept {
    int total = 0;
    int current = 0;

    for (const auto &ext: extension)
        for (const auto &entry: filesystem::recursive_directory_iterator(source))
            if (entry.is_regular_file() && entry.path().extension().string() == ext)
                total++;

    for (const auto &ext : extension) {
        for (const auto &entry : filesystem::recursive_directory_iterator(source)) {
            if (!entry.is_regular_file() || entry.is_symlink() || entry.path().extension().string() != ext) {
                std::cout << entry.path().filename() << " skipped: not a regular file or symlink or wrong extension\n";
                continue;
            }

            if (filesystem::equivalent(entry.path().parent_path(), target)) {
                std::cout << entry.path().filename() << " skipped: file is already in target\n";
                continue;
            }

            filesystem::path dest = target / entry.path().filename();

            if (filesystem::exists(dest)) {
                std::cout << entry.path().filename() << " skipped: file with same name already exists in target\n";
                continue;
            }

            if (filesystem::space(target).available < filesystem::file_size(entry.path())) {
                std::cout << entry.path().filename() << " skipped: not enough disk space\n";
                continue;
            }

            filesystem::copy_file(entry.path(), dest);

            if (progress)
                progress(current++, total);
        }
    }
}
