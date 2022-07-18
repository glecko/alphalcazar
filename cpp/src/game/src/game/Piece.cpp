#include "game/Piece.hpp"
#include "game/parameters.hpp"

namespace Alphalcazar::Game {
	Piece::Piece() noexcept
		: mType{ c_InvalidPieceType }
		, mDirection{ Direction::NONE }
		, mOwner{ PlayerId::NONE }
	{}

	Piece::Piece(PlayerId owner, PieceType type) noexcept
		: mType{ type }
		, mDirection{ Direction::NONE }
		, mOwner { owner }
	{}

	Piece::Piece(const Piece& other) noexcept
		: mType {other.mType}
		, mDirection { other.mDirection }
		, mOwner{ other.mOwner }
	{}

	Piece::~Piece() {}

	PieceType Piece::GetType() const {
		return mType;
	}

	PlayerId Piece::GetOwner() const {
		return mOwner;
	}

	Direction Piece::GetMovementDirection() const {
		return mDirection;
	}

	bool Piece::IsPushable() const {
		return mType == c_PushablePieceType;
	}

	bool Piece::IsPusher() const {
		return mType == c_PusherPieceType;
	}

	bool Piece::IsValid() const {
		return mType != c_InvalidPieceType && mOwner != PlayerId::NONE;
	}

	void Piece::SetMovementDirection(Direction direction) {
		mDirection = direction;
	}

	bool Piece::operator==(const Piece& other) const {
		return other.mOwner == mOwner && other.mType == mType;
	}
}