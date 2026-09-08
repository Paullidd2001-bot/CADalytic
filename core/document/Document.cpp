#include "Document.h"

#include <utility>

namespace cadalytic {

Document::Document(std::string name)
    : m_name(std::move(name))
{
}

Part& Document::addPart(std::string name)
{
    auto part = std::make_unique<Part>(m_nextPartId++, std::move(name));
    m_parts.push_back(std::move(part));
    return *m_parts.back();
}

} // namespace cadalytic
