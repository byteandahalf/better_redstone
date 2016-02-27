#include "minecraftpe/world/level/BlockSource.h"
#include "minecraftpe/world/level/BlockPos.h"
#include "minecraftpe/world/entity/player/Player.h"

#include "RedStoneWireBlock.h"

#include "offsets.h"
#include "virtualhook.h"

void RedStoneWireBlock::initVtable(Block* self)
{
	uintptr_t** vtable = VirtualHook::GetVtable((void*) self);
	vtable[BLOCK_VTABLE_ON_PLACE] = (uintptr_t*) &_onPlace;
	vtable[BLOCK_VTABLE_PLAYER_DESTROY] = (uintptr_t*) &_playerDestroy;
	vtable[BLOCK_VTABLE_NEIGHBOR_CHANGED] = (uintptr_t*) &_neighborChanged;
	vtable[BLOCK_VTABLE_IS_SIGNAL_SOURCE] = (uintptr_t*) &_isSignalSource;
	vtable[BLOCK_VTABLE_ON_REDSTONE_UPDATE] = (uintptr_t*) &_getSignal;
	vtable[BLOCK_VTABLE_GET_DIRECT_SIGNAL] = (uintptr_t*) &_getDirectSignal;
}

void RedStoneWireBlock::_onPlace(RedStoneWireBlock* self, BlockSource& region, const BlockPos& pos)
{
	//Block::onPlace(region, pos);
	//int x = pos.x, y = pos.y, z = pos.z;
	FullBlock silly(3, 1);
	region.setBlockAndData(pos, silly, 2);
	return;

	int x = pos.x, y = pos.y, z = pos.z;

	recalculate(self, region, pos);
	region.updateNeighborsAt({x, y + 1, z}, self->blockId);
	region.updateNeighborsAt({x, y - 1, z}, self->blockId);
	updateWires(self, region, {x - 1, y, z});
	updateWires(self, region, {x + 1, y, z});
	updateWires(self, region, {x, y, z - 1});
	updateWires(self, region, {x, y, z + 1});

	if(Block::mSolid[region.getBlock({x - 1, y, z})])
		updateWires(self, region, {x - 1, y + 1, z});
	else
		updateWires(self, region, {x - 1, y - 1, z});

	if(Block::mSolid[region.getBlock({x + 1, y, z})])
		updateWires(self, region, {x + 1, y + 1, z});
	else
		updateWires(self, region, {x + 1, y - 1, z});

	if(Block::mSolid[region.getBlock({x, y, z - 1})])
		updateWires(self, region, {x, y + 1, z - 1});
	else
		updateWires(self, region, {x, y - 1, z - 1});

	if(Block::mSolid[region.getBlock({x, y, z + 1})])
		updateWires(self, region, {x, y + 1, z + 1});
	else
		updateWires(self, region, {x, y - 1, z + 1});
}

void RedStoneWireBlock::_playerDestroy(RedStoneWireBlock* self, Player* player, const BlockPos& pos, int side)
{
	int x = pos.x, y = pos.y, z = pos.z;

	player->region.updateNeighborsAt({x, y + 1, z}, self->blockId);
	player->region.updateNeighborsAt({x, y - 1, z}, self->blockId);
	player->region.updateNeighborsAt({x + 1, y, z}, self->blockId);
	player->region.updateNeighborsAt({x - 1, y, z}, self->blockId);
	player->region.updateNeighborsAt({x, y, z + 1}, self->blockId);
	player->region.updateNeighborsAt({x, y, z - 1}, self->blockId);
	recalculate(self, player->region, {x, y, z});
	updateWires(self, player->region, {x - 1, y, z});
	updateWires(self, player->region, {x + 1, y, z});
	updateWires(self, player->region, {x, y, z - 1});
	updateWires(self, player->region, {x, y, z + 1});

	if(Block::mSolid[player->region.getBlock({x - 1, y, z})])
		updateWires(self, player->region, {x - 1, y + 1, z});
	else
		updateWires(self, player->region, {x - 1, y - 1, z});

	if(Block::mSolid[player->region.getBlock({x + 1, y, z})])
		updateWires(self, player->region, {x + 1, y + 1, z});
	else
		updateWires(self, player->region, {x + 1, y - 1, z});

	if(Block::mSolid[player->region.getBlock({x, y, z - 1})])
		updateWires(self, player->region, {x, y + 1, z - 1});
	else
		updateWires(self, player->region, {x, y - 1, z - 1});

	if(Block::mSolid[player->region.getBlock({x, y, z + 1})])
		updateWires(self, player->region, {x, y + 1, z + 1});
	else
		updateWires(self, player->region, {x, y - 1, z + 1});
		
	//Block::playerDestroy(player, pos, side);
}

