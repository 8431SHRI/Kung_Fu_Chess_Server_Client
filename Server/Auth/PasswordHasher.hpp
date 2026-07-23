#pragma once

#include <string>
#include <vector>

/*
 * @class PasswordHasher
 * @brief גיבוב ואימות סיסמאות באמצעות PBKDF2-HMAC-SHA256 עם salt אקראי (OpenSSL).
 * Purpose: מבטיח שסיסמה גולמית לעולם לא נשמרת/מועברת בטקסט גלוי (חוק גלובלי #6).
 * Contract: hash()/verify() הם כל מה ש-AuthService מכיר. מעבר עתידי ל-bcrypt/argon2
 *           דורש שינוי רק בקובץ הזה.
 */
class PasswordHasher
{
public:
    // מחזיר מחרוזת יחידה שמקודדת את האלגוריתם, מספר האיטרציות, ה-salt וה-hash
    static std::string hash(const std::string& password);

    static bool verify(const std::string& password, const std::string& storedHash);

private:
    static constexpr int kIterations = 100000;
    static constexpr int kSaltBytes = 16;
    static constexpr int kHashBytes = 32;

    static std::string toHex(const unsigned char* data, size_t len);

    static std::vector<unsigned char> fromHex(const std::string& hex);

    static std::vector<unsigned char> pbkdf2(
        const std::string& password,
        const std::vector<unsigned char>& salt,
        int iterations,
        int keyLen);
};