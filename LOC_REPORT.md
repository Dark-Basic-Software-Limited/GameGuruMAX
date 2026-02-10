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
