#include "movingFiles.hpp"

#include <algorithm>
static string toLower(string s) {
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

void movingFiles::findAndMoveFiles(filesystem::path source,
                                   filesystem::path target,
                                   const vector<string>& extension,
                                   const function<void(int, int)>& progress) noexcept {
#ifdef __linux__
    source = resolveVirtualPath(source);
    target = resolveVirtualPath(target);
#endif
    int total = 0;
    int current = 0;

    for (const auto &ext: extension)
        for (const auto &entry: filesystem::recursive_directory_iterator(source))
            if (entry.is_regular_file() && toLower(entry.path().extension().string()) == ext)
                total++;

    for (const auto &ext : extension) {
        for (const auto &entry : filesystem::recursive_directory_iterator(source)) {
            if (!entry.is_regular_file() || entry.is_symlink() || toLower(entry.path().extension().string()) != ext)
                continue;

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

#ifdef __linux__
#include <unistd.h>

filesystem::path movingFiles::resolveVirtualPath(const filesystem::path &path) {
    const string s = path.string();

    if (s.find("gphoto2://") == 0) {
        string host = s.substr(10);
        if (!host.empty() && host.back() == '/')
            host.pop_back();
        return "/run/user/" + to_string(getuid()) + "/gvfs/gphoto2:host=" + host;
    }
    if (s.find("mtp://") == 0) {
        string host = s.substr(6);
        if (!host.empty() && host.back() == '/')
            host.pop_back();
        return "/run/user/" + to_string(getuid()) + "/gvfs/mtp:host=" + host;
    }

    return path;
}
#endif
