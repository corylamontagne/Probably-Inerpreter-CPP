#pragma once
#include "LookupTable.h"
#include <string>

//TODO: Eventually all this information needs a proper place to live.
//this is its home for the time being until the important things are done
char gXRegister = 0, gYRegister = 0;
bool registerCheckPassed = false;
LookupTable gLookup;
bool gFunctionParseMode = false;


unsigned gSeed = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count();
std::default_random_engine gGenerator(gSeed);
std::uniform_int_distribution<> gDistribution(0, MAX_PROB);