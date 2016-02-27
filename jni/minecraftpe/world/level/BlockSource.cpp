#include <cmath>

#include "BlockPos.h"
#include "../Facing.h"
#include "block/Block.h"

#include "offsets.h"
#include "virtualhook.h"

#include "BlockSource.h"

bool BlockSource::isBlockIndirectlyGettingPowered(const BlockPos& pos) {
	int x = pos.x, y = pos.y, z = pos.z;

	if(getIndirectPowerLevelTo({x, y - 1, z}, 0) > 0)
	{
		return true;
	}
	if(getIndirectPowerLevelTo({x, y + 1, z}, 1) > 0)
	{
		return true;
	}
	if(getIndirectPowerLevelTo({x, y, z - 1}, 2) > 0)
	{
		return true;
	}
	if(getIndirectPowerLevelTo({x, y, z + 1}, 3) > 0)
	{
		return true;
	}
	if(getIndirectPowerLevelTo({x - 1, y, z}, 4) > 0)
	{
		return true;
	}
	return getIndirectPowerLevelTo({x + 1, y, z}, 5) > 0;
}

int BlockSource::getIndirectPowerLevelTo(const BlockPos& pos, int side) {
	int blockId = getBlock(pos);
	if(blockId == 0)
	{
		return 0;
	}
	if(Block::mSolid[blockId] && blockId != Block::mRedstoneBlock->blockId)
	{
		return getBlockPowerInput(pos);
	}
	else return Block::mBlocks[blockId]->getDirectSignal(*this, pos, side);
}

int BlockSource::getStrongestIndirectPower(const BlockPos& pos) {
	int x = pos.x, y = pos.y, z = pos.z;
	int var4 = 0;

	for (int var5 = 0; var5 < 6; ++var5)
	{
		int var6 = getIndirectPowerLevelTo({x + Facing::STEP_X[var5], y + Facing::STEP_Y[var5], z + Facing::STEP_Z[var5]}, var5);

		if(var6 >= 15)
		{
			return 15;
		}
		if(var6 > var4)
		{
			var4 = var6;
		}
	}
	return var4;
}

int BlockSource::isBlockProvidingPowerTo(const BlockPos& pos, int side) {
	int blockId = getBlock(pos);
	if(blockId == 0)
	{
		return 0;
	}
	else
	{
		uintptr_t** vtable = VirtualHook::GetVtable((void*) Block::mBlocks[blockId]);
		int (*getSignal)(Block*, BlockSource&, const BlockPos&, int) = (int (*)(Block*, BlockSource&, const BlockPos&, int)) vtable[BLOCK_VTABLE_ON_REDSTONE_UPDATE];
		return getSignal(Block::mBlocks[blockId], *this, pos, side);
	}
}

bool BlockSource::getIndirectPowerOutput(const BlockPos& pos, int side) {
	return getIndirectPowerLevelTo(pos, side) > 0;
}

bool BlockSource::isBlockGettingPowered(const BlockPos& pos) {
	int x = pos.x, y = pos.y, z = pos.z;

	if(isBlockProvidingPowerTo({x, y - 1, z}, 0))
	{
		return true;
	}
	if(isBlockProvidingPowerTo({x, y + 1, z}, 1))
	{
		return true;
	}
	if(isBlockProvidingPowerTo({x, y, z - 1}, 2))
	{
		return true;
	}
	if(isBlockProvidingPowerTo({x, y, z + 1}, 3))
	{
		return true;
	}
	if(isBlockProvidingPowerTo({x - 1, y, z}, 4))
	{
		return true;
	}
	return isBlockProvidingPowerTo({x + 1, y, z}, 5);
}

int BlockSource::getBlockPowerInput(const BlockPos& pos) {
	int x = pos.x, y = pos.y, z = pos.z;
	int var4 = 0;
	int var5 = std::max(var4, isBlockProvidingPowerTo({x, y - 1, z}, 0));

	if(var5 >= 15)
	{
		return var5;
	}
	else {
		var5 = std::max(var5, isBlockProvidingPowerTo({x, y + 1, z}, 1));

		if(var5 >= 15)
		{
			return var5;
		}
		else {
			var5 = std::max(var5, isBlockProvidingPowerTo({x, y, z - 1}, 2));

			if(var5 >= 15)
			{
				return var5;
			}
			else {
				var5 = std::max(var5, isBlockProvidingPowerTo({x, y, z + 1}, 3));

				if(var5 >= 15)
				{
					return var5;
				}
				else {
					var5 = std::max(var5, isBlockProvidingPowerTo({x - 1, y, z}, 4));

					if(var5 >= 15)
					{
						return var5;
					}
					else {
						var5 = std::max(var5, isBlockProvidingPowerTo({x + 1, y, z}, 5));
						return var5 >= 15 ? var5 : var5;
					}
				}
			}
		}
	}
}
