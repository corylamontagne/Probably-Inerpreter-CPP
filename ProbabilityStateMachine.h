#pragma once
#include "ProbabilityState.h"

class ProbabilityStateMachine
{
public:
	ProbabilityStateMachine();
	~ProbabilityStateMachine();
private:
	ProbabilityState* mCurrentState;

};

