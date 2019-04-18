#include "ProbabilityState.h"


ProbabilityState::ProbabilityState(int maxInst, int probMod) : 
	mMaximimInstructionCount(maxInst), 
	mProbabilityModifier(probMod)
{
}

ProbabilityState::~ProbabilityState()
{
}
