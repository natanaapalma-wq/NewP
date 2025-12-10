#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "build_interaction_interface.h"
#include "wall_enums.h"
#include "Wall.generated.h"

// ============================================================================
//                            WALL ACTOR
// 
// Represents a procedural wall segment in the build mode system.
// This actor combines:
// 1. Procedural mesh generation (via ProceduralWallGenerator)
// 2. Interactive object placement (doors, windows)
// 3. Grid-based coordinate system for precise positioning
// 
// NOTE: This class is under active development and will be refactored
// into smaller, more focused components (e.g., WallGridCalculator,
// WallDecorator, WallMeshManager) as the system evolves.
// ============================================================================

namespace wall_data {
  struct WallCut;
  struct PlacementResult;
  struct DecorationGrid;
  struct WallSegmentData;
  struct ProcessedWallSegment;
}

class ProceduralWallGenerator;
class WallSlicer;
class AFloorGrid;
class BuildModeManager;
class LogCommands;
class UProceduralMeshComponent;

UCLASS()
class GAMEBUILDMODE_API AWall : public AActor, public IBuildInteraction {
  GENERATED_BODY()

public:

  FIntPoint wall_id;

  bool debug = true;

  AWall();

  /**
   * Primary initialization method.
   * @param wall_data - Structural definition of the wall segment
   * @param relative_axis - Fixed Axis, no matter wall size, it's always the same (Doesn't work for diagonal walls)
   * @param wall_corners - World-space corners defining wall geometry
   * @param generator - Procedural mesh generator for this wall
   */
  void Initialize(
    wall_data::WallSegmentData* wall_data,
    float relative_axis,
    const TArray<FVector>& wall_corners,
    ProceduralWallGenerator* generator
  );

  /**
   * Initializes the internal slicing grid for window/door placement.
   * @param index - Edge index in the wall segment
   */
  void InitializeSlicerGrid(FIntPoint index);

  // ====================
  //   WALL OPERATIONS
  // ====================

  /**
   * Generates wall segments with cuts for windows/doors.
   * @param wall_cuts - Map of window/door cuts per grid edge
   * @return Array of processed wall segments for mesh generation
   */
  TArray<wall_data::ProcessedWallSegment> GenerateWallSegments(
    const TMap<FIntPoint, TArray<wall_data::WallCut>>& wall_cuts
  );

  /** Assigns a procedural mesh component to this wall actor */
  void SetWallMesh(UProceduralMeshComponent* mesh);

  void DestroyWall();

  // ====================
  //   BUILD INTERFACE
  // ====================

  virtual void PlaceObject(const FVector& world_point, EEditTool current_tool) override;
  virtual void RemoveObject(const FVector& world_point) override;

  // ====================
  // COORDINATE CONVERSION
  // ====================

  /**
   * Converts local grid coordinates to world space.
   * TODO: Move to dedicated WallGridCalculator class for reusability.
   */
  FVector GridToWorld(const FIntPoint& local_coord);

  FVector GridToWorld(const FIntPoint& local_coord, wall_enums::WallSide side);

  FVector EdgeGridToWorld(const FIntPoint& local_coord, int32 edge);

  // ====================
  //    WALL UPDATES
  // ====================

  /**
   * Updates wall geometry with new corner positions.
   * Used when walls are extended or modified after placement.
   */
  void UpdateWall(wall_data::WallSegmentData* wall_data, const TArray<FVector>& new_corners);

  // ====================
  //   GRID MANAGEMENT
  // ====================

  /** Gets IDs of grid edges contained within this wall segment */
  TArray<FIntPoint> GetContainedIDs();

  /** Sets which grid edges are contained by this wall */
  void SetContainedIDs(TArray<FIntPoint> edges);

protected:
  virtual void BeginPlay() override {
    Super::BeginPlay();
  }

private:

  void InitializeWallGrid(const FIntPoint& edge, const FVector2D& grid_size, const int32& edge_count);

  // ====================
  // OBJECT PLACEMENT LOGIC
  // ====================
  wall_data::PlacementResult TryPlaceObject();
  wall_data::PlacementResult TryPlaceWindow();
  wall_data::PlacementResult TryPlaceDoor();

