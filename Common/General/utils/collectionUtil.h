#pragma once

namespace col
{
    template <typename Collection, typename Value>
    bool contains(const Collection& collection, const Value& value)
    {
        return (collection.find(value) != collection.end());
    }
}
