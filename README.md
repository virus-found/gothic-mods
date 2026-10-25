# Gothic II / Archolos Union Mods

Collection of lightweight quality-of-life plugins, standalone Daedalus scripts, and engine patches for Gothic II: Night of the Raven and The Chronicles of Myrtana: Archolos.

---

## Included Mods

| Mod | Type | Description |
|---|---|---|
| **[zSprint](zSprint/)** | `.d` Script | Unconditional 24/7 sprint without stamina requirements. |
| **[zHeroDontDie](zHeroDontDie/)** | `.d` Script | Fatal damage knocks the hero unconscious instead of game over, with an invincibility grace period upon recovery. |
| **[zDropEquipmentOnDeath](zDropEquipmentOnDeath/)** | `.d` Script | Slain NPCs and enemies drop their equipped weapons and shields onto the ground. |
| **[zTrampleRats](zTrampleRats/)** | `.dll` Plugin | Trample and kill small ambient rats by running over or stepping on them. |
| **[zNoBubbles](zNoBubbles/)** | `.vdf` Plugin | Fixes the vanilla engine bug where water bubble emitter trails continuously linger attached to the hero's head after swimming/diving. |
| **[zMinDamage](zMinDamage/)** | `.d` Script | Adjusts the guaranteed minimum combat damage threshold from 5 to 1 across all entities. |
| **[zPlunderInState](zPlunderInState/)** | `.patch` Patch | Prevents looting NPCs while they are in active combat/dialogue states before properly entering death/unconsciousness (with community fix for static world corpses). |
| **[zNoUnderwaterEffects](zNoUnderwaterEffects/)** | `.patch` Patch | Removes underwater camera color distortion and audio muffling for clear underwater visibility. |
| **[zPrintScreenDuration](zPrintScreenDuration/)** | `.dll` / `.d` | Extends on-screen message notification duration to a configurable 10 seconds. |
| **[zNoMapArrow](zNoMapArrow/)** | `.patch` / `.d` | Disables player position arrow and cursor on all in-game maps for authentic navigation. |
| **[zCompass](zCompass/)** | `.dll` / `.d` | Minimalist hand-drawn HUD compass styled after the authentic map legend. |

---

## Manual Installation (Non-Steam)

All mods require **[Union](https://worldofplayers.ru/threads/40376/) 1.0m+**.

1. **Daedalus Scripts (`.d`) and Engine Patches (`.patch`)**:
   * Copy the `.d` or `.patch` file into your game's `system/Autorun/` directory (e.g. `Gothic II/system/Autorun/` or `TheChroniclesOfMyrtana/system/Autorun/`).
2. **C++ Plugins (`.dll`)**:
   * Copy the `.dll` file into `system/Autorun/` (or `system/Plugins/`).
3. **Volume Archives (`.vdf`)**:
   * Copy the `.vdf` file into `Data/Plugins/` (or `Data/`).

---

## Steam Workshop Automation

The repository includes an interactive automation script ([`update_all_mods.example.sh`](update_all_mods.example.sh)) to publish and update mods directly to Steam Workshop using the official CLI uploader (`GothicSteamWorkshopUploader.exe`).

### Linux (Proton) vs Windows Support
* **Linux (Default)**: The script is configured to run the Windows uploader tool seamlessly through **Valve Proton / Wine** (`"$PROTON" run "$UPLOADER"`), automatically translating Linux repository paths into Wine drive paths (`Z:\...`).
* **Windows**: The underlying uploader command arguments (`publish` / `update` flags, titles, descriptions, and tags) are identical. On Windows, execute `GothicSteamWorkshopUploader.exe` directly with standard Windows paths.

### Quick Start (Linux)
1. Copy the example configuration to your local untracked script:
   ```bash
   cp -pv update_all_mods.example.sh update_all_mods.sh
   ```
2. Adjust your local Proton binary and Steam compatibility paths in `update_all_mods.sh` (this file is ignored by `.gitignore`).
3. Run the script:
   ```bash
   ./update_all_mods.sh
   ```
   The script will interactively ask for confirmation (`y/N/q`) before updating or publishing each mod.
