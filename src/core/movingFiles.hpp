#pragma once

#include <filesystem>
#include <vector>

using namespace std;

class movingFiles {
public:
    void findListFiles (const filesystem::path& directory, const string& extension);
    void moveFiles (const filesystem::path& directory);
private:
    vector<filesystem::path> files;
};