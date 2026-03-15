#pragma once

#include <filesystem>

using namespace std;

class pathChecker {
public:
    explicit pathChecker(const filesystem::path& path):
    m_path_(path),
    m_status_(filesystem::status(path)),
    m_link_status_(filesystem::symlink_status(path)) {}

    pathChecker& exits () {
        if (m_status_.type() == filesystem::file_type::not_found)
            throw std::runtime_error("Does not exists");
        return *this;
    }
    pathChecker& isFile () {
        if (m_status_.type() != filesystem::file_type::regular)
            throw std::runtime_error("Does not file");
        return *this;
    }
    pathChecker& isDirectory () {
        if (m_status_.type() != filesystem::file_type::directory)
            throw std::runtime_error("Does not directory");
        return *this;
    }
    pathChecker& isLink() {
        if (m_link_status_.type() == filesystem::file_type::symlink)
            throw std::runtime_error("Path is a symlink");
        return *this;
    }
    pathChecker& isKnown() {
        if (m_status_.type() == filesystem::file_type::unknown)
            throw std::runtime_error("Path type is unknown");
        return *this;
    }
    pathChecker& isReadable() {
#ifdef _WIN32
        DWORD attrs = GetFileAttributesW(m_path_.wstring().c_str());
        if (attrs == INVALID_FILE_ATTRIBUTES)
            throw std::runtime_error("Path is not readable");
#elif __linux__ || __APPLE__
        if (access(m_path_.string().c_str(), R_OK) != 0)
            throw std::runtime_error("Path is not readable");
#else
#error "Unsupported platform"
#endif
        return *this;
    }

    pathChecker& isWritable() {
#ifdef _WIN32
        DWORD attrs = GetFileAttributesW(m_path_.wstring().c_str());
        if (attrs == INVALID_FILE_ATTRIBUTES || (attrs & FILE_ATTRIBUTE_READONLY))
            throw std::runtime_error("Path is not writable");
#elif __linux__ || __APPLE__
        if (access(m_path_.string().c_str(), W_OK) != 0)
            throw std::runtime_error("Path is not writable");
#else
#error "Unsupported platform"
#endif
        return *this;
    }
private:
    filesystem::path m_path_;
    filesystem::file_status m_status_;
    filesystem::file_status m_link_status_;
};