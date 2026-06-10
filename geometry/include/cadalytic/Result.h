#pragma once
#include <string>
#include <optional>

namespace cadalytic {

template<typename T>
struct Result {
    T value;
    bool ok;
    std::string error;

    static Result<T> success(const T& v) {
        return {v, true, ""};
    }

    static Result<T> failure(const std::string& msg) {
        return {{}, false, msg};
    }
};

} // namespace cadalytic
