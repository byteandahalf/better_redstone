#include <jni.h>
#include <dlfcn.h>
#include <android/log.h>
#include <stdlib.h>
#include <substrate.h>
#include <memory>

#include "offsets.h"
#include "virtualhook.h"

#include "minecraftpe/world/level/block/Block.h"

#include "redstonehooks/RedStoneWireBlock.h"


static int redstone_stuff[32] = 
{
	/* recievers */
	46,
	25,
	154,
	123,
	124,
	23,
	125,
	64,
	71,
	96,
	167,
	107,
	183,
	184,
	185,
	186,
	187,
	193,
	194,
	195,
	196,
	197,
	66,
	27,
	126,
	/* providers */
	55,
	93,
	94,
	149,
	150,
	75,
	76
};

static void disableRedstoneUpdates()
{
	/* should disable all redstone recievers */
	return;
}

static void _stopTicks()
{
	/* don't tick the redstone system */
	return;
}

static void (*_initBlocks)();
static void initBlockVtables()
{
	_initBlocks();

	for(int block_id : redstone_stuff)
	{
		uintptr_t** block_vtable = VirtualHook::GetVtable((void*) Block::mBlocks[block_id]);
		block_vtable[BLOCK_VTABLE_ON_REDSTONE_UPDATE] = (uintptr_t*) &disableRedstoneUpdates;
	}

	RedStoneWireBlock::initVtable(Block::mRedStoneDust);
}


JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
	void* stopTicks = dlsym(RTLD_DEFAULT, "_ZN13CircuitSystem8evaluateEv");
	MSHookFunction(stopTicks, (void*) &_stopTicks, (void**) NULL);
	MSHookFunction((void*) &Block::initBlocks, (void*) &initBlockVtables, (void**) &_initBlocks);
	
	return JNI_VERSION_1_2;
}
