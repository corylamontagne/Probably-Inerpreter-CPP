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

#if 0
//GLOBAL CONFIG

bool equalityCheckPass = true, ifBlockStarted = true;
char x = 0, y = 0, z = 0;
double mainProbability = 1;
double mainOccuredProbabiltiy = 1;
bool functionMode = false, functionName = false;
std::map<std::string, std::string> functions;
std::string currentFunctionName = "", executingFunctionName = "";
double gateMultiplier = 1.0;

unsigned seed = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count();
std::default_random_engine generator(seed);
std::uniform_int_distribution<> distribution(0, MAX_PROB);

int instructionCount = 0, lastInstructionCount = 0;

enum ProbabilityMode
{
	UP = 0,
	DOWN,
	FLAT
};
ProbabilityMode mode = FLAT;

//INSTRUCTIONS
class Instruction
{
public:
	virtual void operator()() = 0;
	Instruction() {}
	virtual ~Instruction() {}
};

class NOP : public Instruction
{
public:
	NOP() : Instruction() {}
	virtual ~NOP() {}
	virtual void operator()() override {}
};

class Add : public Instruction
{
public:
	Add() : Instruction() {}
	virtual ~Add() {}
	virtual void operator()() override { (*ptr)++; }
};

class Sub : public Instruction
{
public:
	Sub() : Instruction() {}
	virtual ~Sub() {}
	virtual void operator()() override { --(*ptr); }
};

class MovF : public Instruction
{
public:
	MovF() : Instruction() {}
	virtual ~MovF() {}
	virtual void operator()() override { ptr++; }
};

class MovB : public Instruction
{
public:
	MovB() : Instruction() {}
	virtual ~MovB() {}
	virtual void operator()() override { --ptr; }
};

class InstructionObject
{
public:
	InstructionObject(std::pair<Instruction*, int> instruction1, std::pair<Instruction*, int> instruction2) { PossibilitySet = { std::make_pair(new NOP(), NOP_PROB), instruction1, instruction2 }; }
	~InstructionObject() { std::for_each(PossibilitySet.begin(), PossibilitySet.end(), [](std::pair<Instruction*, int> &n) { delete n.first; }); }

	void operator()()
	{
		int probability = distribution(generator);
		int modifier = (mode == FLAT) ? 0 : (MIN_PROB / MAX_INSTR) * (MAX_INSTR - instructionCount);
		if (mode == UP)
		{
			probability += modifier;
		}
		else if (mode == DOWN)
		{
			probability -= modifier;
		}

		int occured = 0;
		if (probability <= (PossibilitySet[2].second * gateMultiplier))
		{
			(*PossibilitySet[2].first)();
			occured = PossibilitySet[2].second;
		}
		else if (probability >= (MAX_PROB - PossibilitySet[1].second))
		{
			(*PossibilitySet[1].first)();
			occured = PossibilitySet[1].second;
		}
		else
		{
			(*PossibilitySet[0].first)();
			occured = PossibilitySet[0].second;
		}
		modifier *= (mode == UP) ? 1 : -1;
		mainProbability *= (double)(PossibilitySet[1].second + modifier) / (double)MAX_PROB;
		mainOccuredProbabiltiy *= (double)(occured + modifier) / (double)MAX_PROB;
	}
private:
	InstructionObject() = delete;
	std::vector<std::pair<Instruction*, int> > PossibilitySet;
};

int high = MAX_PROB - MIN_PROB;
int low = MAX_PROB - (high + NOP_PROB);

InstructionObject Increment(std::make_pair(new Add(), high), std::make_pair(new Sub(), low));
InstructionObject MoveForward(std::make_pair(new MovF(), high), std::make_pair(new MovB(), low));
InstructionObject Decrement(std::make_pair(new Sub(), high), std::make_pair(new Add(), low));
InstructionObject MoveBackward(std::make_pair(new MovB(), high), std::make_pair(new MovF(), low));

void IncrementInstructionCount()
{
	instructionCount++;
	if (lastInstructionCount > 0)
	{
		lastInstructionCount--;
		if (lastInstructionCount == 0)
		{
			instructionCount = 0;
		}
	}
}

