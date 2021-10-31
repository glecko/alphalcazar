#include "Piece.hpp"
#include "Coordinates.hpp"
#include "parameters.hpp"

namespace Alphalcazar::Game {
	Piece::Piece(PlayerId owner, PieceType type)
		: mOwner { owner }
		, mType { type }
	{}

	Piece::Piece(const Piece& other)
		: mOwner { other.mOwner }
		, mType{other.mType}
		, mDirection{ other.mDirection }
		, mInPlay { other.mInPlay }
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

	void Piece::SetMovementDirection(Direction direction) {
		mDirection = direction;
	}

	void Piece::SetInPlay(bool inPlay) {
		mInPlay = inPlay;
	}

	bool Piece::IsInPlay() const {
		return mInPlay;
	}

	bool Piece::operator==(const Piece& other) const {
		return other.mOwner == mOwner && other.mType == mType;
	}
}