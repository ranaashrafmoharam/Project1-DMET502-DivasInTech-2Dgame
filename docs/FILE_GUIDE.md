# File-by-file guide

Each entry: purpose, owner, tasks, what it does, who it talks to.

## src/main.cpp
**Owner:** Member 1  |  **Tasks:** M1-01

Program entry point. Creates the Game object, runs it until the window closes, then shuts everything down.

- Create the Game object
- Call Game's run function
- Return an exit code

Interacts with: core/Game

## src/core/Constants.h
**Owner:** Member 1  |  **Tasks:** M1-02

Single source of truth for every shared number so nobody hard-codes values in their own files.

- Window size and projection bounds
- Lane count (3+), lane height, lane y-positions
- Home-base x boundary, enemy spawn x, player spawn x
- Movement speeds (player, enemy, Hard enemy)
- Health, damage and cooldown values per unit type
- Card costs (Melee, Ranged = 2x, Defense = 3x) and starting coins
- Wave sizes (3 / 5 / 10) and spawn interval

Interacts with: Used by every file

## src/core/GameState.h
**Owner:** Member 1  |  **Tasks:** M1-03

Defines the list of game states used by the state machine.

- MAIN_MENU, CUTSCENE, PLAYING, PAUSED, WON, LOST

Interacts with: core/Game, ui/Menus, cutscene/Cutscene

## src/core/Game.h / .cpp
**Owner:** Member 1  |  **Tasks:** M1-01, M1-03, M1-05, M1-11

The heart of the program. Owns the state machine, the main loop, all entity lists and the reset logic. Every other system is called from here.

- Run the loop: input -> update -> render, once per frame
- Switch states and keep track of the current state
- Hold lists of players, enemies, projectiles and collectibles
- Call each system's update/draw depending on the state
- Detect win/lose and switch to WON / LOST
- reset(): clear everything so Restart works from any state

Planned interface: `run()`, `setState(state)`, `reset()`, `update(dt)`, `render()`

Interacts with: Input, Renderer, LaneGrid, WaveManager, CollisionSystem, Scene, HUD, Menus, Cutscene

## src/core/Input.h / .cpp
**Owner:** Member 1  |  **Tasks:** M1-01, M1-04

Wraps raw GLFW callbacks into a simple query API so game code never touches GLFW directly.

- Track mouse position in world coordinates
- Track left-click events (pressed this frame)
- Track W / Up, S / Down and Esc presses

Planned interface: `getMouseWorldPos()`, `wasLeftClicked()`, `jumpPressed()`, `duckPressed()`, `escPressed()`

Interacts with: LaneGrid, PlayerCards, Menus, Game

## src/core/Shader.h / .cpp
**Owner:** Member 1  |  **Tasks:** M1-01

Loads, compiles and links shader programs from the assets/shaders folder and lets other code set uniforms.

- Read vertex/fragment shader files
- Report compile and link errors
- Set uniforms (matrices, colour, tint, grayscale flag)

Planned interface: `load(vertPath, fragPath)`, `use()`, `setMat4 / setVec4 / setFloat / setBool`

Interacts with: Renderer, TextRenderer

## src/core/Renderer.h / .cpp
**Owner:** Member 1  |  **Tasks:** M1-01

Common drawing helpers used by everybody: projection setup and basic draw calls with colour, tint and grayscale options.

- Set up the 2D orthographic projection
- Draw coloured/textured quads and shapes
- Draw with a red damage tint or grayscale (locked cards)
- Provide a transform stack used by hierarchical models

Planned interface: `init()`, `drawQuad(...)`, `drawShape(...)`, `pushTransform() / popTransform()`, `setTint(...) / setGrayscale(...)`

Interacts with: Entities, Scene, UI, Cutscene

## src/core/Texture.h / .cpp
**Owner:** Member 3  |  **Tasks:** M3-14

Loads images into OpenGL textures and caches them so the same image is never loaded twice.

- Load an image file into a texture
- Cache textures by path
- Bind a texture for drawing

Planned interface: `load(path)`, `bind(slot)`

Interacts with: Scene, Mesh, HUD, PlayerCards

## src/entities/Entity.h
**Owner:** Member 1  |  **Tasks:** M1-02

Base class every unit shares. Gives Member 1's systems one common way to move, collide with and draw any unit.

