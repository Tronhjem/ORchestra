#pragma once
#include <string>

namespace ORchestra {


class StoredFunction
{
public:
	StoredFunction() {}
	StoredFunction(int numOfParams, std::vector<Instruction> &instructions) : mNumOfParams(numOfParams),
																			  mInstructions(instructions)
	{
	}

	int mNumOfParams;
	std::vector<Instruction> mInstructions;
};


} // namespace ORchestra