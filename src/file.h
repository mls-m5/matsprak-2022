#pragma once

#include <filesystem>
#include <fstream>
#include <string_view>

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

    const std::filesystem::path &path() {
        return _path;
    }

    std::string_view lineAt(int targetLine) {
        int line = 0;
        int lineStart = 0;
        int lineEnd = 0;
        for (size_t i = 0; i < _content.size(); ++i) {
            auto c = _content.at(i);
            if (c == '\n') {
                lineStart = lineEnd + 1;
                lineEnd = i;
                ++line;
                if (line == targetLine) {
                    return std::string_view(_content.data() + lineStart,
                                            lineEnd - lineStart);
                }
            }
        }
        return {};
    }

private:
    std::string _content;
    std::filesystem::path _path;
};

inline std::unique_ptr<File> openFile(std::filesystem::path path) {
    auto in = std::ifstream{path};
    auto ss = std::ostringstream{};
    ss << in.rdbuf();

    return std::make_unique<File>(path, ss.str());
}
