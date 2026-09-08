#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "Part.h"

namespace cadalytic {

class Document
{
public:
    explicit Document(std::string name = {});

    const std::string& name() const { return m_name; }
    void setName(std::string name) { m_name = std::move(name); }

    Part& addPart(std::string name);
    const std::vector<std::unique_ptr<Part>>& parts() const { return m_parts; }

private:
    std::string m_name;
    std::uint64_t m_nextPartId = 1;
    std::vector<std::unique_ptr<Part>> m_parts;
};

} // namespace cadalytic
