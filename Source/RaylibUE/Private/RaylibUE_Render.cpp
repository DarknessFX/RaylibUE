#include "RaylibUE_Render.h"
#include "RaylibUE.h"
#include "raylib.h"
#include "Math/UnrealMathUtility.h"
#include "HAL/PlatformFilemanager.h"

// Helpers to make life easier
constexpr ::Vector2 ToRayVector2(const FIntPoint& Pt) { return { (float)Pt.X, (float)Pt.Y }; }
constexpr ::Vector2 ToRayVector2(const FVector2D& Vec) { return { (float)Vec.X, (float)Vec.Y }; }
constexpr ::Vector2 ToRayVector2(const FVector& Vec) { return { (float)Vec.X, (float)Vec.Y }; }
constexpr ::Rectangle ToRayRectangle(const FIntPoint& Position, const FIntPoint& Size) { return { (float)Position.X, (float)Position.Y, (float)Size.X, (float)Size.Y }; }
constexpr ::Vector3 ToRayVector3(const FVector& Vec) { return { (float)Vec.X, (float)Vec.Y, (float)Vec.Z }; }
constexpr ::Color ToRayColor(const FLinearColor& C) { return { (uint8)(C.R * 255.f), (uint8)(C.G * 255.f), (uint8)(C.B * 255.f), (uint8)(C.A * 255.f) }; }
static Camera3D DefaultCamera = { {0,0,0}, {0,10,0}, {0,0,1}, 45.f };
struct Scoped3DMode { Scoped3DMode() { BeginMode3D(DefaultCamera); } ~Scoped3DMode() { EndMode3D(); } };