void RedStoneWireBlock::_neighborChanged(RedStoneWireBlock* self, BlockSource& region, const BlockPos& pos, const BlockPos& newPos)
{
	recalculate(self, region, pos);
	//Block::neighborChanged(region, pos, newPos);
}

bool RedStoneWireBlock::_isSignalSource(RedStoneWireBlock* self)
{
	return self->wiresProvidePower;
}

int RedStoneWireBlock::_getSignal(RedStoneWireBlock* self, BlockSource& region, const BlockPos& pos, int side)
{
	if(!self->wiresProvidePower)
	{
		return 0;
	}
	return _getDirectSignal(self, region, pos, side);
}

int RedStoneWireBlock::_getDirectSignal(RedStoneWireBlock* self, BlockSource& region, const BlockPos& pos, int side)
{
	if(!self->wiresProvidePower)
	{
		return 0;
	}

	int x = pos.x, y = pos.y, z = pos.z;
	int data = region.getData({x, y, z});

	if(data == 0)
	{
		return 0;
	}
	if(side == 1)
	{
		return data;
	}

	bool xm = self->shouldConnectTo(region, {x - 1, y, z}, 1) || !Block::mSolid[region.getBlock({x - 1, y, z})] && self->shouldConnectTo(region, {x - 1, y - 1, z}, -1);
	bool xp = self->shouldConnectTo(region, {x + 1, y, z}, 3) || !Block::mSolid[region.getBlock({x + 1, y, z})] && self->shouldConnectTo(region, {x + 1, y - 1, z}, -1);
	bool zm = self->shouldConnectTo(region, {x, y, z - 1}, 2) || !Block::mSolid[region.getBlock({x, y, z - 1})] && self->shouldConnectTo(region, {x, y - 1, z - 1}, -1);
	bool zp = self->shouldConnectTo(region, {x, y, z + 1}, 0) || !Block::mSolid[region.getBlock({x, y, z + 1})] && self->shouldConnectTo(region, {x, y - 1, z + 1}, -1);

	if(!Block::mSolid[region.getBlock({x, y + 1, z})])
	{
		if(Block::mSolid[region.getBlock({x - 1, y, z})] && self->shouldConnectTo(region, {x - 1, y + 1, z}, -1))
		{
			xm = true;
		}
		if(Block::mSolid[region.getBlock({x + 1, y, z})] && self->shouldConnectTo(region, {x + 1, y + 1, z}, -1))
		{
			xp = true;
		}
		if(Block::mSolid[region.getBlock({x, y, z - 1})] && self->shouldConnectTo(region, {x, y + 1, z - 1}, -1))
		{
			zm = true;
		}
		if(Block::mSolid[region.getBlock({x, y, z + 1})] && self->shouldConnectTo(region, {x, y + 1, z + 1}, -1))
		{
			zp = true;
		}
	}

	if(!zm && !xp && !xm && !zp && side >= 2 && side <= 5)
	{
		return data;
	}
	if(side == 2 && zm && !xm && !xp)
	{
		return data;
	}
	if(side == 3 && zp && !xm && !xp)
	{
		return data;
	}
	if(side == 4 && xm && !zm && !zp)
	{
		return data;
	}
	if(side == 5 && xp && !zm && !zp)
	{
		return data;
	}

	return 0;
}

