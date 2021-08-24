#pragma once

#include <sstream>

namespace str
{
    template <typename Iterable>
    std::string join(const Iterable& iterable, std::string_view sep)
    {
        std::ostringstream oss;
        for (const auto& elem : iterable)
        {
            oss << elem;
            if (&elem != &iterable.back())
            {
                oss << sep;
            }
        }
        return oss.str();
    }
}
