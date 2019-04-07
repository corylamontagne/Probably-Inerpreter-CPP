#pragma once
#include "Instruction.h"
#include "Configuration.h"
#include <tuple>
#include <random>

class BaseInstruction
{
public:
	BaseInstruction() {}
	virtual ~BaseInstruction() {}
};

template<typename T>
class InstructionObject : public BaseInstruction
{
public:
	InstructionObject(Configuration config, Instructions::Instruction<T> primary, unsigned long long primaryProbability, 
											Instructions::Instruction<T> secondary, unsigned long long secondaryProbability, unsigned long long NOPProbability) : configuration(config)
	{
		//create our instruction possibilities for this object, primary, secondary, and NOP.
		mInstructions = std::tuple< InstructionProbabilityObject, InstructionProbabilityObject, InstructionProbabilityObject >(std::make_tuple(std::make_pair(primary, primaryProbability), std::make_pair(secondary, secondaryProbability), std::make_pair([](T) {}, NOPProbability)));
	}

	virtual ~InstructionObject() {}

	void operator()(unsigned long long probability, int probabilityModifier)
	{
		unsigned long long finalProbability = probability + probabilityModifier;

		if (finalProbability <= std::get<0>(mInstructions).second)
		{
			std::get<0>(mInstructions).first();
		}

	}
private:
	typedef std::pair<Instructions::Instruction<T>, unsigned long long> InstructionProbabilityObject;
	std::tuple< InstructionProbabilityObject, InstructionProbabilityObject, InstructionProbabilityObject > mInstructions;
	Configuration configuration;
};