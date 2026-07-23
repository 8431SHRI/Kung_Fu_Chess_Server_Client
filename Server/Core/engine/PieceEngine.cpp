#include "PieceEngine.hpp"

// אתחול המשתנה הסטטי של המחלקה ל-1
std::atomic<int> PieceEngine::nextId{1};

std::shared_ptr<Piece> PieceEngine::createPiece(char typeChar, Side side, Position pos) {
    // חיפוש סוג הכלי במפה הדינמית
    auto it = GameConfig::TYPE_MAP.find(typeChar);
    if (it == GameConfig::TYPE_MAP.end()) {
        return nullptr; // סוג כלי לא מוכר
    }

    // הפקת מזהה ייחודי: לוקח את הערך הנוכחי ומקדם את המונה עבור הכלי הבא
    int currentId = nextId++;

    // יצירת הכלי עם ה-ID האוטומטי (מתקן את השגיאה שבה הועבר הזמן ההתחלתי)
    return std::make_shared<Piece>(currentId, side, it->second, pos);
}

bool PieceEngine::isKnownType(char type) {
    return GameConfig::TYPE_MAP.find(type) != GameConfig::TYPE_MAP.end();
}

std::shared_ptr<Piece> PieceEngine::createFromToken(const std::string& token, const Position& pos) {
    
    if (token.length() != 2) {
        return nullptr;
    }
    
    char sideChar = token[0];  // התו הראשון מייצג את הצד (w / b)
    char typeChar = token[1];  // התו השני מייצג את סוג הכלי (K, Q, R...)
    Side side;
    
    // אימות קפדני של הצד למניעת באגים לוגיים
    if (sideChar == GameConfig::SIDE_WHITE) {
        side = Side::WHITE;
    } else if (sideChar == GameConfig::SIDE_BLACK) {
        side = Side::BLACK;
    } else {
        return nullptr; // אם זה לא 'w' ולא 'b' - הטוקן אינו חוקי!
    }

    // קריאה ל-createPiece שמטפלת בהצלבה מול המפה ובשליפת ה-ID הייחודי
    return createPiece(typeChar, side, pos);
}

void PieceEngine::resetIdCounter() {
    nextId = 1;
}