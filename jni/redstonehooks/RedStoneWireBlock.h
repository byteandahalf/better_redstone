#pragma once

#include "minecraftpe/world/level/block/Block.h"

struct RedStoneWireBlock
: public Block
{
	bool wiresProvidePower;
	std::vector<int> textures;
	std::vector<int> moreTextures;

	bool shouldConnectTo(BlockSource&, const BlockPos&, int);

	static void initVtable(Block*);

	static void _onPlace(RedStoneWireBlock*, BlockSource&, const BlockPos&);
	static void _neighborChanged(RedStoneWireBlock*, BlockSource&, const BlockPos&, const BlockPos&);
	static void _playerDestroy(RedStoneWireBlock*, Player*, const BlockPos&, int);
	static bool _isSignalSource(RedStoneWireBlock*);
	static int _getSignal(RedStoneWireBlock*, BlockSource&, const BlockPos&, int);
	static int _getDirectSignal(RedStoneWireBlock*, BlockSource&, const BlockPos&, int);

	static void recalculate(RedStoneWireBlock*, BlockSource&, const BlockPos&);
	static void calculateChanges(RedStoneWireBlock*, BlockSource&, const BlockPos&, const BlockPos&);
	static void updateWires(RedStoneWireBlock*, BlockSource&, const BlockPos&);
	static int getStrongerSignal(RedStoneWireBlock*, BlockSource&, const BlockPos&, int);
};
