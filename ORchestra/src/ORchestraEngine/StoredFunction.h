#pragma once

#include <vector>

#include "Instruction.h"

namespace ORchestra
{
	class StoredFunction
	{
	public:
		StoredFunction() : mNumOfParams(0), mInstructions() {}
		StoredFunction(int numOfParams, std::vector<Instruction>& instructions) : mNumOfParams(numOfParams),
			mInstructions(instructions)
		{
		}

		int mNumOfParams;
		std::vector<Instruction> mInstructions {};
	};
} // namespace ORchestra
