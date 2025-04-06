#include "gtest/gtest.h"
#include "helpers.h"
#include <vector>

TEST(HelpersTest, CountMarksEmpty) {
    std::vector<int> arr = {0, 0, 0, 0, 0};
    EXPECT_EQ(countMarks(arr, 1), 0);
}

TEST(HelpersTest, CountMarksNonEmpty) {
    std::vector<int> arr = {1, 2, 1, 0, 1};
    EXPECT_EQ(countMarks(arr, 1), 3);
    EXPECT_EQ(countMarks(arr, 2), 1);
}

TEST(HelpersTest, CleanupMarks) {
    std::vector<int> arr = {1, 2, 1, 0, 1};
    cleanupMarks(arr, 1);
    EXPECT_EQ(countMarks(arr, 1), 0);
    EXPECT_EQ(arr[1], 2);
}
