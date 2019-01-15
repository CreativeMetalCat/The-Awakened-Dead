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