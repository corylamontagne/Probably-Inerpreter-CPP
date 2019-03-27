#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <fstream>

char array[2046] = { 0 };
char *ptr = array;
unsigned seed = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count();
std::default_random_engine generator(seed);
std::uniform_real_distribution<> distribution(0.0, 100.0);

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
	InstructionObject(std::pair<Instruction*, float> instruction1, std::pair<Instruction*, float> instruction2) { PossibilitySet = { std::make_pair(new NOP(), 0.0), instruction1, instruction2 }; }
	~InstructionObject() { std::for_each(PossibilitySet.begin(), PossibilitySet.end(), [](std::pair<Instruction*, float> &n) { delete n.first; }); }

	double operator()()
	{
		double probability = distribution(generator);
		if (probability <= PossibilitySet[2].second)
		{
			(*PossibilitySet[2].first)();
		}
		else if (probability >= PossibilitySet[1].second)
		{
			(*PossibilitySet[1].first)();
		}
		else
		{
			(*PossibilitySet[0].first)();
		}
		return (100 - PossibilitySet[1].second)/100;
	}
private:
	InstructionObject() = delete;
	std::vector<std::pair<Instruction*, float> > PossibilitySet;
};

int main()
{
	InstructionObject Increment(std::make_pair(new Add(), 0.3), std::make_pair(new Sub(), 0.1));
	InstructionObject Move(std::make_pair(new MovF(), 0.3), std::make_pair(new MovB(), 0.1));

	std::ifstream t("..\\test.prob");
	std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

	double probability = 1;
	for (std::string::iterator it = str.begin(); it != str.end(); ++it)
	{
		switch (*it)
		{
		case 'i':
			probability *= Increment();
			break;
		case 'm':
			probability *= Move();
			break;
		case 'o':
			std::cout << *ptr;
		default:
			break;
		}
	}
	std::cout << std::endl << "Probability: " << probability * 100 << "%" << std::endl;

	t.close();

	while (!std::cin.get()) {}
	return 0;
}