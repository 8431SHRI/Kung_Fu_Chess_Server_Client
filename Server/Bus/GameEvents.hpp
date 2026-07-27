#pragma once

#include <optional>
#include <string>

#include "Piece.hpp"
#include "Position.hpp"

/*
 * קובץ זה מכיל אך ורק מבני Event (DTO) שעוברים על ה-EventBus.
 * שום Event כאן לא יודע מי מפרסם אותו ומי מאזין לו - זו כל המטרה של ה-Bus.
 */

// מפורסם פעם אחת ע"י GameSession/Room כשמשחק מתחיל בפועל (שני שחקנים חוברו).
// Consumers טיפוסיים: אנימציית "התחלת משחק" בצד לקוח, לוג.
struct GameStartedEvent
{
    std::string gameId;
    std::string whiteUsername;
    std::string blackUsername;
};

// מפורסם ע"י GameSession בכל מהלך שבוצע בפועל על הלוח (אחרי שהתנועה/הקפיצה הסתיימה,
// לא ברגע שהתקבלה הבקשה - כדי לשקף את מה שבאמת קרה על הלוח).
// Consumers טיפוסיים: Move-log, אפקט קול "move.wav".
struct MoveMadeEvent
{
    std::string gameId;
    Side side;
    PieceType pieceType;
    Position from;
    Position to;
};

// מפורסם בנוסף ל-MoveMadeEvent, ורק כשהיה כלי שנאכל ביעד.
// Consumers טיפוסיים: אפקט קול "capture.wav", move-log ("Nxe5").
struct PieceCapturedEvent
{
    std::string gameId;
    Side capturingSide;
    PieceType capturedPieceType;
    Position at;
};

// מפורסם פעם אחת ע"י GameSession כשהמשחק מסתיים (מלך נאכל / ניתוק / ויתור וכו').
// Consumers טיפוסיים: ScoreUpdateSubscriber (ELO), אנימציית "סיום משחק", GAME_OVER לרשת.
struct GameOverEvent
{
    std::string gameId;
    std::optional<Side> winnerSide; // nullopt = תיקו
    std::string reason;             // למשל "king_captured" / "disconnect_timeout" / "resign"
};

// מפורסם ע"י ScoreUpdateSubscriber אחרי שחישב/שמר ELO חדש - כדי שרכיבים אחרים
// (למשל שכבת הרשת ששולחת ללקוח, או לוג) לא יצטרכו להכיר את EloCalculator/IUserRepository בעצמם.
struct ScoreUpdatedEvent
{
    std::string gameId;
    std::string userId;
    std::string username;
    int oldElo;
    int newElo;
};
