#include <gtest/gtest.h>
#include "PasswordHasher.hpp"

TEST(PasswordHasherTest, VerifyCorrectPassword) {
    std::string hash = PasswordHasher::hash("myPassword123");
    EXPECT_TRUE(PasswordHasher::verify("myPassword123", hash));
}

TEST(PasswordHasherTest, RejectWrongPassword) {
    std::string hash = PasswordHasher::hash("myPassword123");
    EXPECT_FALSE(PasswordHasher::verify("wrongPassword", hash));
}

TEST(PasswordHasherTest, SameSaltNeverRepeats) {
    // מוודא שה-salt אקראי ולכן שתי קריאות לאותה סיסמה יפיקו תוצאות שונות
    EXPECT_NE(PasswordHasher::hash("abc"), PasswordHasher::hash("abc"));
}