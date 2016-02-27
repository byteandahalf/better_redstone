#pragma once
class Block;
struct BlockPos;
class BlockEntity;
class Level;
class Material;

#include "../../CommonTypes.h"

struct BlockSource
{
	Level* getLevel() const;
	Block* getBlockPtr(const BlockPos&);
	BlockID getBlock(const BlockPos&);
	FullBlock getBlockID(const BlockPos&);
	DataID getData(const BlockPos&);
	Material& getMaterial(const BlockPos&);
	bool setBlockAndData(int, int, int, BlockID, unsigned char, int);
	bool setBlockAndData(const BlockPos&, FullBlock, int);
	void removeBlock(const BlockPos&);
	void updateNeighborsAt(const BlockPos&, BlockID);
	BlockEntity* getBlockEntity(const BlockPos&);

	bool isBlockIndirectlyGettingPowered(const BlockPos&);
	int isBlockProvidingPowerTo(const BlockPos&, int);
	int getIndirectPowerLevelTo(const BlockPos&, int);
	int getBlockPowerInput(const BlockPos&);
	bool getIndirectPowerOutput(const BlockPos&, int);
	bool isBlockGettingPowered(const BlockPos&);
	int getStrongestIndirectPower(const BlockPos&);
};
