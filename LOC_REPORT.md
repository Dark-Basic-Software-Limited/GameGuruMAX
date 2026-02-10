# Code Statistics

This report provides a breakdown of the lines of code (LOC) in the project, excluding third-party libraries and large asset data files.

## Summary

**Total Source Lines of Code:** **~455,016**

## Breakdown by Directory

| Directory | Files | Lines of Code | Non-Empty Lines |
| :--- | :--- | :--- | :--- |
| **GameGuru Core** | 346 | 382,577 | 346,078 |
| **Scripts** | 387 | 71,629 | 66,730 |
| **Simple Sound Recorder** | 7 | 515 | 434 |
| **GameGuru Launcher MAX** | 6 | 295 | 252 |
| **TOTAL** | **746** | **455,016** | **413,494** |

## Notes

- **Source Code Included:** The counts include `.cpp`, `.c`, `.cc`, `.h`, `.hpp`, and `.lua` files.
- **Excluded Directories:**
    - `GameGuru Core/SDK` (Third-party SDKs)
    - `GameGuru Core/Dark Basic Public Shared` (Likely third-party/legacy shared library)
    - `GameGuru Core/GameGuru/Imgui` (Imgui library)
    - `GameGuru Core/Guru-WickedMAX/GGTerrain/TreeMeshes` (Contains large generated asset headers)
    - `GameGuru Core/Guru-WickedMAX/Nlohmann JSON` (Third-party JSON library)
- **Impact of Exclusions:** If we were to include `TreeMeshes` and `Nlohmann JSON`, the total line count would be approximately **1.6 million lines**, primarily due to the extensive vertex data in the tree mesh headers.
