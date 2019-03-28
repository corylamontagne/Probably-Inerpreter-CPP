#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <fstream>
#include <iomanip>

#define MAX_PROB 10000000
#define MIN_PROB 300
#define NOP_PROB 100
char array[2046] = { 0 };
char *ptr = array;
unsigned seed = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count();
std::default_random_engine generator(seed);
std::uniform_int_distribution<> distribution(0, MAX_PROB);

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
		return std::make_pair(PossibilitySet[1].second, occured);
	}
private:
	InstructionObject() = delete;
	std::vector<std::pair<Instruction*, int> > PossibilitySet;
};

int main()
{
	int high = MAX_PROB - MIN_PROB;
	int low = MAX_PROB - (high + NOP_PROB);
	InstructionObject Increment(std::make_pair(new Add(), high), std::make_pair(new Sub(), low));
	InstructionObject MoveForward(std::make_pair(new MovF(), high), std::make_pair(new MovB(), low));
	InstructionObject Decrement(std::make_pair(new Sub(), high), std::make_pair(new Add(), low));
	InstructionObject MoveBackward(std::make_pair(new MovB(), high), std::make_pair(new MovF(), low));

	std::ifstream file("test.prob");
	std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	double probability = 1;
	double occuredProbabiltiy = 1;

	for (std::string::iterator it = contents.begin(); it != contents.end(); ++it)
	{
		std::pair<int, int> probs;
		switch (*it)
		{
		case 'i':
		{
			probs = Increment();
			probability *= (double)probs.first / (double)MAX_PROB;
			occuredProbabiltiy *= (double)probs.second / (double)MAX_PROB;
		}
			break;
		case 'd':
		{
			probs = Decrement();
			probability *= (double)probs.first / (double)MAX_PROB;
			occuredProbabiltiy *= (double)probs.second / (double)MAX_PROB;
		}
			break;
		case 'f':
		{
			probs = MoveForward();
			probability *= (double)probs.first / (double)MAX_PROB;
			occuredProbabiltiy *= (double)probs.second / (double)MAX_PROB;
		}
			break;
		case 'b':
		{
			probs = MoveBackward();
			probability *= (double)probs.first / (double)MAX_PROB;
			occuredProbabiltiy *= (double)probs.second / (double)MAX_PROB;
		}
			break;
		case 'o':
		{
			std::cout << *ptr;
		}
			break;
		case '{':
		{
		}
		break;
		case '}':
		{
		}
		break;
		default:
			break;
		}
	}

	std::cout << std::endl << std::setprecision(20) << "Intended Probability: " << probability * 100  << "%" << std::endl;
	std::cout << std::endl << std::setprecision(20) << "Chances of outcome: " << occuredProbabiltiy * 100 << "%" << std::endl;

	file.close();

	while (!std::cin.get()) {}
	return 0;
}