- Position, lane index, health, max health, alive flag
- Unit type tag (player / enemy and which kind)
- Virtual update(dt) and draw()
- Bounding box for collision

Interacts with: Player, Enemy, Projectile, Collectible, CollisionSystem

## src/entities/Player.h / .cpp
**Owner:** Member 2  |  **Tasks:** M2-02, M2-12

The three player types (Melee, Ranged, Defense) with their stats and behaviour.

- Health: Melee 5, Ranged 5, Defense 7 hits
- Damage 1 per attack (Defense deals none)
- Advance right automatically (Defense stays still)
- Attack cooldown and attack animation trigger
- Jump and duck state (Defense ignores both)
- takeDamage(), isDead(), shielded-by-Defense flag

Planned interface: `update(dt)`, `draw()`, `takeDamage(amount)`, `jump()`, `duck()`, `isShielded()`

Interacts with: Entity, CharacterModels, Animation, CollisionSystem, HealthBar

## src/entities/Enemy.h / .cpp
**Owner:** Member 2  |  **Tasks:** M2-02

The three enemy types (Easy upper-ranged, Medium lower-ranged, Hard melee) with their stats and behaviour.

- Health: Easy 5, Medium 5, Hard 7
- Damage 1 per attack, Hard deals 2
- Advance left automatically, Hard runs faster
- Easy fires HIGH attacks, Medium fires LOW attacks, Hard attacks in melee
- takeDamage(), isDead(), drops a collectible on death

Planned interface: `update(dt)`, `draw()`, `takeDamage(amount)`, `isDead()`, `attackHeight()`

Interacts with: Entity, CharacterModels, Animation, WaveManager, CollisionSystem

## src/entities/Projectile.h / .cpp
**Owner:** Member 2  |  **Tasks:** M2-11

A shot fired by the Ranged Player or a ranged enemy.

- Spawn at the weapon, travel along the lane
- Tagged HIGH (duck) or LOW (jump)
- Animated in flight; disappears on collision
- Exposes position and hitbox for collision checks

Planned interface: `update(dt)`, `draw()`, `getBounds()`, `isHigh()`, `destroy()`

Interacts with: Entity, PropModels, CollisionSystem

## src/entities/Collectible.h / .cpp
**Owner:** Member 4  |  **Tasks:** M4-08

The coin dropped by a defeated enemy.

- Appear where the enemy died, in its lane
- Spin/bob animation
- Be collected when a player touches it
- Report its value so the coin counter updates

Planned interface: `update(dt)`, `draw()`, `getBounds()`, `collect()`

Interacts with: Entity, PropModels, HUD, PlayerCards

## src/entities/HierarchicalModel.h / .cpp
**Owner:** Member 2  |  **Tasks:** M2-01

Reusable parent-child transform system for building characters from parts (body, head, arms, legs, weapon) that can move independently.

- Hold a tree of parts with local transforms
- Draw the tree using the Renderer's transform stack
- Let animations set each part's pose
- Attach weapon models from Member 3

Planned interface: `addPart(name, parent, shape)`, `setPose(name, transform)`, `draw()`

Interacts with: CharacterModels, Animation, Renderer, PropModels

## src/entities/CharacterModels.h / .cpp
**Owner:** Member 2  |  **Tasks:** M2-03 to M2-08

Builds the six distinct character models from primitives (or loaded models). Each model must have a clearly different shape and equipment.

- Melee Player, Ranged Player, Defense Player
- Easy, Medium and Hard enemies
- Everything built from multiple parts, not a single primitive
- Fits the theme chosen by Member 3

Planned interface: `buildMeleePlayer()`, `buildRangedPlayer()`, `buildDefensePlayer()`, `buildEasyEnemy()`, `buildMediumEnemy()`, `buildHardEnemy()`

Interacts with: HierarchicalModel, Player, Enemy, Cutscene

## src/entities/Animation.h / .cpp
**Owner:** Member 2  |  **Tasks:** M2-09, M2-10, M2-12

All character animation poses driven by time.

- Walk / run / idle cycles
- Melee attack swing
- Jump arc and duck pose
- Death/removal feedback hook

Planned interface: `updateWalk(model, t)`, `updateAttack(model, t)`, `updateJump(model, t)`, `updateDuck(model, t)`

Interacts with: HierarchicalModel, Player, Enemy, Cutscene

## src/world/LaneGrid.h / .cpp
**Owner:** Member 1  |  **Tasks:** M1-04

