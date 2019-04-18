#pragma once
#include "ProbabilityState.h"

class ProbabilityStateMachine
{
public:
	ProbabilityStateMachine();
	~ProbabilityStateMachine();

	ProbabilityState* GetCurrentState() { return mCurrentState; }
private:
	ProbabilityState* mCurrentState;

};

