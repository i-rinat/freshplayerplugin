#ifndef FPP_STATIC_ASSERT_H
#define FPP_STATIC_ASSERT_H


#define STATIC_ASSERT_HELPER1(expr, msg)    typedef char static_assertion_##msg[(!!(expr))*2 - 1]
#define STATIC_ASSERT_HELPER2(expr, msg)    STATIC_ASSERT_HELPER1(expr, msg)
#define STATIC_ASSERT(expr)                 STATIC_ASSERT_HELPER2(expr, __LINE__)


#endif // FPP_STATIC_ASSERT_H