bool ProcessScript(std::string script)
{
	//TODO: Fuck this temporary parsing. We need something legit that allows us to create multi character instructions AND functions.
	for (std::string::iterator it = script.begin(); it != script.end(); ++it)
	{
		if (ifBlockStarted && !equalityCheckPass && *it != '}')
		{
			continue;
		}

		if (functionMode && *it != '#')
		{
			if (functionName)
			{
				if (functions.find(&*it) == functions.end())
				{
					currentFunctionName = *it;
					functionName = false;
				}
				else
				{
					std::cout << "Function with name " << *it << " already exists!" << std::endl;
					return false;
				}
			}
			else
			{
				functions[currentFunctionName] += *it;
			}
		}
		else
		{
			bool set = true, frame1 = false;
			switch (*it)
			{
			case '{':
				ifBlockStarted = true;
				set = false;
				break;
			case '}':
				ifBlockStarted = false;
				equalityCheckPass = false;
				set = false;
				break;
			case '=':
			{
				set = false;
				int probability = distribution(generator);
				equalityCheckPass = (x == y);
				if (probability <= IF_BLOCK_PROBABILITY)
				{
					equalityCheckPass = !equalityCheckPass;
				}
			}
			break;
			case 'x':
				set = false;
				x = *ptr;
				break;
			case 'y':
				set = false;
				y = *ptr;
				break;
			case 'z':
				set = false;
				z = *ptr;
				break;
			case 'i':
				Increment();
				IncrementInstructionCount();
				break;
			case 'd':
				Decrement();
				IncrementInstructionCount();
				break;
			case 'f':
				MoveForward();
				IncrementInstructionCount();
				break;
			case 'b':
				MoveBackward();
				IncrementInstructionCount();
				break;
			case 'o':
				set = false;
				std::cout << *ptr;
				break;
			case '#':
				functionMode = !functionMode;
				functionName = functionMode;
				set = false;
				break;
			case '+':
			{
				set = false;
				if (mode == FLAT || instructionCount == 0)
				{
					mode = UP;
					frame1 = true;
				}
				else if (instructionCount > 0)
				{
					if (mode != UP)
					{
						mode = UP;
					}
					else if (mode == UP)
					{
						mode = DOWN;
					}
					lastInstructionCount = instructionCount;
					instructionCount = 0;
				}
			}
			break;
			case '-':
			{
				set = false;
				if (mode == FLAT || instructionCount == 0)
				{
					mode = DOWN;
					frame1 = true;
				}
				else if (instructionCount > 0)
				{
					if (mode != DOWN)
					{
						mode = DOWN;
					}
					else if (mode == DOWN)
					{
						mode = UP;
					}
					lastInstructionCount = instructionCount;
					instructionCount = 0;
				}
			}
			break;
			default:
				if (*it == '\n')
				{
					break;
				}
				std::string f(1, *it);
				auto iter = functions.find(f);
				if (iter != functions.end())
				{
					if (!executingFunctionName.empty() && f == executingFunctionName)
					{
						std::cout << "Recursion is not currently allowed!" << std::endl;
						return false;
					}
					executingFunctionName = f;
					//set gate multiplier
					int gateProbability = distribution(generator);
					if (gateProbability < FUNCTION_PROBABILITY_GATE * 100)
					{
						//down
						gateMultiplier = 1000;
					}
					else if (gateProbability > MAX_PROB - FUNCTION_PROBABILITY_GATE)
					{
						//up
						gateMultiplier = .1;
					}
					bool res = ProcessScript(iter->second);
					//reset the gate mult
					gateMultiplier = 1.0;
					executingFunctionName = "";
					if (!res)
					{

						return false;
					}
				}
				else
				{
					std::cout << "Unknown instruction \'" << *it << "\' found!" << std::endl;
					return false;
				}
				set = true;
				break;
			}

			if (!frame1 && ((instructionCount == 0 && mode == FLAT) || (lastInstructionCount == 0 && instructionCount == 0 && (mode == DOWN || mode == UP))))
			{
				mode = FLAT;
				instructionCount = 0;
				lastInstructionCount = 0;
			}
			if (instructionCount == MAX_INSTR)
			{
				if (mode == UP)
				{
					mode = DOWN;
					lastInstructionCount = instructionCount;
					instructionCount = 0;
				}
				else if (mode == DOWN)
				{
					mode = UP;
					lastInstructionCount = instructionCount;
					instructionCount = 0;
				}
			}
		}
	}
	return true;
}

int main()
{
	std::ifstream file("test.prob");
	//std::ifstream file("testprob.prob");
	std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	if (!ProcessScript(contents))
	{
		std::cout << "Error running script.";
	}

	std::cout << std::endl;
	std::cout << std::endl << std::setprecision(20) << "Intended Probability: " << mainProbability * 100 << "%" << std::endl;
	std::cout << std::setprecision(20) << "Chances of actual outcome: " << mainOccuredProbabiltiy * 100 << "%" << std::endl;

	file.close();

	while (!std::cin.get()) {}
	return 0;
}
#else
#include "InstructionObject.h"
#include "LookupTable.h"
#include "GlobalStateInfo.h"

