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
#define MAX_INSTR 50
#define FUNCTION_PROBABILITY_GATE 10000

//GLOBAL CONFIG
char array[2046] = { 0 };
char *ptr = array;

double mainProbability = 1;
double mainOccuredProbabiltiy = 1;
bool functionMode = false, functionName = false;
std::map<std::string, std::string> functions;
std::string currentFunctionName = "", executingFunctionName = "";

unsigned seed = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count();
std::default_random_engine generator(seed);
std::uniform_int_distribution<> distribution(0, MAX_PROB);

int instructionCount = 0, lastInstructionCount = 0;

enum ProbabilityMode
{
	UP = 0,
	DOWN,
	FLAT,
	GATE_UP,
	GATE_DOWN
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
		if (probability <= PossibilitySet[2].second)
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
					//determine chance of probability gate
					int probability = distribution(generator);
					if (probability <= FUNCTION_PROBABILITY_GATE)
					{
						//TODO: determine how to alter the function probability range
					}
					bool res = ProcessScript(iter->second);
					//TODO: Reset the probability gate

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
	std::cout << std::endl << std::setprecision(20) << "Intended Probability: " << mainProbability * 100  << "%" << std::endl;
	std::cout << std::setprecision(20) << "Chances of actual outcome: " << mainOccuredProbabiltiy * 100 << "%" << std::endl;

	file.close();

	while (!std::cin.get()) {}
	return 0;
}