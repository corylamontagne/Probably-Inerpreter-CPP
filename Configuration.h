#pragma once

class Configuration
{
public:
	Configuration() : mMaximumProbability(0), mMinimumProbability(0), mNopProbability(0), mFunctionGateProbability(0), mConditionalGateProbability(0), mMaximimInstructionCount(1) {}
	Configuration(unsigned long long maxP, unsigned long long minP, unsigned long long nopP, unsigned long long funcP, unsigned long long condP, int instr) :
					mMaximumProbability(maxP), mMinimumProbability(minP), mNopProbability(nopP), mFunctionGateProbability(funcP), mConditionalGateProbability(condP),
					mMaximimInstructionCount(instr) {}
	~Configuration() {}
private:
	unsigned long long mMaximumProbability;
	unsigned long long mMinimumProbability;
	unsigned long long mNopProbability;
	unsigned long long mFunctionGateProbability;
	unsigned long long mConditionalGateProbability;
	int mMaximimInstructionCount;
};