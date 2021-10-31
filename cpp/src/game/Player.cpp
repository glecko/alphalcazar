#include "Player.hpp"
#include "Piece.hpp"
#include "parameters.hpp"

namespace Alphalcazar::Game {
    Player::Player(PlayerId id)
        : mId { id }
    {
        mPieces.reserve(c_PieceTypes);
        for (PieceType type = 1; type <= c_PieceTypes; type++) {
            mPieces.emplace_back(std::make_unique<Piece>(mId, type));
        }
    }

    Player::Player(const Player& other)
        : mId{ other.mId }
    {
        mPieces.reserve(other.mPieces.size());
        for (auto& piece : other.mPieces) {
            mPieces.emplace_back(std::make_unique<Piece>(*piece));
        }
    }

    Player::~Player() {}

    Piece* Player::GetPiece(PieceType type) const {
        for (auto& piece : mPieces) {
            if (piece->GetType() == type) {
                return piece.get();
            }
        }
        return nullptr;
    }

    std::vector<Piece*> Player::GetPieces() const {
        std::vector<Piece*> result;
        result.reserve(mPieces.size());
        for (auto& piece : mPieces) {
            result.push_back(piece.get());
        }
        return result;
    }

    PlayerId Player::GetId() const {
        return mId;
    }

    bool Player::operator==(const Player& player) const {
        return GetId() == player.GetId();
    }

    bool Player::operator!=(const Player& player) const {
        return GetId() == player.GetId();
    }
}
