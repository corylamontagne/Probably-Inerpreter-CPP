#pragma once

class ProbabilityState
{
public:

	ProbabilityState();
	ProbabilityState(int maxInst, long long probMod, int ident);
	~ProbabilityState();

	bool TickState();
	int GetIdentifier() { return mStateIdentifier; }
	void SetInstructionCount(int count) { mCurrentInstructionCount = (mMaximimInstructionCount - count); }
	void ResetState();
	int GetMaxInstructionCount() { return mMaximimInstructionCount; }
	int GetCurrentInstructionCount() { return mCurrentInstructionCount; }
	long long GetProbabilityModifier() { return mProbabilityModifier; }
private:
	int mCurrentInstructionCount;
	int mMaximimInstructionCount;
	long long mProbabilityModifier;
	int mStateIdentifier;
};

