// File: src/TileType.hpp (UPDATED - Store editor tile IDs)
#pragma once
#include <cstdint>

enum class TileState : uint8_t
{
    Empty = 0,
    Wall = 1,
    Floor = 2,
    Flammable = 3,
    Burning = 4,
    Burnt = 5,
    Prop = 6,
    Door = 7
};

// Auto-tiling system for walls
enum class WallType : uint8_t
{
    None = 0,
    
    // Basic walls
    Top = 1,
    Bottom = 2,
    Left = 3,
    Right = 4,
    
    // Corners
    TopLeft = 5,
    TopRight = 6,
    BottomLeft = 7,
    BottomRight = 8,
    
    // T-junctions
    TJunctionTop = 9,     // ┴
    TJunctionBottom = 10, // ┬
    TJunctionLeft = 11,   // ┤
    TJunctionRight = 12,  // ├
    
    // Cross
    Cross = 13,           // ┼
    
    // Single walls
    Horizontal = 14,      // ─
    Vertical = 15,        // │
    
    // End caps
    EndTop = 16,
    EndBottom = 17,
    EndLeft = 18,
    EndRight = 19
};

struct Tile
{
    TileState state = TileState::Empty;
    WallType wallType = WallType::None;
    float burnTime = 0.0f;
    float ignitionThreshold = 1.0f;
    float heatAccumulated = 0.0f;
    
    // Prop metadata
    int propVariant = 0; // Which prop type (0=table, 1=chair, etc.)
    
    // Special tile flags
    bool hasClock = false;
    bool isDoor = false;
    
    // NEW: Store original editor tile IDs for correct rendering
    int editorTileId = 0;     // Base layer tile ID from CSV
    int overlayTileId = 0;    // Overlay layer tile ID from CSV
};