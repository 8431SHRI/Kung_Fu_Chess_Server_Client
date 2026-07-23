#pragma once

#include <string>

/*
 * @struct User
 * @brief ייצוג נתונים טהור (POD) של משתמש רשום.
 * Purpose: רשומת המשתמש המנוהלת ע"י IUserRepository implementations.
 * Contains: מזהה ייחודי, שם משתמש, hash של הסיסמה, ודירוג ELO נוכחי.
 * Created by: AuthService (בעת register) | Used by: AuthService, IUserRepository implementations.
 */
struct User
{
    std::string id;

    std::string username;

    std::string passwordHash;

    int elo = 1000;
};