
#include "GridSystem/floor_grid.h"
#include "ProceduralMeshComponent.h"
#include "grid_tool_enums.h"
#include "build_mode_manager.h"
#include "GridSystem/grid_calculator.h"
#include "Log/log_commands.h"
#include "GridSystem/grid_click.h"
#include "GridSystem/WallGenerator/grid_wall_interactions.h"
#include "GridSystem/grid_object_interactions.h"
#include "grid_tile_data.h"

using namespace grid_data;

AFloorGrid::AFloorGrid()
    : build_mode_manager_(nullptr)
    , log_(nullptr)
    , tiles_data_(nullptr)
    , click_(nullptr)
    , calculate_(nullptr)
    , wall_interactions_(nullptr)
    , object_interactions_(nullptr)
    , pathfinder_(nullptr)
    , room_manager_(nullptr)
    , current_floor_index(0)
    , floor_above(nullptr)
    , floor_below(nullptr)
    , debug(false) {  
      
  // First we get the singletons
  build_mode_manager_ = BuildModeManager::GetInstance();
  log_ = LogCommands::GetInstance();
}

void AFloorGrid::Initialize(grid_calculator_enum::Lot lot_key, float floor_height) {
  
  if (debug) log_->Info("Grid", "StartGrid!");

  // Here we get the specilized calculator for this grid, the calculator holds the size (example: 64x64)
  // and also contains the world offset and world rotation
  calculate_ = build_mode_manager_->LotGridCalculator(lot_key);
    
  if (calculate_ == nullptr) {
    log_->Error(
      "FloorGrid", "Failed to get grid calculator for lot key: {0}", 
      static_cast<int32>(lot_key));
    return;
  }

  tiles_data_ = TMakeUnique<GridTileData>(calculate_, floor_height);

  // - Z axis is not pre calculated by the gridCalculator, 
  // which allows to simulate various floors with a single calculator
  // - Yaw rotation is the only axis considered by the gridCalculator

  InitializeComponents();
}

void AFloorGrid::InitializeComponents() {

  click_ = TMakeUnique<GridClick>(calculate_);

  wall_interactions_ = TMakeUnique<GridWallInteractions>(
    this, click_.Get(), tiles_data_.Get()
  );

  object_interactions_ = TMakeUnique<GridObjectInteractions>(
    this, click_.Get(), tiles_data_.Get()
  );

  pathfinder_ = TMakeUnique<GridPathFinder>(
    this, click_.Get(), tiles_data_.Get()
  );

  rooms_manager_ = TMakeUnique<RoomsManager>(
    this, click_.Get(), tiles_data_.Get()
  );

}


void AFloorGrid::HandleClick(EditTool tool, const FVector& world_point, bool is_pressed) {

  switch (tool) {
  case EditTool::kPlaceWall:
    wall_interactions_->HandlePlaceWall(world_point, is_pressed);
    break;

  case EditTool::kPlaceObject:
    object_interactions_->HandlePlaceObject(world_point, is_pressed); 
    break;

  case EditTool::None:
    break;
    
  default:
    if (debug) {
      log_->Warning("GridSystem", "Unhandled tool type: %d", 
      static_cast<int32>(tool));}
    break;
    
  }
}

UProceduralMeshComponent* AFloorGrid::CreateAndAttachProceduralMesh() {

  UProceduralMeshComponent* new_mesh;
  new_mesh = NewObject<UProceduralMeshComponent>(this);
  new_mesh->RegisterComponent();
  new_mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
  return new_mesh;
}

