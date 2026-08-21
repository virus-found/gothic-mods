#!/bin/bash
set -euo pipefail

# -------------------------------------------------------------
# Configuration (Overridden by environment or update_all_mods.sh)
# -------------------------------------------------------------
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WINE_REPO_ROOT="Z:${SCRIPT_DIR//\//\\}"

UPLOADER="${UPLOADER:-/path/to/steamapps/common/TheChroniclesOfMyrtana/launcher/GothicSteamWorkshopUploader_real.exe}"
PROTON="${PROTON:-/path/to/steamapps/common/Proton - Experimental/proton}"

export SteamAppId="${SteamAppId:-39510}"
export SteamGameId="${SteamGameId:-39510}"
export STEAM_COMPAT_APP_ID="${STEAM_COMPAT_APP_ID:-39510}"
export STEAM_COMPAT_CLIENT_INSTALL_PATH="${STEAM_COMPAT_CLIENT_INSTALL_PATH:-/path/to/steam}"
export STEAM_COMPAT_DATA_PATH="${STEAM_COMPAT_DATA_PATH:-/path/to/steamapps/compatdata/1467450}"

run_uploader() {
    "$PROTON" run "$UPLOADER" "$@"
}

ask_and_run() {
    local title="$1"
    shift
    local answer=""

    echo ""
    echo "========================================================"
    echo "Target: ${title}"
    echo "========================================================"
    read -r -p "Execute for '${title}'? [y/N/q]: " answer

    case "${answer}" in
        [yY]|[yY][eE][sS])
            run_uploader "$@"
            ;;
        [qQ]|[qQ][uU][iI][tT])
            echo "Aborted by user."
            exit 0
            ;;
        *)
            echo "Skipping ${title}."
            ;;
    esac
}

echo "=== Gothic Steam Workshop Uploader ==="
echo "Confirm (y), skip (N/Enter), or quit (q) for each mod."

# -------------------------------------------------------------
# 1. Update Existing Published Mods
# -------------------------------------------------------------
ask_and_run "Update: zSprint (3770177726)" \
    update --game=g2 --mod=3770177726 \
    --content="${WINE_REPO_ROOT}\zSprint\Content" \
    --description="$(cat "${SCRIPT_DIR}/zSprint/description.bbcode")" \
    --tags="Addon,Union,Patch,Gameplay,Archolos" \
    --changenote="Change mod name due to copyright"

ask_and_run "Update: zDropEquipmentOnDeath (3770297210)" \
    update --game=g2 --mod=3770297210 \
    --content="${WINE_REPO_ROOT}\zDropEquipmentOnDeath\Content" \
    --description="$(cat "${SCRIPT_DIR}/zDropEquipmentOnDeath/description.bbcode")" \
    --changenote="Fix error Undefined function : MEM_GOTHOPTEXISTS"

ask_and_run "Update: zHeroDontDie (3770285059)" \
    update --game=g2 --mod=3770285059 \
    --content="${WINE_REPO_ROOT}\zHeroDontDie\Content" \
    --description="$(cat "${SCRIPT_DIR}/zHeroDontDie/description.bbcode")" \
    --changenote="Fixed ovens, chests, and interactive objects becoming non-interactable after main hero knock out"

ask_and_run "Update: zTrampleRats (3773551430)" \
    update --game=g2 --mod=3773551430 \
    --content="${WINE_REPO_ROOT}\zTrampleRats\Content" \
    --preview="${WINE_REPO_ROOT}\zTrampleRats\preview.png" \
    --description="$(cat "${SCRIPT_DIR}/zTrampleRats/description.bbcode")" \
    --changenote="Maintenance update"

ask_and_run "Update: zMinDamage (3786940595)" \
    update --game=g2 --mod=3786940595 \
    --title="zMinDamage" \
    --description="$(cat "${SCRIPT_DIR}/zMinDamage/description.bbcode")" \
    --tags="Addon,Union,Patch,Gameplay,Archolos" \
    --content="${WINE_REPO_ROOT}\zMinDamage\Content" \
    --preview="${WINE_REPO_ROOT}\zMinDamage\preview.png" \
    --changenote="Initial release"

ask_and_run "Update: zNoBubbles (3786941527)" \
    update --game=g2 --mod=3786941527 \
    --title="zNoBubbles" \
    --description="$(cat "${SCRIPT_DIR}/zNoBubbles/description.bbcode")" \
    --tags="Addon,Union,Plugin,Fix,Gameplay,Archolos" \
    --content="${WINE_REPO_ROOT}\zNoBubbles\Content" \
    --preview="${WINE_REPO_ROOT}\zNoBubbles\preview.png" \
    --changenote="Initial release"

ask_and_run "Update: zPlunderInState (3786949082)" \
    update --game=g2 --mod=3786949082 \
    --content="${WINE_REPO_ROOT}\zPlunderInState\Content" \
    --preview="${WINE_REPO_ROOT}\zPlunderInState\preview.png" \
    --description="$(cat "${SCRIPT_DIR}/zPlunderInState/description.bbcode")" \
    --tags="Addon,Union,Patch,Fix,Gameplay,Archolos" \
    --changenote="Maintenance update"

ask_and_run "Update: zNoUnderwaterEffects (3786949195)" \
    update --game=g2 --mod=3786949195 \
    --content="${WINE_REPO_ROOT}\zNoUnderwaterEffects\Content" \
    --preview="${WINE_REPO_ROOT}\zNoUnderwaterEffects\preview.png" \
    --description="$(cat "${SCRIPT_DIR}/zNoUnderwaterEffects/description.bbcode")" \
    --tags="Addon,Union,Patch,Gameplay,Archolos" \
    --changenote="Maintenance update"

ask_and_run "Update: zPrintScreenDuration (3787732835)" \
    update --game=g2 --mod=3787732835 \
    --title="zPrintScreenDuration" \
    --description="$(cat "${SCRIPT_DIR}/zPrintScreenDuration/description.bbcode")" \
    --tags="Addon,Union,Patch,Gameplay,Archolos" \
    --content="${WINE_REPO_ROOT}\zPrintScreenDuration\Content" \
    --preview="${WINE_REPO_ROOT}\zPrintScreenDuration\preview.png" \
    --changenote="Initial release"

#ask_and_run "Publish: MyNewMod" \
#    publish --game=g2 --title="MyNewMod" \
#    --description="$(cat "${SCRIPT_DIR}/MyNewMod/description.bbcode")" \
#    --tags="Addon,Union,Plugin,Gameplay,Archolos" \
#    --content="${WINE_REPO_ROOT}\MyNewMod\Content" \
#    --preview="${WINE_REPO_ROOT}\MyNewMod\preview.png"

echo ""
echo "Finished processing all items!"
