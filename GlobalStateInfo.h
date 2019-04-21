#pragma once
#include "LookupTable.h"
#include "Configuration.h"
#include <string>
#include "ProbabilityStateMachine.h"

//TODO: Eventually all this information needs a proper place to live.
//this is its home for the time being until the important things are done

#define MAX_PROB 1000000000
#define MIN_PROB 500
#define NOP_PROB 100
#define MAX_INSTR 20
#define FUNCTION_PROBABILITY_GATE 10000
#define IF_BLOCK_PROBABILITY 10000

char array[2046] = { 0 };
char *ptr = array;

char gXRegister = 0, gYRegister = 0;
bool gRegisterCheckPassed = true;

LookupTable gLookup;
FunctionTable gFunctionLookup;
Configuration gConfig;
char gEndFunction = ';';
char gQuit = 'q';

double gProbability = 1;

unsigned gSeed = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count();
std::default_random_engine gGenerator(gSeed);
std::uniform_int_distribution<> gDistribution(0, MAX_PROB);
int GetMaxProbabilityRoll() { return gDistribution(gGenerator); }
double gProbabilityMultiplier = 1.0;

ProbabilityStateMachine* gStateMachine;

struct FunctionBuildData
{
	FunctionBuildData() { ResetFunctionBuildData(); }

	std::string instructionList;
	std::string currentFunctionName;
	bool functionParseMode;

	void ResetFunctionBuildData()
	{
		instructionList = "";
		currentFunctionName = "";
		functionParseMode = false;
	}
};
FunctionBuildData functionBuild;