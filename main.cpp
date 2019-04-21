#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <fstream>
#include <iomanip>
#include <map>

//SETTINGS
#define MAX_PROB 1000000000
#define MIN_PROB 500
#define NOP_PROB 100
#define MAX_INSTR 20
#define FUNCTION_PROBABILITY_GATE 10000
#define IF_BLOCK_PROBABILITY 10000

char array[2046] = { 0 };
char *ptr = array;

#include "InstructionObject.h"
#include "LookupTable.h"
#include "GlobalStateInfo.h"

bool Process(const std::string&input)
{
	for (std::string::const_iterator it = input.begin(); it != input.end(); ++it)
	{
		if (*it == 'q') { return true; }

		bool executeNextInstruction = gRegisterCheckPassed;
		gRegisterCheckPassed = true;

		//in function parse mode, we add instructions to a separate string to execute later
		if (functionBuild.functionParseMode && !functionBuild.currentFunctionName.empty() && *it != ';')
		{
			functionBuild.instructionList += std::string(1, *it);
			continue;
		}
		//when we exit function parse mode, store off the function in the lookup table under the instructon name 
		//and reset the function build data
		else if (functionBuild.functionParseMode && !functionBuild.currentFunctionName.empty() && *it == ';')
		{
			gFunctionLookup.AddFunction(functionBuild.currentFunctionName, functionBuild.instructionList);
			functionBuild.ResetFunctionBuildData();
			continue;
		}
		//fetch and invoke the instruction if valid and if the last register check passed
		BaseInstruction* instruction = gLookup.FetchInstruction(std::string(1, *it));
		if (executeNextInstruction && instruction && (!functionBuild.functionParseMode || *it == ';'))
		{
			gStateMachine->Tick();
			//TODO: Apply probability gate logic
			(*instruction)(GetMaxProbabilityRoll(), gStateMachine->GetCurrentProbabiltiyModifier(), gProbabilityMultiplier);
		}
		else
		{
			//we have entered functino mode, store off first character as the functino identifier
			if (functionBuild.functionParseMode && functionBuild.currentFunctionName.empty())
			{
				//set the function name
				functionBuild.currentFunctionName = std::string(1, *it);
			}
			else
			{
				//we need to execute a function if we find one, otherwise the instruction is invalid
				std::string func = gFunctionLookup.FetchFunction(std::string(1, *it));
				if (!func.empty())
				{
					//do not execute the function if the register check fails
					if (executeNextInstruction)
					{
						//determine the probabilities for the function
						int gateProbability = GetMaxProbabilityRoll();
						if (gateProbability < FUNCTION_PROBABILITY_GATE * 100)
						{
							//down
							gProbabilityMultiplier = gConfig.GetFunctionDownMult();
						}
						else if (gateProbability > MAX_PROB - FUNCTION_PROBABILITY_GATE)
						{
							//up
							gProbabilityMultiplier = gConfig.GetFunctionUpMult();
						}

						//execute the function
						if (!Process(func))
						{
							return false;
						}

						//TODO: Make this better? Global single var for multiplier is no bueno
						//reset the probability modifier
						gProbabilityMultiplier = 1.0;
					}
				}
				else
				{
					std::cout << "Unknown instruction " << *it << ". Script failed" << std::endl;
					return false;
				}
			}
		}
	}
	return true;
}

void SetUpLookupTable(Configuration config)
{
	//reusable lambdas
	std::function<void()> nop = [](void) {};
	std::function<void()> increment = [](void) {(*ptr)++; };
	std::function<void()> decrement = [](void) {(*ptr)--; };
	std::function<void()> forward = [](void) {ptr++; };
	std::function<void()> backward = [](void) {ptr--; };
	std::function<void()> output = [](void) {std::cout << *ptr; };
	std::function<void()> incState = [](void) {gStateMachine->SetState(ProbabilityStateMachine::DEC_STATE); };
	std::function<void()> decState = [](void) {gStateMachine->SetState(ProbabilityStateMachine::INC_STATE); };

	int highEndProbability = (MAX_PROB - (MIN_PROB + NOP_PROB));

	//table setup
	gLookup.AddInstruction("i", new InstructionObject<void>(config, Instructions::Instruction<void>(increment), highEndProbability,
		Instructions::Instruction<void>(decrement), MIN_PROB, NOP_PROB));
	gLookup.AddInstruction("d", new InstructionObject<void>(config, Instructions::Instruction<void>(decrement), highEndProbability,
		Instructions::Instruction<void>(increment), MIN_PROB, NOP_PROB));

	gLookup.AddInstruction("f", new InstructionObject<void>(config, Instructions::Instruction<void>(forward), highEndProbability,
		Instructions::Instruction<void>(backward), MIN_PROB, NOP_PROB));
	gLookup.AddInstruction("b", new InstructionObject<void>(config, Instructions::Instruction<void>(backward), highEndProbability,
		Instructions::Instruction<void>(forward), MIN_PROB, NOP_PROB));

	gLookup.AddInstruction("o", new InstructionObject<void>(config, Instructions::Instruction<void>(output), MAX_PROB,
		Instructions::Instruction<void>(nop), 0, 0));

	gLookup.AddInstruction("x", new InstructionObject<void>(config, Instructions::Instruction<void>([]() {gXRegister = (*ptr); }), MAX_PROB,
		Instructions::Instruction<void>(nop), 0, 0));
	gLookup.AddInstruction("y", new InstructionObject<void>(config, Instructions::Instruction<void>([]() {gYRegister = (*ptr); }), MAX_PROB,
		Instructions::Instruction<void>(nop), 0, 0));

	gLookup.AddInstruction("+", new InstructionObject<void>(config, Instructions::Instruction<void>(incState), highEndProbability,
		Instructions::Instruction<void>(decState), MIN_PROB, NOP_PROB));
	gLookup.AddInstruction("-", new InstructionObject<void>(config, Instructions::Instruction<void>(decState), highEndProbability,
		Instructions::Instruction<void>(incState), MIN_PROB, NOP_PROB));

	gLookup.AddInstruction("=", new InstructionObject<void>(config, Instructions::Instruction<void>([]() {gRegisterCheckPassed = (gXRegister == gYRegister);}), highEndProbability,
		Instructions::Instruction<void>([]() {gRegisterCheckPassed = (gXRegister != gYRegister); }), MIN_PROB, NOP_PROB));

	gLookup.AddInstruction("#", new InstructionObject<void>(config, Instructions::Instruction<void>([]() {functionBuild.functionParseMode = true; }), MAX_PROB,
		Instructions::Instruction<void>(nop), 0, 0));
}

int main()
{
	gConfig = Configuration(MAX_PROB, MIN_PROB, NOP_PROB, FUNCTION_PROBABILITY_GATE, IF_BLOCK_PROBABILITY, MAX_INSTR);
	SetUpLookupTable(gConfig);
	gStateMachine = new ProbabilityStateMachine(gConfig);

	//load the script
	std::ifstream file("test.prob");
	//std::ifstream file("testprob.prob");

	//build string of the contents of the file
	std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	//purge newlines
	contents.erase(std::remove(contents.begin(), contents.end(), '\n'), contents.end());

	//process the data
	Process(contents);

	//TODO: output the final probabilities(optional)

	while (!std::cin.get()) {}

	delete gStateMachine;

	return 0;
}