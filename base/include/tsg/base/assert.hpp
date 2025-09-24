#ifndef TSG_BASE_ASSERT_HPP
#define TSG_BASE_ASSERT_HPP

#ifndef NDEBUG
// see : https://thesoftwarecondition.com/blog/2010/08/01/cc-assertions-and-defining-your-own-assert-macro/
#include <assert.h>
#include <iostream>
#define TSG_ASSERT(condition, message)                                                                     \
    {                                                                                                        \
        if (!(condition)) {                                                                                  \
            std::cerr << "Assertion failed at " << __FILE__ << ":" << __LINE__;                              \
            std::cerr << " inside " << __FUNCTION__ << std::endl;                                            \
            std::cerr << "  condition: " << #condition << std::endl;                                         \
            std::cerr << "  message: \"" << message << "\"" << std::endl;                                    \
            abort();                                                                                         \
        }                                                                                                    \
    }
#else
#define TSG_ASSERT(condition, message) (condition)
#endif

#endif // TSG_BASE_ASSERT_HPP