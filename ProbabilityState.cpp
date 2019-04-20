#include "ProbabilityState.h"

ProbabilityState::ProbabilityState()
{
	mStateIdentifier = -1;
	mMaximimInstructionCount = 10;
	ResetState();
}

ProbabilityState::ProbabilityState(int maxInst, long long probMod, int ident) :
	mMaximimInstructionCount(maxInst), 
	mProbabilityModifier(probMod),
	mStateIdentifier(ident),
	mCurrentInstructionCount(0)
{
}

ProbabilityState::~ProbabilityState()
{
}

bool ProbabilityState::TickState()
{
	if (mMaximimInstructionCount != -1)
	{
		mCurrentInstructionCount += 1;
		return (mCurrentInstructionCount >= mMaximimInstructionCount);
	}
	return false;
}

void ProbabilityState::ResetState()
{
	mCurrentInstructionCount = 0;
	mProbabilityModifier = 0;
}