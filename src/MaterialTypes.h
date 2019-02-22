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


#define MAT_TYPE_METAL 18

#define MAT_TYPE_METAL_CANISTER 19

#define MAT_TYPE_METAL_CANISTER_LIGHT 20

#define MAT_TYPE_METAL_CANISTER_HARD 21

#define MAT_TYPE_METAL_CHAINLINK 22

//#define MAT_TYPE_METAL_CHAINLINK_LIGHT 23

#define MAT_TYPE_METAL_CHAINLINK_HARD 24

#define MAT_TYPE_METAL_COMPUTER 25

#define MAT_TYPE_METAL_COMPUTER_LIGHT 26

#define MAT_TYPE_METAL_COMPUTER_HARD 27

#define MAT_TYPE_METAL_GRATE 28

#define MAT_TYPE_METAL_GRATE_LIGHT 29

#define MAT_TYPE_METAL_GRATE_HARD 30

#define MAT_TYPE_METAL_GRENADE 31

#define MAT_TYPE_METAL_GRENADE_LIGHT 32

#define MAT_TYPE_METAL_GRENADE_HARD 33

#define MAT_TYPE_METAL_SHEET 34

#define MAT_TYPE_METAL_SHEET_LIGHT 35

#define MAT_TYPE_METAL_SHEET_HARD 36

#define MAT_TYPE_METAL_WEAPON 37

#define MAT_TYPE_METAL_SODA_CAN 38

#define MAT_TYPE_METAL_SODA_CAN_LIGHT 39

//#define MAT_TYPE_METAL_SODA_CAN_HARD  40

#define MAT_TYPE_METAL_BARREL 41

#define MAT_TYPE_METAL_BARREL_LIGHT 42

#define MAT_TYPE_METAL_BARREL_HARD 43

#define MAT_TYPE_METAL_BOX 44

#define MAT_TYPE_METAL_BOX_LIGHT 45

#define MAT_TYPE_METAL_BOX_HARD 46

#define MAT_TYPE_ROCK 47

#define MAT_TYPE_CONCRETE_BLOCK 48

#define MAT_TYPE_CONCRETE 49

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



#define MAT_TYPE_METAL_CANISTER_LIGHT_NAME "metal_box_scrape_smooth_loop1"

#define MAT_TYPE_METAL_CANISTER_HARD_NAME "metal_box_scrape_rough_loop1"



#define MAT_TYPE_METAL_CHAINLINK_HARD_NAME "metal_chainlink_scrape_rough_loop1"



#define MAT_TYPE_METAL_COMPUTER_LIGHT_NAME "metal_box_scrape_smooth_loop1"

#define MAT_TYPE_METAL_COMPUTER_HARD_NAME "metal_box_scrape_rough_loop1"



#define MAT_TYPE_METAL_GRATE_LIGHT_NAME "metal_box_scrape_smooth_loop1"

#define MAT_TYPE_METAL_GRATE_HARD_NAME "metal_box_scrape_rough_loop1"


#define MAT_TYPE_METAL_GRENADE_LIGHT_NAME "metal_grenade_scrape_smooth_loop1"

#define MAT_TYPE_METAL_GRENADE_HARD_NAME "metal_grenade_scrape_rough_loop1"


#define MAT_TYPE_METAL_SHEET_LIGHT_NAME "metal_box_scrape_smooth_loop1"

#define MAT_TYPE_METAL_SHEET_HARD_NAME "metal_box_scrape_rough_loop1"



#define MAT_TYPE_METAL_SODA_CAN_LIGHT_NAME  "soda_can_scrape_rough_loop1"


#define MAT_TYPE_METAL_BARREL_LIGHT_NAME "metal_box_scrape_smooth_loop1"

#define MAT_TYPE_METAL_BARREL_HARD_NAME "metal_box_scrape_rough_loop1"



#define MAT_TYPE_METAL_BOX_LIGHT_NAME "metal_box_scrape_smooth_loop1"

#define MAT_TYPE_METAL_BOX_HARD_NAME "metal_box_scrape_rough_loop1"


#define MAT_TYPE_ROCK_NAME "rock_scrape_rough_loop1"

#define MAT_TYPE_CONCRETE_BLOCK_NAME "concrete_block_scrape_rough_loop1"

#define MAT_TYPE_CONCRETE_NAME "concrete_scrape_smooth_loop1"

#pragma endregion

#pragma region Mat types Bullet Impact sound names

#define MAT_TYPE_WOOD_IMPACT_BULLET_SOUND_NAME "wood_box_impact_bullet"

#define MAT_TYPE_WOOD_CRATE_IMPACT_BULLET_SOUND_NAME "wood_box_impact_bullet"

#define MAT_TYPE_WOOD_PLANK_IMPACT_BULLET_SOUND_NAME "wood_box_impact_bullet"

#define MAT_TYPE_WOOD_BOX_IMPACT_BULLET_SOUND_NAME "wood_box_impact_bullet"


#define MAT_TYPE_PLASTIC_BOX_IMPACT_BULLET_SOUND_NAME "plastic_box_impact_bullet"

#define MAT_TYPE_PLASTIC_BARREL_IMPACT_BULLET_SOUND_NAME "plastic_barrel_impact_bullet"


#define MAT_TYPE_METAL_IMPACT_BULLET_SOUND_NAME "metal_solid_impact_bullet"

#define MAT_TYPE_METAL_SHEET_IMPACT_BULLET_SOUND_NAME "metal_sheet_impact_bullet"

#define MAT_TYPE_METAL_COMPUTER_IMPACT_BULLET_SOUND_NAME "metal_computer_impact_bullet"

#define MAT_TYPE_METAL_BOX_IMPACT_BULLET_SOUND_NAME "metal_box_impact_bullet"

#define MAT_TYPE_METAL_CHAINLINK_IMPACT_BULLET_SOUND_NAME "metal_chainlink_impact_hard"


#define MAT_TYPE_CONCRETE_IMPACT_BULLET_SOUND_NAME "concrete_impact_bullet"

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

#define MAT_SOUND_TYPE_METAL_BOX 0x11

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