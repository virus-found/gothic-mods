#!/bin/bash
set -euo pipefail

# -------------------------------------------------------------
# Configuration (overridden by environment or update_all_mods.sh)
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

# Note: GothicSteamWorkshopUploader.exe 'update' treats --content, --preview,
# --description, --tags, and --title as mutually exclusive per invocation.
ask_and_update() {
    local mod_dir="$1"
    local mod_id="$2"
    local changenote="$3"
    local answer=""

    echo ""
    echo "========================================================"
    echo "Target: Update ${mod_dir} (${mod_id})"
    echo "========================================================"
    read -r -p "Execute update for '${mod_dir}'? [y/N/q]: " answer

    case "${answer}" in
        [yY]|[yY][eE][sS])
            run_uploader update --game=g2 --mod="${mod_id}" \
                --content="${WINE_REPO_ROOT}\\${mod_dir}\\Content" \
                --changenote="${changenote}"
            if [[ -f "${SCRIPT_DIR}/${mod_dir}/preview.png" ]] ; then
                run_uploader update --game=g2 --mod="${mod_id}" \
                    --preview="${WINE_REPO_ROOT}\\${mod_dir}\\preview.png"
            fi
            if [[ -f "${SCRIPT_DIR}/${mod_dir}/description.bbcode" ]] ; then
                run_uploader update --game=g2 --mod="${mod_id}" \
                    --description="$(cat "${SCRIPT_DIR}/${mod_dir}/description.bbcode")"
            fi
            ;;
        [qQ]|[qQ][uU][iI][tT])
            echo "Aborted by user."
            exit 0
            ;;
        *)
            echo "Skipping ${mod_dir}."
            ;;
    esac
}

echo "=== Gothic Steam Workshop Uploader ==="
echo "Confirm (y), skip (N/Enter), or quit (q) for each mod."

ask_and_update "zSprint"               "3770177726" "Maintenance update"
ask_and_update "zDropEquipmentOnDeath" "3770297210" "Fix G1 and G2 compatibility, MergeMode, and ZS_Dead chaining"
ask_and_update "zHeroDontDie"          "3770285059" "Fix compatibility with LHiver, G1, and custom overhaul mods"
ask_and_update "zTrampleRats"          "3773551430" "Exclude giant and hostile monster rats from trampling"
ask_and_update "zMinDamage"            "3786940595" "Enable MergeMode in script metadata"
ask_and_update "zNoBubbles"            "3786941527" "Maintenance update"
ask_and_update "zPlunderInState"       "3786949082" "Maintenance update"
ask_and_update "zNoUnderwaterEffects"  "3786949195" "Fix engine patch addresses for G1, G1A, and G2 Classic"
ask_and_update "zPrintScreenDuration"  "3787732835" "Fix standalone support across G1, G1A, G2, G2A, and modded installs"
ask_and_update "zNoMapArrow"           "3788405389" "Fix stack cleanup in map cursor patch and LeGo compatibility"
ask_and_update "zCompass"              "3788405613" "Fix native C++ HUD rendering across clean Union and modded installs"

echo ""
echo "Finished processing all items!"
