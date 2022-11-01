#pragma once

#include <filesystem>
#include <fstream>

struct File {
    File() = default;
    File(const File &) = delete;
    File(File &&) = delete;
    File &operator=(const File &) = delete;
    File &operator=(File &&) = delete;
    ~File() = default;
    explicit File(std::filesystem::path path, std::string content)
        : _path{path}
        , _content{std::move(content)} {}

    std::string_view content() const {
        return _content;
    }

private:
    std::string _content;
    std::filesystem::path _path;
};

std::unique_ptr<File> openFile(std::filesystem::path path) {
    auto in = std::ifstream{path};
    auto ss = std::ostringstream{};
    ss << in.rdbuf();

    return std::make_unique<File>(path, ss.str());
}
