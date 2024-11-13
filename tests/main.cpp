//
// Created by Jonas on 8/03/2024.
//

#include "gtest/gtest.h"
#ifdef _WIN32
#include <windows.h>
#endif

int main(int argc, char** argv)
{
#ifdef _WIN32
    if (IsDebuggerPresent())
        testing::GTEST_FLAG(break_on_failure) = true;
#endif
    //printf("Running main() from %s\n", __FILE__);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}