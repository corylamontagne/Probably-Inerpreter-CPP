#include "LookupTable.h"

LookupTable::LookupTable()
{
}

LookupTable::~LookupTable()
{
}

bool LookupTable::AddInstruction(std::string identifier, BaseInstruction*instruction)
{
	if (instruction && !identifier.empty())
	{
		if (mInstructionTable.find(identifier) == mInstructionTable.end())
		{
			mInstructionTable.insert(std::make_pair(identifier, instruction));
			return true;
		}
	}
	return false;
}

BaseInstruction* LookupTable::FetchInstruction(const std::string &identifier)
{
	if (!identifier.empty())
	{
		auto iter = mInstructionTable.find(identifier);
		if (iter != mInstructionTable.end())
		{
			return iter->second;
		}
	}
	return nullptr;
}