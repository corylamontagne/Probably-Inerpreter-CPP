#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <fstream>
#include <iomanip>

//SETTINGS
#define MAX_PROB 1000000000
#define MIN_PROB 500
#define NOP_PROB 100
#define MAX_INSTR 50

//GLOBAL CONFIG
char array[2046] = { 0 };
char *ptr = array;
unsigned seed = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count();
std::default_random_engine generator(seed);
std::uniform_int_distribution<> distribution(0, MAX_PROB);

//TODO: This has to be fixed to account for frequent mode swapping. That in conjunction with
//the auto mode return to FLAT causes problems.
//CASE: +i-i+i-i-iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiof
//The last DOWN mode instruction does not return to FLAT, the previous instructino already did that
//This returns us to DOWN mode for the duration of MAX_INSTR. NO BUENO!
//Rework how instruction counts work.
int instructionCount = 0;

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

	std::pair<int, int> operator()()
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
		return std::make_pair(PossibilitySet[1].second + modifier, occured + modifier);
	}
private:
	InstructionObject() = delete;
	std::vector<std::pair<Instruction*, int> > PossibilitySet;
};

void IncrementInstructionCount() 
{ 
	if (mode == UP)
	{
		instructionCount++;
	}
	if (mode == DOWN)
	{
		instructionCount--;
	}
}

int main()
{
	int high = MAX_PROB - MIN_PROB;
	int low = MAX_PROB - (high + NOP_PROB);

	InstructionObject Increment(std::make_pair(new Add(), high), std::make_pair(new Sub(), low));
	InstructionObject MoveForward(std::make_pair(new MovF(), high), std::make_pair(new MovB(), low));
	InstructionObject Decrement(std::make_pair(new Sub(), high), std::make_pair(new Add(), low));
	InstructionObject MoveBackward(std::make_pair(new MovB(), high), std::make_pair(new MovF(), low));

	//std::ifstream file("test.prob");
	std::ifstream file("testprob.prob");
	std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	double probability = 1;
	double occuredProbabiltiy = 1;

	//TODO: Fuck this temporary parsing. We need something legit that allows us to create multi character instructions AND functions.
	for (std::string::iterator it = contents.begin(); it != contents.end(); ++it)
	{
		bool set = true, frame1 = false;
		std::pair<int, int> probs;
		switch (*it)
		{
		case 'i':
			probs = Increment();
			IncrementInstructionCount();
			break;
		case 'd':
			probs = Decrement();
			IncrementInstructionCount();
			break;
		case 'f':
			probs = MoveForward();
			IncrementInstructionCount();
			break;
		case 'b':
			probs = MoveBackward();
			IncrementInstructionCount();
			break;
		case 'o':
			set = false;
			std::cout << *ptr;
			break;
		case '+':
		{
			set = false;
			if (mode == FLAT || (instructionCount > 0 && mode == DOWN))
			{
				mode = UP;
				frame1 = true;
			}
			else
			{
				if (instructionCount > 0)
				{
					mode = DOWN;
				}
				else
				{
					mode = FLAT;
				}
			}
		}
		break;
		case '-':
		{
			set = false;
			if (mode == FLAT || (instructionCount > 0 && mode == UP))
			{
				mode = DOWN;
				frame1 = true;
			}
			else
			{
				if (instructionCount > 0)
				{
					mode = UP;
				}
				else
				{
					mode = FLAT;
				}
			}
		}
		break;
		default:
			set = false;
			break;
		}

		if (!frame1 && instructionCount == 0 && mode != FLAT)
		{
			mode = FLAT;
		}
		if (instructionCount == MAX_INSTR)
		{
			if (mode == UP)
			{
				mode = DOWN;
			}
			else if (mode == DOWN)
			{
				mode = UP;
			}
		}

		//only modify probabilties if the instruction was probabiistic
		if (set)
		{
			probability *= (double)probs.first / (double)MAX_PROB;
			occuredProbabiltiy *= (double)probs.second / (double)MAX_PROB;
		}
	}
	std::cout << std::endl;
	std::cout << std::endl << std::setprecision(20) << "Intended Probability: " << probability * 100  << "%" << std::endl;
	std::cout << std::setprecision(20) << "Chances of actual outcome: " << occuredProbabiltiy * 100 << "%" << std::endl;

	file.close();

	while (!std::cin.get()) {}
	return 0;
}