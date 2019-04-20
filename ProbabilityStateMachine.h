#pragma once
#include "ProbabilityState.h"
#include "Configuration.h"

class ProbabilityStateMachine
{
public:
	enum
	{
		COUNT_MATCH_TRANSITION = 0,
		DEFAULT_TRANSITION
	};

	enum
	{
		INC_STATE = 0,
		DEC_STATE,
		BASE_STATE,
		NUM_STATES
	};

	ProbabilityStateMachine(Configuration config);
	~ProbabilityStateMachine();

	ProbabilityState* GetCurrentState() { return mCurrentState; }
	long long GetCurrentProbabiltiyModifier() { return mCurrentState->GetProbabilityModifier(); }

	bool Tick();
	void SetState(int stateIdentifier);
private:
	ProbabilityStateMachine() = delete;
	ProbabilityState* mCurrentState;
	ProbabilityState* mIncState;
	ProbabilityState* mDecState;
	ProbabilityState* mBaseState;
	bool mTransitionMode;
	int mPreviousState;
	Configuration mConfig;
};

