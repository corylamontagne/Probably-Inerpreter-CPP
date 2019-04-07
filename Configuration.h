#pragma once

class Configuration
{
public:
	Configuration(unsigned long long maxP, unsigned long long minP, unsigned long long nopP, unsigned long long funcP, unsigned long long condP, int instr) :
					maximumProbability(maxP), minimumProbability(minP), nopProbability(nopP), functionGateProbability(funcP), conditionalGateProbability(condP),
					maximimInstructionCount(instr) {}
	~Configuration() {}
private:
	unsigned long long maximumProbability;
	unsigned long long minimumProbability;
	unsigned long long nopProbability;
	unsigned long long functionGateProbability;
	unsigned long long conditionalGateProbability;
	int maximimInstructionCount;
};