#pragma once

#ifndef MATTYPES_INCLUDED
#define MATTYPES_INCLUDED
#endif // !MATTYPES_INCLUDED


#pragma region Mat Types



#define MAT_TYPE_GENERIC 0x0

#define MAT_TYPE_WOOD 0x1

#define MAT_TYPE_WOOD_CRATE 0x2

#define MAT_TYPE_WOOD_PLANK 0x3

#define MAT_TYPE_WOOD_BOX 0x4

#define MAT_TYPE_WOOD_CRATE_LIGHT 0x5

#define MAT_TYPE_WOOD_CRATE_HARD 0x6

#define MAT_TYPE_WOOD_PLANK_LIGHT 0x7

#define MAT_TYPE_WOOD_PLANK_HARD 0x8

#define MAT_TYPE_WOOD_BOX_LIGHT 0x9

#define MAT_TYPE_WOOD_BOX_HARD 0xa

#define MAT_TYPE_PLASTIC 0xb

#define MAT_TYPE_PLASTIC_BARREL 0xc

#define MAT_TYPE_PLASTIC_BOX 0xd

#define MAT_TYPE_PLASTIC_BARREL_LIGHT 0xe

#define MAT_TYPE_PLASTIC_BARREL_HARD 0xf

#define MAT_TYPE_PLASTIC_BOX_LIGHT 0x10

#define MAT_TYPE_PLASTIC_BOX_HARD 0x11

#define MAT_TYPE_METAL 0xc

#pragma endregion

#pragma region Mat Types Names

#define MAT_TYPE_WOOD_CRATE_LIGHT_NAME "wood_crate_scrape_rough_loop1"

#define MAT_TYPE_WOOD_CRATE_HARD_NAME "wood_crate_scrape_rough_loop1"

#define MAT_TYPE_WOOD_PLANK_LIGHT_NAME "wood_plank_scrape_light_loop1"

#define MAT_TYPE_WOOD_PLANK_HARD_NAME "wood_plank_scrape_rough_loop1"

#define MAT_TYPE_WOOD_BOX_LIGHT_NAME "wood_box_scrape_light_loop1"

#define MAT_TYPE_WOOD_BOX_HARD_NAME "wood_box_scrape_rough_loop1"

#define MAT_TYPE_PLASTIC_BARREL_LIGHT_NAME "plastic_barrel_scrape_smooth_loop1"

#define MAT_TYPE_PLASTIC_BARREL_HARD_NAME "plastic_barrel_scrape_rough_loop1"

#define MAT_TYPE_PLASTIC_BOX_LIGHT_NAME "plastic_box_scrape_smooth_loop1"

#define MAT_TYPE_PLASTIC_BOX_HARD_NAME "plastic_box_scrape_rough_loop1"

#pragma endregion


#pragma region Footstep_Mat_Types

#define MAT_SOUND_TYPE_CONCRETE 0x0

#define MAT_SOUND_TYPE_CHAINLINK 0x1

#define MAT_SOUND_TYPE_DIRT 0x2

#define MAT_SOUND_TYPE_DUCT 0x3

#define MAT_SOUND_TYPE_GRASS 0x4

#define MAT_SOUND_TYPE_GRAVEL 0x5

#define MAT_SOUND_TYPE_LADDER 0x6

#define MAT_SOUND_TYPE_METAL 0x7

#define MAT_SOUND_TYPE_METALGRATE 0x8

#define MAT_SOUND_TYPE_MUD 0x9

#define MAT_SOUND_TYPE_SAND 0xa

#define MAT_SOUND_TYPE_SLOSH 0xb

#define MAT_SOUND_TYPE_TILE 0xd

#define MAT_SOUND_TYPE_WADE 0xe

#define MAT_SOUND_TYPE_WOOD 0xf

#define MAT_SOUND_TYPE_WOODPANEL 0x10

#pragma endregion

#pragma region Footstep_Mat_Types_Paths

#define MAT_SOUND_TYPE_CONCRETE_NAME "footstep_concrete"

#define MAT_SOUND_TYPE_CHAINLINK_NAME "footstep_chainlink"

#define MAT_SOUND_TYPE_DIRT_NAME "footstep_dirt"

#define MAT_SOUND_TYPE_DUCT_NAME "footstep_duct"

#define MAT_SOUND_TYPE_GRASS_NAME "footstep_grass"

#define MAT_SOUND_TYPE_GRAVEL_NAME "footstep_gravel"

#define MAT_SOUND_TYPE_LADDER_NAME "footstep_ladder"

#define MAT_SOUND_TYPE_METAL_NAME "footstep_metal"

#define MAT_SOUND_TYPE_METALGRATE_NAME "footstep_metalgrate"

#define MAT_SOUND_TYPE_MUD_NAME "footstep_mud"

#define MAT_SOUND_TYPE_SAND_NAME "footstep_sand"

#define MAT_SOUND_TYPE_SLOSH_NAME "footstep_slosh"

#define MAT_SOUND_TYPE_TILE_NAME "footstep_tile"

#define MAT_SOUND_TYPE_WADE_NAME "footstep_wade"

#define MAT_SOUND_TYPE_WOOD_NAME "footstep_wood"

#define MAT_SOUND_TYPE_WOODPANEL_NAME "footstep_woodpanel"

#pragma endregion

//unused
enum MAT_TYPE
{
	GENERIC,

	WOOD,

	WOOD_CRATE,

	WOOD_PLANK,

	WOOD_BOX,

	WOOD_CRATE_LIGHT,

	WOOD_CRATE_HARD,

	WOOD_PLANK_LIGHT,

	WOOD_PLANK_HARD,

	WOOD_BOX_LIGHT,

	WOOD_BOX_HARD,

	PLASTIC,

	METAL
};