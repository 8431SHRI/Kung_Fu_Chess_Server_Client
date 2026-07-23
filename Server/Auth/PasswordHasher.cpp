#include "PasswordHasher.hpp"

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <vector>
#include <sstream>
#include <iomanip>
#include <stdexcept>

namespace
{
    constexpr const char* kAlgoTag = "pbkdf2_sha256";
}

std::string PasswordHasher::toHex(const unsigned char* data, size_t len)
{
    std::ostringstream oss;
    for (size_t i = 0; i < len; ++i)
    {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]);
    }
    return oss.str();
}

std::vector<unsigned char> PasswordHasher::fromHex(const std::string& hex)
{
    std::vector<unsigned char> bytes(hex.size() / 2);
    for (size_t i = 0; i < bytes.size(); ++i)
    {
        bytes[i] = static_cast<unsigned char>(std::stoi(hex.substr(i * 2, 2), nullptr, 16));
    }
    return bytes;
}

std::vector<unsigned char> PasswordHasher::pbkdf2(
    const std::string& password,
    const std::vector<unsigned char>& salt,
    int iterations,
    int keyLen)
{
    std::vector<unsigned char> out(keyLen);

    int ok = PKCS5_PBKDF2_HMAC(
        password.c_str(), static_cast<int>(password.size()),
        salt.data(), static_cast<int>(salt.size()),
        iterations,
        EVP_sha256(),
        keyLen,
        out.data());

    if (ok != 1)
    {
        throw std::runtime_error("PBKDF2 hashing failed");
    }

    return out;
}

std::string PasswordHasher::hash(const std::string& password)
{
    std::vector<unsigned char> salt(kSaltBytes);

    if (RAND_bytes(salt.data(), kSaltBytes) != 1)
    {
        throw std::runtime_error("Failed to generate random salt");
    }

    std::vector<unsigned char> derived = pbkdf2(password, salt, kIterations, kHashBytes);

    // פורמט: pbkdf2_sha256$iterations$saltHex$hashHex
    std::ostringstream oss;
    oss << kAlgoTag << '$' << kIterations << '$'
        << toHex(salt.data(), salt.size()) << '$'
        << toHex(derived.data(), derived.size());

    return oss.str();
}

bool PasswordHasher::verify(const std::string& password, const std::string& storedHash)
{
    // מפרקים את הפורמט: pbkdf2_sha256$iterations$saltHex$hashHex
    size_t p1 = storedHash.find('$');
    size_t p2 = storedHash.find('$', p1 + 1);
    size_t p3 = storedHash.find('$', p2 + 1);

    if (p1 == std::string::npos || p2 == std::string::npos || p3 == std::string::npos)
    {
        return false; // פורמט לא תקין
    }

    std::string algo = storedHash.substr(0, p1);
    int iterations = std::stoi(storedHash.substr(p1 + 1, p2 - p1 - 1));
    std::string saltHex = storedHash.substr(p2 + 1, p3 - p2 - 1);
    std::string hashHex = storedHash.substr(p3 + 1);

    if (algo != kAlgoTag)
    {
        return false;
    }

    std::vector<unsigned char> salt = fromHex(saltHex);
    std::vector<unsigned char> expected = fromHex(hashHex);

    std::vector<unsigned char> actual = pbkdf2(password, salt, iterations, static_cast<int>(expected.size()));

    if (actual.size() != expected.size())
    {
        return false;
    }

    // השוואה ב-constant time כדי לא לחשוף מידע דרך timing attack
    unsigned char diff = 0;
    for (size_t i = 0; i < actual.size(); ++i)
    {
        diff |= actual[i] ^ expected[i];
    }

    return diff == 0;
}