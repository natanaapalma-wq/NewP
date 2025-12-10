
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "grid_tool_enums.h" 
#include "grid_calculator_enum.h" 
#include "floor_grid.generated.h" 

// ================================================================
// 	               SYSTEM COORDINATOR: FloorGrid
// 	Central hub for grid-based editing operations in Build Mode.
// 	Delegates specialized tasks to focused subsystem managers.
// ================================================================

// --- SINGLETONS ---
class LogCommands;
class BuildModeManager;

// --- INTERFACE ---
class BaseGridCalculator;

// --- UNREAL COMPONENT ---
class UProceduralMeshComponent;

// --- GRID MEMBERS ---
class GridTileData;
class GridClick;
class GridWallInteractions;
class GridObjectInteractions;
class GridPathFinder;
class RoomsManager;

// UnrealEngine has it's own name conventions.
UCLASS()
class GAMEBUILDMODE_API AFloorGrid : public AActor {
	GENERATED_BODY()

public:

	int32 current_floor_index;
	AFloorGrid* floor_above;
	AFloorGrid* floor_below;

	/** Enables debug logging and visualization. */
	bool debug = false;

	// Unreal Engine classes doesn't allow to pass variable through constructor.
	AFloorGrid();

	// It's needed to be initialized manually.
	void Initialize(grid_calculator_enum::Lot lot_key, float floor_height);

	void HandleClick(EditTool tool, const FVector& world_point, bool is_pressed);

	UProceduralMeshComponent* CreateAndAttachProceduralMesh();

protected:
	virtual void BeginPlay() override {
		Super::BeginPlay();

		// This is just for test purpose, instead this will be called by the GlobalMap class.
		Initialize(grid_calculator_enum::Lot::kNeighbor1Lot1, 100.f);
	}

private:

	void InitializeComponents();

	// Core systems
	LogCommands* log_;
	BuildModeManager* build_mode_manager_;

	/** Template with optmized operations, accessed via this interface. */
	BaseGridCalculator* calculate_;

	// Member classes
	TUniquePtr<GridWallInteractions> wall_interactions_;
	TUniquePtr<GridObjectInteractions> object_interactions_;
	TUniquePtr<GridPathFinder> pathfinder_;
	TUniquePtr<RoomsManager> rooms_manager_;

	// Data
	TUniquePtr<GridClick> click_;
	TUniquePtr<GridTileData> tiles_data_; // Holds tiles, tile edges, tile corners and operations.
};

