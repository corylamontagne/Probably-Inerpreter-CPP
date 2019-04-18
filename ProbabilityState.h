#pragma once
class ProbabilityState
{
public:
	ProbabilityState(int maxInst, int probMod);
	~ProbabilityState();
private:
	int mCurrentInstructionCount;
	int mMaximimInstructionCount;
	int mProbabilityModifier;


	//Do we handle transition possibilities here?
};

