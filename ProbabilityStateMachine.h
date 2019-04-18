#pragma once
#include "ProbabilityState.h"
#include "Configuration.h"

class ProbabilityStateMachine
{
public:
	ProbabilityStateMachine();
	~ProbabilityStateMachine();

	ProbabilityState* GetCurrentState() { return mCurrentState; }
private:
	ProbabilityState* mCurrentState;
	Configuration mConfig;
};

