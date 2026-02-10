# Code Statistics

This report provides a breakdown of the lines of code (LOC) in the project, including the core engine components while excluding certain third-party SDKs and large asset data files.

## Summary

**Total Source Lines of Code:** **~806,870**

## Breakdown by Directory

| Directory | Files | Lines of Code | Non-Empty Lines |
| :--- | :--- | :--- | :--- |
| **GameGuru Core** | 1,146 | 734,431 | 644,079 |
| **Scripts** | 387 | 71,629 | 66,730 |
| **Simple Sound Recorder** | 7 | 515 | 434 |
| **GameGuru Launcher MAX** | 6 | 295 | 252 |
| **TOTAL** | **1,546** | **806,870** | **711,495** |

## Top 10 Largest Files (C/C++ Headers & Source)

The following files contain the most lines of code, excluding generated asset headers.

1.  **GameGuru Core/GameGuru/Source/M-GridEditB.cpp** (52,425 lines)
2.  **GameGuru Core/GameGuru/Source/M-GridEdit.cpp** (26,523 lines)
3.  **GameGuru Core/Dark Basic Public Shared/Dark Basic Pro SDK/DarkSDKMore/DarkLUA/DarkLUA.cpp** (16,673 lines)
4.  **GameGuru Core/GameGuru/Source/M-Importer.cpp** (12,988 lines)
5.  **GameGuru Core/GameGuru/Source/M-TerrainNew.cpp** (12,239 lines)
6.  **GameGuru Core/Guru-WickedMAX/GGTerrain/GGTerrain.cpp** (11,414 lines)
7.  **GameGuru Core/Dark Basic Public Shared/Dark Basic Pro SDK/Shared/Objects/CObjectsC.cpp** (11,241 lines)
8.  **GameGuru Core/GameGuru/Include/Types.h** (10,824 lines)
9.  **GameGuru Core/GameGuru/Source/M-Entity.cpp** (10,036 lines)
10. **GameGuru Core/GameGuru/Include/gameguru.h** (8,787 lines)

## Notes

- **Source Code Included:** The counts include `.cpp`, `.c`, `.cc`, `.h`, `.hpp`, and `.lua` files.
- **Included Directories:**
    - `GameGuru Core/Dark Basic Public Shared` (Included as part of the game engine, adding ~352k lines).
- **Excluded Directories:**
    - `GameGuru Core/SDK` (Third-party SDKs)
    - `GameGuru Core/GameGuru/Imgui` (Imgui library)
    - `GameGuru Core/Guru-WickedMAX/GGTerrain/TreeMeshes` (Contains large generated asset headers)
    - `GameGuru Core/Guru-WickedMAX/Nlohmann JSON` (Third-party JSON library)
- **Impact of Exclusions:** If we were to include `TreeMeshes` and `Nlohmann JSON`, the total line count would be approximately **2 million lines**, primarily due to the extensive vertex data in the tree mesh headers.