void FRaylibUEModule::RenderRaylibOverlay() {
  float rlWinSizeX = (float)rlWindowSize.X;
  float rlWinSizeY = (float)rlWindowSize.Y;

  BeginTextureMode(rlRenderTarget);
    ClearBackground(BLANK);
    for (const FRlDrawCommand& Cmd : rlDrawCommandsBuffer.rlDrawCommands) {
      const ::Color rlColor = ToRayColor(Cmd.Color);
      const ::Color rlColor1 = ToRayColor(Cmd.Color1);
      const ::Color rlColor2 = ToRayColor(Cmd.Color2);
      const ::Color rlColor3 = ToRayColor(Cmd.Color3);
      const ::Color rlColor4 = ToRayColor(Cmd.Color4);
      switch (Cmd.Type) {
        case ERlDrawType::DrawPixel:
          DrawPixel(Cmd.Position.X, Cmd.Position.Y, rlColor);
          break;
        case ERlDrawType::DrawPixelV:
          DrawPixelV(ToRayVector2(Cmd.Position), rlColor);
          break;
        case ERlDrawType::DrawLine:
          DrawLine(Cmd.StartPos.X, Cmd.StartPos.Y, Cmd.EndPos.X, Cmd.EndPos.Y, rlColor);
          break;
        case ERlDrawType::DrawLineV:
          DrawLineV(ToRayVector2(Cmd.StartPos), ToRayVector2(Cmd.EndPos), rlColor);
          break;
        case ERlDrawType::DrawLineEx:
          DrawLineEx(ToRayVector2(Cmd.StartPos), ToRayVector2(Cmd.EndPos), Cmd.Thick, rlColor);
          break;
        case ERlDrawType::DrawLineBezier:
          DrawLineBezier(ToRayVector2(Cmd.StartPos), ToRayVector2(Cmd.EndPos), Cmd.Thick, rlColor);
          break;
        case ERlDrawType::DrawLineBezierQuad:
          //DrawLineBezierQuad(Vector2{ (float)Cmd.StartPos.X, (float)Cmd.StartPos.Y }, Vector2{ (float)Cmd.EndPos.X, (float)Cmd.EndPos.Y }, Vector2{ (float)Cmd.Position.X, (float)Cmd.Position.Y }, Cmd.Thick, rlColor);
          break;
        case ERlDrawType::DrawLineBezierCubic:
          //DrawLineBezierCubic(Vector2{ (float)Cmd.StartPos.X, (float)Cmd.StartPos.Y }, Vector2{ (float)Cmd.EndPos.X, (float)Cmd.EndPos.Y }, Vector2{ (float)Cmd.Position.X, (float)Cmd.Position.Y }, Vector2{ (float)Cmd.Size.X, (float)Cmd.Size.Y }, Cmd.Thick, rlColor);
          break;
        case ERlDrawType::DrawLineStrip: {
          Vector2* points = new Vector2[Cmd.Points.Num()];
          for (int32 i = 0; i < Cmd.Points.Num(); ++i) {
            points[i] = ToRayVector2(Cmd.Points[i]);
          }
          DrawLineStrip(points, Cmd.Points.Num(), rlColor);
          delete[] points;
          break;
        }
        case ERlDrawType::DrawCircle:
          DrawCircle(Cmd.Position.X, Cmd.Position.Y, Cmd.Radius, rlColor);
          break;
        case ERlDrawType::DrawCircleSector:
          DrawCircleSector(ToRayVector2(Cmd.Position), Cmd.Radius, Cmd.StartAngle, Cmd.EndAngle, Cmd.Segments, rlColor);
          break;
        case ERlDrawType::DrawCircleSectorLines:
          DrawCircleSectorLines(ToRayVector2(Cmd.Position), Cmd.Radius, Cmd.StartAngle, Cmd.EndAngle, Cmd.Segments, rlColor);
          break;
        case ERlDrawType::DrawCircleGradient:
          DrawCircleGradient(ToRayVector2(Cmd.Position), Cmd.Radius, rlColor1, rlColor2);
          break;
        case ERlDrawType::DrawCircleV:
          DrawCircleV(ToRayVector2(Cmd.Position), Cmd.Radius, rlColor);
          break;
        case ERlDrawType::DrawCircleLines:
          DrawCircleLines(Cmd.Position.X, Cmd.Position.Y, Cmd.Radius, rlColor);
          break;
        case ERlDrawType::DrawEllipse:
          DrawEllipse(Cmd.Position.X, Cmd.Position.Y, Cmd.Radius, Cmd.Size.X, rlColor); // Size.X as RadiusV
          break;
        case ERlDrawType::DrawEllipseLines:
          DrawEllipseLines(Cmd.Position.X, Cmd.Position.Y, Cmd.Radius, Cmd.Size.X, rlColor);
          break;
        case ERlDrawType::DrawRing:
          DrawRing(ToRayVector2(Cmd.Position), (float)Cmd.Size.X, Cmd.Radius, Cmd.StartAngle, Cmd.EndAngle, Cmd.Segments, rlColor); // Size.X as InnerRadius
          break;
        case ERlDrawType::DrawRingLines:
          DrawRingLines(ToRayVector2(Cmd.Position), (float)Cmd.Size.X, Cmd.Radius, Cmd.StartAngle, Cmd.EndAngle, Cmd.Segments, rlColor);
          break;
        case ERlDrawType::DrawRectangle:
          DrawRectangle(Cmd.Position.X, Cmd.Position.Y, Cmd.Size.X, Cmd.Size.Y, rlColor);
          break;
        case ERlDrawType::DrawRectangleV:
          DrawRectangleV(ToRayVector2(Cmd.Position), ToRayVector2(Cmd.Size), rlColor);
          break;
        case ERlDrawType::DrawRectangleRec:
          DrawRectangleRec(ToRayRectangle(Cmd.Position, Cmd.Size), rlColor);
          break;
        case ERlDrawType::DrawRectanglePro:
          DrawRectanglePro(ToRayRectangle(Cmd.Position, Cmd.Size), ToRayVector2(Cmd.StartPos), Cmd.Rotation, rlColor);
          break;
        case ERlDrawType::DrawRectangleGradientV:
          DrawRectangleGradientV(Cmd.Position.X, Cmd.Position.Y, Cmd.Size.X, Cmd.Size.Y, rlColor1, rlColor2);
          break;
        case ERlDrawType::DrawRectangleGradientH:
          DrawRectangleGradientH(Cmd.Position.X, Cmd.Position.Y, Cmd.Size.X, Cmd.Size.Y, rlColor1, rlColor2);
          break;
        case ERlDrawType::DrawRectangleGradientEx: {
          Vector2 topLeft = ToRayVector2(Cmd.Position);
          Vector2 topRight = ToRayVector2(Cmd.StartPos);
          Vector2 bottomRight = ToRayVector2(Cmd.EndPos);
          Vector2 bottomLeft = ToRayVector2(Cmd.Size);
          float minX = FMath::Min(FMath::Min(topLeft.x, topRight.x), FMath::Min(bottomRight.x, bottomLeft.x));
          float minY = FMath::Min(FMath::Min(topLeft.y, topRight.y), FMath::Min(bottomRight.y, bottomLeft.y));
          float maxX = FMath::Max(FMath::Max(topLeft.x, topRight.x), FMath::Max(bottomRight.x, bottomLeft.x));
          float maxY = FMath::Max(FMath::Max(topLeft.y, topRight.y), FMath::Max(bottomRight.y, bottomLeft.y));
          Rectangle rec = { minX, minY, maxX - minX, maxY - minY };
          DrawRectangleGradientEx(rec, rlColor1, rlColor2, rlColor3, rlColor4);
          break; }
        case ERlDrawType::DrawRectangleLines:
          DrawRectangleLines(Cmd.Position.X, Cmd.Position.Y, Cmd.Size.X, Cmd.Size.Y, rlColor);
          break;
        case ERlDrawType::DrawRectangleLinesEx:
          DrawRectangleLinesEx(ToRayRectangle(Cmd.Position, Cmd.Size), Cmd.Thick, rlColor);
          break;
        case ERlDrawType::DrawRectangleRounded:
          DrawRectangleRounded(ToRayRectangle(Cmd.Position, Cmd.Size), Cmd.Roundness, Cmd.Segments, rlColor);
          break;
        case ERlDrawType::DrawRectangleRoundedLines:
          DrawRectangleRoundedLines(ToRayRectangle(Cmd.Position, Cmd.Size), Cmd.Roundness, Cmd.Segments, rlColor);
          break;
        case ERlDrawType::DrawTriangle:
          DrawTriangle(ToRayVector2(Cmd.Points[0]), ToRayVector2(Cmd.Points[1]), ToRayVector2(Cmd.Points[2]), rlColor);
          break;
        case ERlDrawType::DrawTriangleLines:
          DrawTriangleLines(ToRayVector2(Cmd.Points[0]), ToRayVector2(Cmd.Points[1]), ToRayVector2(Cmd.Points[2]), rlColor);
          break;
        case ERlDrawType::DrawPoly:
          DrawPoly(ToRayVector2(Cmd.Position), Cmd.Sides, Cmd.Radius, Cmd.Rotation, rlColor);
          break;
        case ERlDrawType::DrawPolyLines:
          DrawPolyLines(ToRayVector2(Cmd.Position), Cmd.Sides, Cmd.Radius, Cmd.Rotation, rlColor);
          break;
        case ERlDrawType::DrawPolyLinesEx:
          DrawPolyLinesEx(ToRayVector2(Cmd.Position), Cmd.Sides, Cmd.Radius, Cmd.Rotation, Cmd.Thick, rlColor);
          break;
        case ERlDrawType::DrawFPS:
          DrawFPS(Cmd.Position.X, Cmd.Position.Y);
          break;
        case ERlDrawType::DrawText:
          DrawText(TCHAR_TO_ANSI(*Cmd.Text), Cmd.Position.X, Cmd.Position.Y, Cmd.FontSize, rlColor);
          break;
        case ERlDrawType::DrawTextEx:
          DrawTextEx(GetFontDefault(), const_cast<char*>(TCHAR_TO_ANSI(*Cmd.Text)), ToRayVector2(Cmd.Position), Cmd.FontSize, Cmd.Spacing, rlColor);
          break;
        case ERlDrawType::DrawTextPro:
          DrawTextPro(GetFontDefault(), const_cast<char*>(TCHAR_TO_ANSI(*Cmd.Text)), ToRayVector2(Cmd.Position), ToRayVector2(Cmd.StartPos), Cmd.Rotation, Cmd.FontSize, Cmd.Spacing, rlColor);
          break;
        case ERlDrawType::DrawTextCodepoint:
          DrawTextCodepoint(GetFontDefault(), Cmd.Codepoints[0], ToRayVector2(Cmd.Position), Cmd.FontSize, rlColor);
          break;
        case ERlDrawType::DrawTextCodepoints: {
          int32* codepoints = new int32[Cmd.Codepoints.Num()];
          for (int32 i = 0; i < Cmd.Codepoints.Num(); ++i) {
            codepoints[i] = Cmd.Codepoints[i];
          }
          DrawTextCodepoints(GetFontDefault(), codepoints, Cmd.Codepoints.Num(), ToRayVector2(Cmd.Position), Cmd.FontSize, Cmd.Spacing, rlColor);
          delete[] codepoints;
          break; }
        // 3D: Wrap in BeginMode3D/EndMode3D with default camera for overlay (assume 2D ortho, but project 3D)
        case ERlDrawType::DrawLine3D: {
            Scoped3DMode _3dScope;
            DrawLine3D(ToRayVector3(Cmd.Position3D), ToRayVector3(Cmd.Origin3D), rlColor);
          }
          break;
        case ERlDrawType::DrawPoint3D:
          {
            Scoped3DMode _3dScope;
            DrawPoint3D(ToRayVector3(Cmd.Position3D), rlColor);
          }
          break;
        case ERlDrawType::DrawCircle3D:
          {
            Scoped3DMode _3dScope;
            DrawCircle3D(ToRayVector3(Cmd.Position3D), Cmd.Radius, ToRayVector3(Cmd.RotationAxis), Cmd.RotationAngle, rlColor);
          }
          break;
        case ERlDrawType::DrawTriangle3D:
          {
            Scoped3DMode _3dScope;
            DrawTriangle3D(ToRayVector3(Cmd.Points3D[0]), ToRayVector3(Cmd.Points3D[1]), ToRayVector3(Cmd.Points3D[2]), rlColor);
          }
          break;
        case ERlDrawType::DrawTriangleStrip3D: {
          Vector3* points3d = new Vector3[Cmd.Points3D.Num()];
          for (int32 i = 0; i < Cmd.Points3D.Num(); ++i) {
            points3d[i] = ToRayVector3(Cmd.Points3D[i]);
          }
          {
            Scoped3DMode _3dScope;
            DrawTriangleStrip3D(points3d, Cmd.Points3D.Num(), rlColor);
          }
          delete[] points3d;
          break; }
        case ERlDrawType::DrawCube:
          {
            Scoped3DMode _3dScope;
            DrawCube(ToRayVector3(Cmd.Position3D), Cmd.Scale3D.X, Cmd.Scale3D.Y, Cmd.Scale3D.Z, rlColor);
          }
          break;
        case ERlDrawType::DrawCubeV:
          {
            Scoped3DMode _3dScope;
            DrawCubeV(ToRayVector3(Cmd.Position3D), ToRayVector3(Cmd.Scale3D), rlColor);
          }
          break;
        case ERlDrawType::DrawCubeWires:
          {
            Scoped3DMode _3dScope;
            DrawCubeWires(ToRayVector3(Cmd.Position3D), Cmd.Scale3D.X, Cmd.Scale3D.Y, Cmd.Scale3D.Z, rlColor);
          }
          break;
        case ERlDrawType::DrawCubeWiresV:
          {
            Scoped3DMode _3dScope;
            DrawCubeWiresV(ToRayVector3(Cmd.Position3D), ToRayVector3(Cmd.Scale3D), rlColor);
          }
          break;
        case ERlDrawType::DrawSphere:
          {
            Scoped3DMode _3dScope;
            DrawSphere(ToRayVector3(Cmd.Position3D), Cmd.Radius, rlColor);
          }
          break;
        case ERlDrawType::DrawSphereEx:
          {
            Scoped3DMode _3dScope;
            DrawSphereEx(ToRayVector3(Cmd.Position3D), Cmd.Radius, Cmd.Segments, Cmd.Sides, rlColor);
          }
          break;
        case ERlDrawType::DrawSphereWires:
          {
            Scoped3DMode _3dScope;
            DrawSphereWires(ToRayVector3(Cmd.Position3D), Cmd.Radius, Cmd.Segments, Cmd.Sides, rlColor);
          }
          break;
        case ERlDrawType::DrawCylinder:
          {
            Scoped3DMode _3dScope;
            DrawCylinder(ToRayVector3(Cmd.Position3D), Cmd.Radius, (float)Cmd.Size.X, Cmd.Scale3D.Z, Cmd.Slices, rlColor);
          }
          break;
        case ERlDrawType::DrawCylinderEx:
          {
            Scoped3DMode _3dScope;
            DrawCylinderEx(ToRayVector3(Cmd.Position3D), ToRayVector3(Cmd.Origin3D), Cmd.Radius, (float)Cmd.Size.X, Cmd.Sides, rlColor);
          }
          break;
        case ERlDrawType::DrawCylinderWires:
          {
            Scoped3DMode _3dScope;
            DrawCylinderWires(ToRayVector3(Cmd.Position3D), Cmd.Radius, (float)Cmd.Size.X, Cmd.Scale3D.Z, Cmd.Slices, rlColor);
          }
          break;
        case ERlDrawType::DrawCylinderWiresEx:
          {
            Scoped3DMode _3dScope;
            DrawCylinderWiresEx(ToRayVector3(Cmd.Position3D), ToRayVector3(Cmd.Origin3D), Cmd.Radius, (float)Cmd.Size.X, Cmd.Sides, rlColor);
          }
          break;
        case ERlDrawType::DrawCapsule:
          {
            Scoped3DMode _3dScope;
            DrawCapsule(ToRayVector3(Cmd.Position3D), ToRayVector3(Cmd.Origin3D), Cmd.Radius, Cmd.Slices, Cmd.Segments, rlColor);
          }
          break;
        case ERlDrawType::DrawCapsuleWires:
          {
            Scoped3DMode _3dScope;
            DrawCapsuleWires(ToRayVector3(Cmd.Position3D), ToRayVector3(Cmd.Origin3D), Cmd.Radius, Cmd.Slices, Cmd.Segments, rlColor);
          }
          break;
        case ERlDrawType::DrawPlane:
          {
            Scoped3DMode _3dScope;
            DrawPlane(ToRayVector3(Cmd.Position3D), ToRayVector2(Cmd.Scale3D), rlColor);
          }
          break;
        case ERlDrawType::DrawRay:
          {
            Scoped3DMode _3dScope;
            DrawRay(Ray{ ToRayVector3(Cmd.RayOrigin), ToRayVector3(Cmd.RayDir) }, rlColor);
          }
          break;
        case ERlDrawType::DrawGrid:
          {
            Scoped3DMode _3dScope;
            DrawGrid(Cmd.Slices, Cmd.Thick);
          }
          break;
        // Model: Placeholder - load if needed (add TMap<FString, Model> LoadedModels; etc. in module)
        case ERlDrawType::DrawModel: {
          static TMap<FString, Model> LoadedModels;
          Model& model = LoadedModels.FindOrAdd(Cmd.ModelPath);
          if (model.meshCount == 0) { // Unloaded default
            model = LoadModel(TCHAR_TO_ANSI(*Cmd.ModelPath));
          }
          {
            Scoped3DMode _3dScope;
            DrawModel(model, ToRayVector3(Cmd.Position3D), (float)Cmd.Scale3D.X, rlColor);
          }
          break;
        }
        case ERlDrawType::DrawModelEx: {
          static TMap<FString, Model> LoadedModels;
          Model& model = LoadedModels.FindOrAdd(Cmd.ModelPath);
          if (model.meshCount == 0) {
            model = LoadModel(TCHAR_TO_ANSI(*Cmd.ModelPath));
          }
          {
            Scoped3DMode _3dScope;
            DrawModelEx(model, ToRayVector3(Cmd.Position3D), ToRayVector3(Cmd.RotationAxis), Cmd.RotationAngle, ToRayVector3(Cmd.Scale3D), rlColor);
          }
          break;
        }
        case ERlDrawType::DrawModelWires: {
          static TMap<FString, Model> LoadedModels;
          Model& model = LoadedModels.FindOrAdd(Cmd.ModelPath);
          if (model.meshCount == 0) {
            model = LoadModel(TCHAR_TO_ANSI(*Cmd.ModelPath));
          }
          {
            Scoped3DMode _3dScope;
            DrawModelWires(model, ToRayVector3(Cmd.Position3D), (float)Cmd.Scale3D.X, rlColor);
          }
          break;
        }
        case ERlDrawType::DrawModelWiresEx: {
          static TMap<FString, Model> LoadedModels;
          Model& model = LoadedModels.FindOrAdd(Cmd.ModelPath);
          if (model.meshCount == 0) {
            model = LoadModel(TCHAR_TO_ANSI(*Cmd.ModelPath));
          }
          {
            Scoped3DMode _3dScope;
            DrawModelWiresEx(model, ToRayVector3(Cmd.Position3D), ToRayVector3(Cmd.RotationAxis), Cmd.RotationAngle, ToRayVector3(Cmd.Scale3D), rlColor);
          }
          break;
        }
        case ERlDrawType::DrawBoundingBox:
          {
            Scoped3DMode _3dScope;
            DrawBoundingBox(BoundingBox{ ToRayVector3(Cmd.Position3D), ToRayVector3(Cmd.Position3D + Cmd.Scale3D) }, rlColor);
          }
          break;
        case ERlDrawType::DrawTexture: {
          static TMap<FString, Texture2D> LoadedTextures;
          Texture2D& tex = LoadedTextures.FindOrAdd(Cmd.TexturePath);
          if (tex.id == 0) {
            Image img = LoadImage(TCHAR_TO_ANSI(*Cmd.TexturePath));
            tex = LoadTextureFromImage(img);
            UnloadImage(img);
          }
          DrawTexture(tex, Cmd.Position.X, Cmd.Position.Y, rlColor);
          break;
        }
        case ERlDrawType::DrawTextureV: {
          static TMap<FString, Texture2D> LoadedTextures;
          Texture2D& tex = LoadedTextures.FindOrAdd(Cmd.TexturePath);
          if (tex.id == 0) {
            Image img = LoadImage(TCHAR_TO_ANSI(*Cmd.TexturePath));
            tex = LoadTextureFromImage(img);
            UnloadImage(img);
          }
          DrawTextureV(tex, ToRayVector2(Cmd.Position), rlColor);
          break;
        }
        case ERlDrawType::DrawTextureEx: {
          static TMap<FString, Texture2D> LoadedTextures;
          Texture2D& tex = LoadedTextures.FindOrAdd(Cmd.TexturePath);
          if (tex.id == 0) {
            Image img = LoadImage(TCHAR_TO_ANSI(*Cmd.TexturePath));
            tex = LoadTextureFromImage(img);
            UnloadImage(img);
          }
          DrawTextureEx(tex, ToRayVector2(Cmd.Position), Cmd.Rotation, (float)Cmd.Scale3D.X, rlColor);
          break;
        }
        case ERlDrawType::DrawTextureRec: {
          static TMap<FString, Texture2D> LoadedTextures;
          Texture2D& tex = LoadedTextures.FindOrAdd(Cmd.TexturePath);
          if (tex.id == 0) {
            Image img = LoadImage(TCHAR_TO_ANSI(*Cmd.TexturePath));
            tex = LoadTextureFromImage(img);
            UnloadImage(img);
          }
          DrawTextureRec(tex, ToRayRectangle(Cmd.SourcePos, Cmd.SourceSize), ToRayVector2(Cmd.Position), rlColor);
          break;
        }
        case ERlDrawType::DrawTexturePro: {
          static TMap<FString, Texture2D> LoadedTextures;
          Texture2D& tex = LoadedTextures.FindOrAdd(Cmd.TexturePath);
          if (tex.id == 0) {
            Image img = LoadImage(TCHAR_TO_ANSI(*Cmd.TexturePath));
            tex = LoadTextureFromImage(img);
            UnloadImage(img);
          }
          DrawTexturePro(tex, ToRayRectangle(Cmd.SourcePos, Cmd.SourceSize), ToRayRectangle(FIntPoint::ZeroValue, Cmd.Size), ToRayVector2(Cmd.StartPos), Cmd.Rotation, rlColor); // Adjust dest rec as needed
          break;
        }
        case ERlDrawType::DrawTextureNPatch: {
          static TMap<FString, Texture2D> LoadedTextures;
          Texture2D& tex = LoadedTextures.FindOrAdd(Cmd.TexturePath);
          if (tex.id == 0) {
            Image img = LoadImage(TCHAR_TO_ANSI(*Cmd.TexturePath));
            tex = LoadTextureFromImage(img);
            UnloadImage(img);
          }
          NPatchInfo nPatchInfo = { 0 }; // Fill from Cmd.Points as needed (complex; assume predefined)
          DrawTextureNPatch(tex, nPatchInfo, ToRayRectangle(Cmd.Position, Cmd.Size), {0.f, 0.f}, Cmd.Rotation, rlColor);
          break;
        }
        case ERlDrawType::DrawBillboard: {
          static TMap<FString, Texture2D> LoadedTextures;
          Texture2D& tex = LoadedTextures.FindOrAdd(Cmd.TexturePath);
          if (tex.id == 0) {
            Image img = LoadImage(TCHAR_TO_ANSI(*Cmd.TexturePath));
            tex = LoadTextureFromImage(img);
            UnloadImage(img);
          }
          {
            Scoped3DMode _3dScope;
            DrawBillboard(DefaultCamera, tex, ToRayVector3(Cmd.Position3D), (float)Cmd.Scale3D.X, rlColor);
          }
          break;
        }
        case ERlDrawType::DrawBillboardRec: {
          static TMap<FString, Texture2D> LoadedTextures;
          Texture2D& tex = LoadedTextures.FindOrAdd(Cmd.TexturePath);
          if (tex.id == 0) {
            Image img = LoadImage(TCHAR_TO_ANSI(*Cmd.TexturePath));
            tex = LoadTextureFromImage(img);
            UnloadImage(img);
          }
          {
            Scoped3DMode _3dScope;
            DrawBillboardRec(DefaultCamera, tex, ToRayRectangle(Cmd.SourcePos, Cmd.SourceSize), ToRayVector3(Cmd.Position3D), ToRayVector2(Cmd.Scale3D), rlColor);
          }
          break;
        }
        case ERlDrawType::DrawBillboardPro: {
          static TMap<FString, Texture2D> LoadedTextures;
          Texture2D& tex = LoadedTextures.FindOrAdd(Cmd.TexturePath);
          if (tex.id == 0) {
            Image img = LoadImage(TCHAR_TO_ANSI(*Cmd.TexturePath));
            tex = LoadTextureFromImage(img);
            UnloadImage(img);
          }
          {
            Scoped3DMode _3dScope;
            DrawBillboardPro(DefaultCamera, tex, ToRayRectangle(Cmd.SourcePos, Cmd.SourceSize), ToRayVector3(Cmd.Position3D), ToRayVector3(FVector::UpVector), ToRayVector2(Cmd.Scale3D), ToRayVector2(FVector2D::ZeroVector), Cmd.Rotation, rlColor);
          }
          break;
        }
        default:
          UE_LOG(LogTemp, Warning, TEXT("Unknown ERlDrawType: %d"), (uint8)Cmd.Type);
          break;
        }
      }
  EndTextureMode();

  BeginDrawing();
    ClearBackground(BLANK);
    DrawTexturePro(
      rlRenderTarget.texture, 
      { 0.0f, 0.0f, rlWinSizeX, -rlWinSizeY },
      { 0.0f, 0.0f, rlWinSizeX, rlWinSizeY },
      { 0.f, 0.f }, 0.0f, WHITE);
  EndDrawing();
}