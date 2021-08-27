#pragma once

#define RETURN_IF(CONDITION_, RETURN_VALUE)                                              \
    {                                                                                    \
        if (CONDITION_)                                                                  \
            return RETURN_VALUE;                                                         \
    }
#define RETURN_IF_MESSAGE(CONDITION_, RETURN_VALUE, MESSAGE_)                            \
    {                                                                                    \
        if (CONDITION_)                                                                  \
        {                                                                                \
            std::cout << MESSAGE_ << std::endl;                                          \
            return RETURN_VALUE;                                                         \
        }                                                                                \
    }
