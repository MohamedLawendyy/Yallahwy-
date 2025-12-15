#pragma once

// -------- UNITS --------
const float PIXELS_PER_METER = 64.0f;
const float TILE_SIZE_PX = 64.0f;

#define PTM(P) ((P) / PIXELS_PER_METER)
#define MTP(M) ((M) * PIXELS_PER_METER)

// -------- EDITOR --------
#define EDITOR_COLUMNS 3
const float PALETTE_WIDTH_PX = 192.0f;

// -------- TILE TYPES --------
enum class TileType {
    NONE = 0,

    // ---- BASE LAYER ----
    FLOOR,
    Outerwall_MidTop,
    Outerwall_MidBottom,
    Classwall_MidTop,
    Classwall_MidBottom,
    Wall_Separator_Left,
    Wall_Separator_Right,
    wall_Separator_Bottom,

    Wall_Corner_BottomRight1,
    Wall_Corner_BottomRight2,
    Wall_Corner_BottomLeft1,
    Wall_Corner_BottomLeft2,
    Wall_Corner_TopLeft,
    Wall_Corner_TopRight,


    // ---- OVERLAY LAYER ----
    Entrance,

    doorTextureTop,
    doorTextureBottom,
    doorOpenTextureTop,
    doorOpenTextureBottom,

    CLOCK,
    CHAIR,

    Desk_TL,
    Desk_TR,
    Desk_BL,
    Desk_BR,

    Plant_Top,
    Plant_Bottom,

    LOCKER_Top,
    LOCKER_Bottom,

    Bookshelf_TL,
    Bookshelf_TR,
    Bookshelf_BL,
    Bookshelf_BR,

    Box,
    Vent,

    Water_Top,
    Water_Bottom,

    Board_TL,
    Board_TR,
    Board_BL,
    Board_BR
};
