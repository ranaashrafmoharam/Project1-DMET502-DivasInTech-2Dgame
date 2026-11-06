# Project 1 Skeleton: 2D Tower Defense (DMET 502)

This project contains the team game implementation and a standalone Member 3
visual preview. It uses GLFW and GLM from the provided OpenGL Template for VS
Code (Other), plus the vendored GLAD loader.

## Theme and style

The game is set at a mountain monastery where the ninja defend the Golden Weapons from the Serpentine.
The dusk sky, stone lanes, pagoda, torii gate, clan banners, lanterns and sakura tree establish that setting.
The palette pairs charcoal stone and indigo dusk with vermilion, gold, ice blue and venom green accents.
The art uses layered geometric shapes and crisp pixel-art sprites, with gameplay lanes kept readable.

## Build and run (Windows / PowerShell)

Install CMake and a C++17 MinGW toolchain, make sure `gcc`, `g++`, and
`mingw32-make` are on `PATH`, then configure with the path to the provided
template folder:

```powershell
cmake -S . -B build-mingw -G "MinGW Makefiles" -DOPENGL_TEMPLATE_ROOT="C:\path\to\OpenGL Template for VS Code (Other)"
cmake --build build-mingw --config Release
.\build-mingw\Project1Dmet502.exe
```

Run the executable from the project root so it can find `assets/shaders`.
The game opens at the main menu; click **START GAME** to play the introduction
cutscene, or press Space/Esc to skip it. The standalone Member 3 preview can be
started with `.\build-mingw\Member3Preview.exe`.

## How the pieces fit together (one frame)
1. game_main.cpp creates Game and calls its run loop; main.cpp is the Member 3 preview.
2. Input turns GLFW events into mouse/key queries.
3. Game looks at the current state (GameState.h):
   - MAIN_MENU / PAUSED / WON / LOST -> ui/Menus
   - CUTSCENE -> cutscene/Cutscene (uses Scene, CharacterModels, Animation, TextRenderer)
   - PLAYING:
       LaneGrid (hover/select lanes) -> PlayerPlacement (place bought players)
       -> WaveManager (spawn enemies) -> Player/Enemy/Projectile update
       -> CollisionSystem (movement, fights, shielding, pickups, base reached)
       -> Game checks win/lose
4. Rendering: Scene (background, environment, home base) -> LaneHighlight
   -> units (via HierarchicalModel + CharacterModels + Animation) -> projectiles
   and collectibles -> HealthBar -> HUD and PlayerCards.
5. Everything uses Constants.h, Renderer and Shader for shared values and drawing.

## Ownership

### Member 1
- src/game_main.cpp
- src/core/Constants.h
- src/core/GameState.h
- src/core/Game
- src/core/Input
- src/core/Shader
- src/core/Renderer
- src/entities/Entity.h
- src/world/LaneGrid
- src/world/PlayerPlacement
- src/world/WaveManager
- src/world/CollisionSystem
- assets/shaders/basic.vert
- assets/shaders/basic.frag

### Member 2
- src/entities/Player
- src/entities/Enemy
- src/entities/Projectile
- src/entities/HierarchicalModel
- src/entities/CharacterModels
- src/entities/Animation

### Member 3
- src/core/Texture
- src/scene/Scene
- src/scene/EnvironmentElements
- src/scene/HomeBase
- src/scene/PropModels
- src/scene/LaneHighlight
- src/models/Model.h
- src/models/Mesh.h

### Member 4
- src/entities/Collectible
- src/ui/TextRenderer
- src/ui/Button
- src/ui/Menus
- src/ui/HUD
- src/ui/PlayerCards
- src/ui/HealthBar
- src/cutscene/Cutscene
- src/audio/AudioManager
- assets/shaders/text.vert
- assets/shaders/text.frag

## Folder map
- src/core       engine basics: loop, state, input, rendering helpers (Member 1; Texture by Member 3)
- src/entities   players, enemies, projectiles, collectibles, models and animation (Member 2; Collectible by Member 4)
- src/world      lanes, placement, waves, collision and combat rules (Member 1)
- src/scene      background, environment, home base, props, lane highlight (Member 3)
- src/models     optional Assimp Model/Mesh loader from Lab 4 (Member 3)
- src/ui         text, buttons, menus, HUD, cards, health bars (Member 4)
- src/cutscene   intro cutscene (Member 4)
- src/audio      optional bonus audio (Member 4)
- assets         shaders, textures, models, fonts, audio
- External       third-party libraries (course template pattern)
