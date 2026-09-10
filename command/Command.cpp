#include "Command.h"

namespace cadalytic {

namespace {
std::uint64_t s_nextCommandId = 1;
}

Command::Command(std::string name)
    : m_id(s_nextCommandId++),
      m_name(std::move(name))
{
}

} // namespace cadalytic