void Process(const std::string&input)
{
	for (std::string::const_iterator it = input.begin(); it != input.end(); ++it)
	{
		if (*it == '\n') { continue; }

		if (functionBuild.functionParseMode && !functionBuild.currentFunctionName.empty() && *it != ';')
		{
			functionBuild.instructionList += std::string(1, *it);
			continue;
		}
		else if (functionBuild.functionParseMode && !functionBuild.currentFunctionName.empty() && *it == ';')
		{
			gFunctionLookup.AddFunction(functionBuild.currentFunctionName, functionBuild.instructionList);
			functionBuild.ResetFunctionBuildData();
			continue;
		}
		//fecth and invoke the instructino if valid
		BaseInstruction* instruction = gLookup.FetchInstruction(std::string(1, *it));
		if (instruction && (!functionBuild.functionParseMode || *it == ';'))
		{
			//TODO: Apply probability gate logic
			(*instruction)(gDistribution(gGenerator), 0, gProbabilityModifier);
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
					//TODO: apply function probability gate
					int gateProbability = gDistribution(gGenerator);
					if (gateProbability < FUNCTION_PROBABILITY_GATE * 100)
					{
						//down
						gProbabilityModifier = 1000.0;
					}
					else if (gateProbability > MAX_PROB - FUNCTION_PROBABILITY_GATE)
					{
						//up
						gProbabilityModifier = 0.1;
					}

					Process(func);

					gProbabilityModifier = 1, 0;
				}
				else
				{
					std::cout << "Unknown instruction " << *it << " script failed" << std::endl;
				}
			}
		}
	}
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

	//table setup
	gLookup.AddInstruction("i", new InstructionObject<void>(config, Instructions::Instruction<void>(increment), (MAX_PROB - (MIN_PROB + NOP_PROB)),
		Instructions::Instruction<void>(decrement), MIN_PROB, NOP_PROB));
	gLookup.AddInstruction("d", new InstructionObject<void>(config, Instructions::Instruction<void>(decrement), (MAX_PROB - (MIN_PROB + NOP_PROB)),
		Instructions::Instruction<void>(increment), MIN_PROB, NOP_PROB));

	gLookup.AddInstruction("f", new InstructionObject<void>(config, Instructions::Instruction<void>(forward), (MAX_PROB - (MIN_PROB + NOP_PROB)),
		Instructions::Instruction<void>(backward), MIN_PROB, NOP_PROB));
	gLookup.AddInstruction("b", new InstructionObject<void>(config, Instructions::Instruction<void>(backward), (MAX_PROB - (MIN_PROB + NOP_PROB)),
		Instructions::Instruction<void>(forward), MIN_PROB, NOP_PROB));

	gLookup.AddInstruction("o", new InstructionObject<void>(config, Instructions::Instruction<void>(output), MAX_PROB,
		Instructions::Instruction<void>(nop), 0, 0));

	gLookup.AddInstruction("x", new InstructionObject<void>(config, Instructions::Instruction<void>([]() {gXRegister = (*ptr); }), MAX_PROB,
		Instructions::Instruction<void>(nop), 0, 0));
	gLookup.AddInstruction("y", new InstructionObject<void>(config, Instructions::Instruction<void>([]() {gYRegister = (*ptr); }), MAX_PROB,
		Instructions::Instruction<void>(nop), 0, 0));

	gLookup.AddInstruction("=", new InstructionObject<void>(config, Instructions::Instruction<void>([]() {registerCheckPassed = (gXRegister == gYRegister); }), (MAX_PROB - (MIN_PROB + NOP_PROB)),
		Instructions::Instruction<void>([]() {registerCheckPassed = (gXRegister != gYRegister); }), MIN_PROB, NOP_PROB));

	gLookup.AddInstruction("#", new InstructionObject<void>(config, Instructions::Instruction<void>([]() {functionBuild.functionParseMode = true; }), MAX_PROB,
		Instructions::Instruction<void>(nop), 0, 0));
}

int main()
{
	gConfig = Configuration(MAX_PROB, MIN_PROB, NOP_PROB, FUNCTION_PROBABILITY_GATE, IF_BLOCK_PROBABILITY, MAX_INSTR);
	SetUpLookupTable(gConfig);

	//load the script
	std::ifstream file("test.prob");
	//std::ifstream file("testprob.prob");
	std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	//process the data
	Process(contents);

	//output the final probabilities(optional)

	while (!std::cin.get()) {}
	return 0;
}
#endif