void RedStoneWireBlock::calculateChanges(RedStoneWireBlock* self, BlockSource& region, const BlockPos& pos, const BlockPos& otherPos)
{
	int x = pos.x, y = pos.y, z = pos.z;
	int xx = otherPos.x, yy = otherPos.y, zz = otherPos.z;

	int oldPower = region.getData(pos);
	int newPower = getStrongerSignal(self, region, otherPos, 0);
	self->wiresProvidePower = false;
	int receivedPower = region.getStrongestIndirectPower(pos);
	self->wiresProvidePower = true;
	
	if(receivedPower > 0 && receivedPower > newPower - 1)
	{
		newPower = receivedPower;
	}
		
	int temp = 0;
		
	for(int it = 0; it < 4; ++it) {
		int newX = x;
		int newZ = z;
		
		if(it == 0)
		{
			newX = x - 1;
		}
		if(it == 1)
		{
			++newX;
		}
		if(it == 2)
		{
			newZ = z - 1;
		}
		if(it == 3)
		{
			++newZ;
		}
			
		if(newX != xx || newZ != zz)
		{
			temp = getStrongerSignal(self, region, {newX, y, newZ}, temp);
		}
			
		if(Block::mSolid[region.getBlock({newX, y, newZ})] && !Block::mSolid[region.getBlock({x, y + 1, z})])
		{
			if((newX != xx || newZ != zz) && y >= yy)
			{
				temp = getStrongerSignal(self, region, {newX, y + 1, newZ}, temp);
			}
		}
		else if(!Block::mSolid[region.getBlock({newX, y, newZ})] && (newX != xx || newZ != zz) && y <= yy)
		{
			temp = getStrongerSignal(self, region, {newX, y - 1, newZ}, temp);
		}
	}
	
	if(temp > newPower)
	{
		newPower = temp - 1;
	}
	else if(newPower > 0)
	{
		--newPower;
	}
	else
	{
		newPower = 0;
	}
		
	if(receivedPower > newPower - 1)
	{
		newPower = receivedPower;
	}
		
	if(oldPower != newPower)
	{
		region.setBlockAndData(x, y, z, self->blockId, 15, 2);
		region.updateNeighborsAt({x, y, z}, self->blockId);
		region.updateNeighborsAt({x - 1, y, z}, self->blockId);
		region.updateNeighborsAt({x + 1, y, z}, self->blockId);
		region.updateNeighborsAt({x, y - 1, z}, self->blockId);
		region.updateNeighborsAt({x, y + 1, z}, self->blockId);
		region.updateNeighborsAt({x, y, z - 1}, self->blockId);
		region.updateNeighborsAt({x, y, z + 1}, self->blockId);
	}
}

void RedStoneWireBlock::recalculate(RedStoneWireBlock* self, BlockSource& region, const BlockPos& pos)
{
	calculateChanges(self, region, pos, pos);
}

void RedStoneWireBlock::updateWires(RedStoneWireBlock* self, BlockSource& region, const BlockPos& pos)
{
	int x = pos.x, y = pos.y, z = pos.z;

	if(region.getBlock(pos) != self->blockId)
	{
		return;
	}

	region.updateNeighborsAt({x, y, z}, self->blockId);
	region.updateNeighborsAt({x - 1, y, z}, self->blockId);
	region.updateNeighborsAt({x + 1, y, z}, self->blockId);
	region.updateNeighborsAt({x, y, z - 1}, self->blockId);
	region.updateNeighborsAt({x, y, z + 1}, self->blockId);
	region.updateNeighborsAt({x, y - 1, z}, self->blockId);
	region.updateNeighborsAt({x, y + 1, z}, self->blockId);
}

int RedStoneWireBlock::getStrongerSignal(RedStoneWireBlock* self, BlockSource& region, const BlockPos& pos, int signal)
{
	if(region.getBlock(pos) != self->blockId)
	{
		return signal;
	}

	int signal2 = region.getData(pos);
	return (signal2 > signal)? signal2 : signal;
}