The lane system: converts the mouse into a lane and tracks hover and selection.

- Mouse position to lane index
- Track hovered lane
- Left click toggles lane selected; multiple lanes allowed
- Provide the selected-lane list for jump/duck

Planned interface: `update(mouse, click)`, `isHovered(lane)`, `isSelected(lane)`, `getSelectedLanes()`, `clear()`

Interacts with: Input, LaneHighlight, Game, Player

## src/world/PlayerPlacement.h / .cpp
**Owner:** Member 1  |  **Tasks:** M1-05

Places a purchased player on the grid.

- Require a selected card and a selected lane
- Check and deduct coins
- Spawn the player at the leftmost x of the lane
- Lock the player to that lane

Planned interface: `tryPlace(cardType, lane)`

Interacts with: PlayerCards, LaneGrid, Player, HUD

## src/world/WaveManager.h / .cpp
**Owner:** Member 1  |  **Tasks:** M1-06, M1-07

Spawner and wave controller.

- Wave 1: 3+ Easy. Wave 2: 5+ Easy/Medium. Wave 3: 10+ mixed
- Spawn at the right edge in a random lane on a timer
- Wave starts at first enemy, ends at last spawn
- Next wave begins only after all enemies are defeated
- Report wave number and enemies remaining

Planned interface: `update(dt)`, `getCurrentWave()`, `getEnemiesRemaining()`, `allWavesFinished()`

Interacts with: Enemy, Game, HUD

## src/world/CollisionSystem.h / .cpp
**Owner:** Member 1  |  **Tasks:** M1-08, M1-09, M1-10

Movement rules and all combat interactions.

- Per-lane box collision
- Stop both units when a player meets an enemy; both keep attacking
- Players pass through each other and Defense Players
- Defense Player shielding for players behind it
- Projectile hits and collectible pickups
- Detect an enemy reaching the Home Base

Planned interface: `update(players, enemies, projectiles, collectibles)`, `hasEnemyReachedBase()`

Interacts with: Entity, Player, Enemy, Projectile, Collectible, Game

## src/scene/Scene.h / .cpp
**Owner:** Member 3  |  **Tasks:** M3-02

Draws the whole game world behind the units.

- Background and lane ground strips with visible lane separation
- Call the environment elements and the home base
- Keep the scene readable against characters

Planned interface: `init()`, `draw()`

Interacts with: EnvironmentElements, HomeBase, Game, Cutscene

## src/scene/EnvironmentElements.h / .cpp
**Owner:** Member 3  |  **Tasks:** M3-03 to M3-07

The five or more themed environment objects.

- Five distinct elements with their own detail
- Placed naturally without blocking gameplay
- Optional ambient animation

Planned interface: `drawElement1() ... drawElement5()`

Interacts with: Scene

## src/scene/HomeBase.h / .cpp
**Owner:** Member 3  |  **Tasks:** M3-08

The player's safe zone at the left end of the playfield.

- Detailed structure, clearly different from the environment
- Highly visible against the background
- Its right edge is the losing line used by Member 1

Planned interface: `draw()`, `getBounds()`

Interacts with: Scene, CollisionSystem

## src/scene/PropModels.h / .cpp
**Owner:** Member 3  |  **Tasks:** M3-09, M3-10

Weapon, projectile and collectible visuals.

- Melee and ranged weapons
- Player projectile and enemy upper/lower projectiles
- Collectible/coin model with spin animation

Planned interface: `drawMeleeWeapon()`, `drawRangedWeapon()`, `drawProjectile(type)`, `drawCollectible()`

Interacts with: CharacterModels, Projectile, Collectible

## src/scene/LaneHighlight.h / .cpp
**Owner:** Member 3  |  **Tasks:** M3-11

Draws the lane hover and selected-lane highlights.

- Highlight the full lane under the mouse
- Different locked look for selected lanes
- Stay visible against the background

Planned interface: `draw(laneGrid)`

Interacts with: LaneGrid, Game

## src/models/Model.h
**Owner:** Member 3  |  **Tasks:** M3-14

OPTIONAL. Assimp-based model loader from the Lab 4 manual. Parses a model file, walks its node tree, converts it into Mesh objects, and caches textures. Skip if every model is built from primitives.

- Load a file with Assimp
- Build Mesh objects from each Assimp mesh
- Draw the whole model

