/*
 * Copyright (C) 2026 Christian Tronhjem
 *
 * This file is part of ORchestra.
 *
 * ORchestra is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ORchestra is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with ORchestra. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <vector>

#include "Instruction.h"

namespace ORchestra
{
	class StoredFunction
	{
	public:
		StoredFunction() : mNumOfParams(0), mHasReturnValue(false), mParamIds(), mInstructions()
		{
		}

		StoredFunction(const int numOfParams, std::vector<Instruction> instructions, const bool hasReturn = false) :
			mNumOfParams(numOfParams),
			mHasReturnValue(hasReturn),
			mInstructions(std::move(instructions))
		{
		}

		StoredFunction(const int numOfParams, std::vector<DataUnit> paramIds, std::vector<Instruction> instructions, const bool hasReturn = false) :
			mNumOfParams(numOfParams),
			mHasReturnValue(hasReturn),
			mParamIds(std::move(paramIds)),
			mInstructions(std::move(instructions))
		{
		}

		int mNumOfParams;
		bool mHasReturnValue;
		std::vector<DataUnit> mParamIds {};
		std::vector<Instruction> mInstructions {};
	};
} // namespace ORchestra
