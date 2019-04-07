#pragma once
#include <unordered_map>
#include <string>
#include "InstructionObject.h"

class LookupTable
{
public:
	LookupTable();
	~LookupTable();

	bool AddInstruction(std::string, BaseInstruction*);
	BaseInstruction* FetchInstruction(const std::string &identifier);
private:
	std::unordered_map<std::string, BaseInstruction*> mInstructionTable;
};

