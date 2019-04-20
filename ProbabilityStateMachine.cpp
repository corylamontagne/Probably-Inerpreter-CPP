#include "ProbabilityStateMachine.h"

ProbabilityStateMachine::ProbabilityStateMachine(Configuration config) : mCurrentState(nullptr)
{
	mConfig = config;
	unsigned long long maxProb = mConfig.GetMaxProbability();
	unsigned long long minProb = mConfig.GetMinProbability();
	mBaseState = new ProbabilityState(-1, 0, BASE_STATE);
	mIncState = new ProbabilityState(mConfig.GetMaxInstructionCount(), 100000, INC_STATE);
	mDecState = new ProbabilityState(mConfig.GetMaxInstructionCount(), -100000, DEC_STATE);
	mCurrentState = mBaseState;
	mTransitionMode = DEFAULT_TRANSITION;
	mPreviousState = BASE_STATE;
}

ProbabilityStateMachine::~ProbabilityStateMachine()
{
}

bool ProbabilityStateMachine::Tick()
{
	if (mCurrentState->TickState())
	{
		if (mTransitionMode == COUNT_MATCH_TRANSITION)
		{
			mCurrentState->ResetState();
			mCurrentState = mBaseState;
			mTransitionMode = DEFAULT_TRANSITION;
		}
		else
		{
			int count = mCurrentState->GetMaxInstructionCount();
			mCurrentState->ResetState();
			//max state instructions reached, swap state
			if (mCurrentState->GetIdentifier() == INC_STATE)
			{
				mCurrentState = mDecState;
				mCurrentState->SetInstructionCount(count);
			}
			else if (mCurrentState->GetIdentifier() == DEC_STATE)
			{
				mCurrentState->ResetState();
				mCurrentState = mIncState;
				mCurrentState->SetInstructionCount(count);
			}
			mTransitionMode = COUNT_MATCH_TRANSITION;
		}
	}

	return true;
}

void ProbabilityStateMachine::SetState(int stateIdentifier)
{
	//in automatic match state instruction mode, abort
	if (mTransitionMode == COUNT_MATCH_TRANSITION)
	{
		return;
	}

	int count = mCurrentState->GetCurrentInstructionCount();
	int state = mCurrentState->GetIdentifier();

	switch (stateIdentifier)
	{
	case INC_STATE:
	{
		switch (state)
		{
		case DEC_STATE:
		{
			mCurrentState->ResetState();
			mCurrentState = mIncState;
			mCurrentState->SetInstructionCount(count);
			mTransitionMode = COUNT_MATCH_TRANSITION;
		}
			break;
		case BASE_STATE:
		{
			mCurrentState->ResetState();
			mCurrentState = mIncState;
			mTransitionMode = DEFAULT_TRANSITION;
		}
		break;
		default:
			break;
		}
	}
		break;
	case DEC_STATE:
	{
		switch (state)
		{
		case INC_STATE:
		{
			mCurrentState->ResetState();
			mCurrentState = mDecState;
			mCurrentState->SetInstructionCount(count);
			mTransitionMode = COUNT_MATCH_TRANSITION;
		}
		break;
		case BASE_STATE:
		{
			mCurrentState->ResetState();
			mCurrentState = mDecState;
			mTransitionMode = DEFAULT_TRANSITION;
		}
		break;
		default:
			break;
		}
	}
		break;
	default:
	{
		if (count > 0)
		{
			switch (state)
			{
			case DEC_STATE:
			{
				mCurrentState->ResetState();
				mCurrentState = mIncState;
				mCurrentState->SetInstructionCount(count);
				mTransitionMode = COUNT_MATCH_TRANSITION;
			}
			break;
			case INC_STATE:
			{
				mCurrentState->ResetState();
				mCurrentState = mDecState;
				mCurrentState->SetInstructionCount(count);
				mTransitionMode = COUNT_MATCH_TRANSITION;
			}
			break;
			default:
				break;
			}
		}
		else
		{
			mCurrentState->ResetState();
			mCurrentState = mBaseState;
			mCurrentState->SetInstructionCount(count);
			mTransitionMode = DEFAULT_TRANSITION;
		}
	}
		break;
	}
}