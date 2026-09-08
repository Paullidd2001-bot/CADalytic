#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace cadalytic {

class Part;

struct RebuildResult
{
    bool success = false;
    std::vector<std::uint64_t> rebuiltFeatureIds;
    std::string error;
};

class RebuildEngine
{
public:
    RebuildResult rebuild(Part& part) const;
};

} // namespace cadalytic