  // ====================
  // WALL CUT OPERATIONS
  // ====================
  wall_data::WallCut CalculateCut(const FIntPoint& window_start, const FIntPoint& window_end);
  void SaveCut(wall_data::WallCut cut);
  FIntPoint GetObjectHalfSize();

  // ====================
  //   SPATIAL QUERIES
  // ====================
  wall_enums::WallSide GetSide(const FVector& world_point);
  FIntPoint GetEdge(int32 coord_x);

  // ====================
  // COORDINATE CONVERSION (Internal)
  // ====================
  FIntPoint WorldToGridCoordinates(const wall_data::DecorationGrid& decoration_grid, const FVector& world_position);
  FIntPoint WorldToGrid(const FVector& world_point);
  FIntPoint WorldToEdgeGrid(const FVector& world_point);
  FIntPoint WorldToEdgeGrid(const FVector& world_point, const FIntPoint& window_offsets, bool invert = false);
  int32 WorldToEdge(const FVector& world_point);

  // Inline utility methods
  int32 FindEdge(const FIntPoint& coords) { return coords.X / 50; }
  FIntPoint GridToDecorationCoord(const FIntPoint& local_coord) { return local_coord / 10; }

  FVector GridToWorldCoordinates(const wall_data::DecorationGrid& decoration_grid, const FIntPoint& wall_grid_coords);

  // ====================
  // GRID VALIDATION
  // ====================
  bool IsAreaFree(const FIntPoint& start_wall_coords);
  void MarkAreaOccupied(
    wall_enums::WallSide side,
    const FIntPoint& start_coords,
    wall_enums::WallGridOccupation occupation_type = wall_enums::WallGridOccupation::kOccupied
  );
  bool IsValidGridCoordinate(const wall_data::DecorationGrid& decoration_grid, const FIntPoint& coords);

  // ====================
  // GEOMETRY CALCULATIONS
  // ====================
  FVector2D CalculateGridSize();
  FVector CalculateInteriorOrigin(int32 edge_num, bool wall_side);
  FRotator CalculateGridRotation();

  // ====================
  // ACTOR MANAGEMENT
  // ====================
  AActor* SpawnObject(TSubclassOf<AActor> actor_class, FVector position, FRotator rotation);
  void UpdatePosition(TWeakObjectPtr<AActor> actor, FVector position, FRotator rotation);

private: 
  // ====================
  // DEBUG & LOGGING
  // ====================
  template<typename... Types>
  void Log(const FString& category, const FString& format, Types... args);

  // ====================
  // PRIVATE CONSTANTS
  // ====================
  static const int32 kCellSize = 10;  // Grid resolution in centimeters

  // ====================
  // SYSTEM DEPENDENCIES
  // ====================
  LogCommands* log_commands_;
  BuildModeManager* build_mode_manager_;
  ProceduralWallGenerator* generator_;
  WallSlicer* slicer_;

  // ====================
  //     WALL STATE
  // ====================
  FVector2D object_size_;               // Dimensions of placed objects
  FIntPoint macro_grid_coords_;         // High-level grid coordinates
  wall_enums::WallSide current_side_;   // Currently selected wall side
  wall_data::WallSegmentData* wall_data_; // Structural definition

  // ====================
  // RENDERING COMPONENTS
  // ====================
  UProceduralMeshComponent* wall_mesh_; // Procedural mesh component

  // ====================
  // SPATIAL PROPERTIES
  // ====================
  float relative_axis_;                 // Refers to the fixed axis of the wall, 
  int32 cached_edge_;                   // Cached edge index for performance
  bool is_horizontal_;                  // Wall Orientation 
  FRotator grid_rotation_;              // Interior wall face rotation
  FRotator exterior_rotation_;          // Exterior wall face rotation
  FVector grid_origin_;                 // Local grid origin
  float wall_rotation_;                 // Wall rotation in degrees
  FVector world_offset_;                // World-space offset
  TArray<FVector> wall_corners_;        // Corner vertices in world space
};