Interacts with: Mesh, Texture

## src/models/Mesh.h
**Owner:** Member 3  |  **Tasks:** M3-14

OPTIONAL. One piece of drawable geometry (vertices, indices, textures) with its own VAO/VBO/EBO; draw() binds its textures and calls glDrawElements.

- Own vertex data and textures
- Set up buffers
- Draw itself

Interacts with: Model, Texture

## src/ui/TextRenderer.h / .cpp
**Owner:** Member 4  |  **Tasks:** M4-01

Draws text on screen for the whole team.

- Load a font (FreeType or bitmap font)
- Draw a string at a position, scale and colour

Planned interface: `init(fontPath)`, `drawText(text, x, y, scale, colour)`

Interacts with: HUD, Menus, Cutscene, HealthBar

## src/ui/Button.h / .cpp
**Owner:** Member 4  |  **Tasks:** M4-02

Reusable clickable button for all menus.

- Rectangle with label
- Hover highlight and click detection

Planned interface: `update(mouse, click)`, `draw()`, `wasClicked()`

Interacts with: Menus

## src/ui/Menus.h / .cpp
**Owner:** Member 4  |  **Tasks:** M4-02, M4-03, M4-04

The three required menus.

- Main Menu: Start / Exit / Sound Settings (bonus)
- Pause Menu (Esc): Continue / Restart / Main Menu
- Win/Lose Menu: Restart / Main Menu with a clear message

Planned interface: `updateMain() / drawMain()`, `updatePause() / drawPause()`, `updateEnd(won) / drawEnd(won)`

Interacts with: Button, Game (setState, reset), TextRenderer

## src/ui/HUD.h / .cpp
**Owner:** Member 4  |  **Tasks:** M4-05

Always-on screen display during gameplay.

- Coin counter at the top of the screen
- Current wave and enemies remaining
- Tell the cards when coins change

Planned interface: `setCoins(n)`, `draw(waveInfo)`

Interacts with: TextRenderer, WaveManager, PlayerCards, Collectible

## src/ui/PlayerCards.h / .cpp
**Owner:** Member 4  |  **Tasks:** M4-06

The three player cards (Melee, Ranged, Defense).

- Locked = grayscale and no hover/click
- Unlocked = normal colours, hover and selectable
- Auto-unlock when coins reach the cost, auto re-lock when below
- Show which card is selected

Planned interface: `update(coins, mouse, click)`, `draw()`, `getSelectedCard()`

Interacts with: Input, HUD, PlayerPlacement, Renderer

## src/ui/HealthBar.h / .cpp
**Owner:** Member 4  |  **Tasks:** M4-07

Health display and damage feedback for every unit.

- Health bar above or below each player and enemy
- Red tint that grows as health drops, or scale-down per hit

Planned interface: `draw(entity)`, `getTintAmount(entity)`

Interacts with: Player, Enemy, Renderer

## src/cutscene/Cutscene.h / .cpp
**Owner:** Member 4  |  **Tasks:** M4-09

The 10-20 second animated introduction.

- Timeline that triggers each beat
- At least one player and one enemy, animated and interacting
- Short dialogue with subtitles, plus visible animation
- Skip with Esc/Space; tells Game when it finishes

Planned interface: `update(dt)`, `draw()`, `isFinished()`, `skip()`

Interacts with: Scene, CharacterModels, Animation, TextRenderer, Game

## src/audio/AudioManager.h / .cpp
**Owner:** Member 4  |  **Tasks:** M4-11

OPTIONAL BONUS. Background music, sound effects and volume settings.

- Play music and sound effects
- Volume up, down and mute (Sound Settings menu)
- Used by Members 1 and 2 to trigger effects

Planned interface: `init()`, `playMusic()`, `playSound(id)`, `setVolume(v)`, `mute()`

Interacts with: Menus, Game, Player, Enemy

## assets/shaders/basic.vert
**Owner:** Member 1

Vertex shader used for most 2D drawing: applies the model matrix and the orthographic projection.

## assets/shaders/basic.frag
**Owner:** Member 1

Fragment shader for coloured/textured shapes. Includes uniforms for the red damage tint and for the grayscale mode used by locked cards.

## assets/shaders/text.vert
**Owner:** Member 4

Vertex shader for text quads.

## assets/shaders/text.frag
**Owner:** Member 4

Fragment shader that samples the font atlas and applies text colour.
