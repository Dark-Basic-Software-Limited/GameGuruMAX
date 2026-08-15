#include "Selecting.h"
#include "stdafx.h"
#include "gameguru.h"
#include "../Imgui/imgui.h"
#include "../Imgui/imgui_gg_dx11.h"
#include "../Imgui/imgui_internal.h"

// Partial definition
namespace SelModes // SelectionModes
{
	eModes mode = Normal;

	struct SelTypeEntry // SelectionTypeEntry
	{
		eModes modeUsed = Normal; // the mode used to scan for this type. This is used to reperform a scan. Eg: after a radius change.
		int bankindex = 0; // the bank index is used to match the entity "type". Essentially, the ".fpe" index.
		// TODO: In a future update, we can have UI properties of the variables used, like radius, distance, accuracy, etc and reapply the scans automatically using these new values, starting with these seeds.
		int seedEntityId = 0; // this entity was selected as the seed/anchor to scan for all other related matches. 
		float PY = 0.0f; // Only used by NearbyLateral mode. The PY position is used to match the nearby entities.
		int typeCount = 0; // how many times this type was added by selection

		// default constructor
		SelTypeEntry()
			: modeUsed(Normal), bankindex(0), seedEntityId(0), PY(0.0f), typeCount(0)
		{
		}
		// custom constructor
		SelTypeEntry(eModes modeUsed, int b, int seedId, float py, int c)
			: modeUsed(modeUsed), bankindex(b), seedEntityId(seedId), PY(py), typeCount(c) {
		}
	};
	std::vector<SelTypeEntry> AllowedTypes; // This stores the bank index used to match the entity type.

	int iSelRadius = 250;
	std::chrono::steady_clock::time_point lastAdjustmentMs = std::chrono::steady_clock::now();
	static constexpr float kfInvisibleObjsMax = -45000.0f; // value range could be -1000 to -48000+1.
	bool bIncludeInvisibleObjs = false; // used to include objects in editor which are hidden underground
	bool bScanningActive = false; // only used to display status message

	// static/local:
	inline void ReplaceIfCurrent(eModes& modeUsed);
	inline static bool currModeMatches(eModes m);
	inline static bool modesMatch(eModes m1, eModes m2);
	inline static bool isRubberBandDependent(eModes m);
	inline static bool isCtrlClickDependent(eModes m);
	inline static eModes IntToMode(int intMode);
	static void initiateScan(int e, SelModes::eModes modeUsed, int seedEntityId = 0, bool scanning = false);

	static struct SelModeNamesEntry
	{
		const char* longName; // plain name
		const char* shortName; // short name
		const char* buttonLabel;  // button text, includes label "##Btn_*"
		const char* purposeDesc;  // for the button tooltip. N.B. The "\n" is stripped out in some cases.
	};
	static constexpr std::array<SelModeNamesEntry, eModes::Count> ModeNames = { {
		{ R"(Normal Selection Mode)",	R"(Normal)",		"Normal##Btn_NormalMode",
			"Selects all entities, even if they are occluded\n"
			"or very far away, using rubber-band selection." },
		{ R"(Select Same Type)",					R"(Same Type)",		"Same Type##Btn_SameType",
			"Selects only entities of the same type as the selected seed\n"
			"entity (or current object, if no seed), using rubber-band selection." },
		{ R"(Select Adjacent Type)",		R"(Adjacent)",		"Adjacent##Btn_Adjacent",
			"Finds and selects adjacent entities, of the\n"
			"same type as the selected seed entity." },
		{ R"(Select Nearby Lateral)",		R"(Nearby)",		"Nearby##Btn_Nearby",
			"Finds and selects nearby lateral entities, of\n"
			"the same type as the selected seed entity." },
		{ R"(Select By Radius)",				R"(Radius)",			"Radius##Btn_Radius",
			"Finds and selects all entities within the spherical\n"
			"radius of the selected seed entity." }
	} };
	static constexpr std::array<const char*, eAddSelMethods::Count_Methods> AddSelMethods = {
		"rubber-band",
		"ctrl+click",
		"scan"
	};

	namespace SelModesWindow
	{
		// static/local:
		static bool doShowTip();
		static void displayTip();
		static void drawButtonForMode(eModes mode);
		static bool drawButtonDeactivated(const char* label, bool active);
	}

	namespace Match
	{
		static struct EntityData
		{
			bool valid = false;
			int bankindex = 0;
			float PY = 0.0f;
			int obj = 0;

			// constructor
			EntityData(bool valid, int bankindex, float PY, int obj)
				: valid(valid), bankindex(bankindex), PY(PY), obj(obj) {
			}
		};

		// static/local:
		static void addEntityType_StructEntry(
			int bankindex,
			int seedEntityId,
			float PY,
			int count,
			eModes modeUsed = eModes::CurrMode);
		static SelTypeEntry* findEntryByBankindex(eModes modeUsed, int bankindex);
		static SelTypeEntry* findEntryBySeedEntityId(eModes modeUsed, int bankindex, int entityId);
		static bool isEntityInRubberBandList(int e);
		static int countAllowedTypesForMode(eModes mode);
		static bool allEntriesMatchMode(eModes mode);
		static EntityData getEntityData(int e);
		static float getDist2Objs(int e1, int e2);
		static bool samePlane2Objs(int e1, int e2);
		static bool objectIsSameType(int e2, int bankindex_e1);
		static bool objectIsTooFar(int e1, int e2);
		static bool fullyAlign(float aMin, float aMax, float bMin, float bMax);
		static bool facesTouch(float aMin, float aMax, float bMin, float bMax);
		static bool getWorldAABB(int e, GGVECTOR3& outMin, GGVECTOR3& outMax);
		static bool adjacentAABB(int e1, int e2);
		static bool sharesSamePY(int e1, int e2);
		static void scanForAdjacentAABB_recusive(int e1, int seedEntityId);
		static void scanForNearbyPY_recusive(int e1, int seedEntityId);
		static void scanForObjectsWithinRadius(int seedEntityId);
		static void clearScanningSelections();
		static void reapplyScans();
	}

	namespace Help
	{
		enum eKeyCombo : int // Keyboard key combination
		{
			Ctrl_LMB = 0,
			Spacebar,
			Count
		};
		constexpr char winLabel[] = "Help##SelMode_HelpPopup";
		bool showWindow = false;

		// static/local:
		void centerModalWindow();
		static void drawPressLMBTip(const char* action = "");
		static void drawKeyAndTextTip(const char* press, eKeyCombo keyComboId, const char* action);
		static void ImGui_fillRowBGColor(ImGuiCol bgColor = ImGuiCol_MenuBarBg, float rounding = 0.0f);
		static void drawHeader(eModes mode);
		static void drawBullet(const char* text);
		static void checkForClose(bool* pShowWin);
		static std::string ReplaceLFWithSpaces(const char* s);
		static void drawWindow();
	}

	namespace DebugLocal
	{
		static void output_allowedTypes(const char* callerFuncName = "", const char* text = "");
		static void output_entityrubberbandlist(const char* callerFuncName = "", const char* text = "");
		static void output_addEntityTypeInfo(
			int e,
			int seedEntityId,
			bool findBySeed,
			eAddSelMethods addSelMethod,
			bool hasParentSeed,
			bool isSeed,
			eModes modeUsed);
	}
};

/// <summary>
/// <see>namespace SelTools</see><br/>
/// Entity selection tools - tab bar window<br/>
/// </summary>
/// Involving these tabs:
/// <list type="number">
///   <item>Selected Objects</item>
///   <item>Groups</item>
///   <item>Selection Modes</item>
/// </list>
namespace SelTools // SelectionTools
{
	eTabs knownActiveTab = Selected;
	eTabs nextTabToShow = None;

	/// Returns: true if the active tab is known to be the given tab.
	bool knownActiveTab_is(eTabs tab)
	{
		return knownActiveTab == tab;
	}

	/// Specify which tab is already currently active.<br/>
	/// Note: It does not set which tab to show. Use setNextTabToShow() for that.
	void setKnownActiveTab(eTabs tab)
	{
		knownActiveTab = tab;
	}

	/// Returns: true if the next tab to switch to is the given tab.
	bool nextTabToShow_is(eTabs tab)
	{
		return nextTabToShow == tab;
	}

	/// Indicate which tab to switch to, if suitable, otherwise keep it on "Selection Modes" tab to 
	/// see selection options and updates.
	void setNextTabToShow(eTabs tab)
	{
		bool keepModesTabVisible = (
			tab == eTabs::Selected // low prirority to show
			&& knownActiveTab_is(eTabs::Modes) // higher priority to show
			//&& !SelModes::mode_isNormal() // doesn't need "Selection Modes" tab to be visible
			);

		if (keepModesTabVisible)
		{
			// Keep existing eTabs::Modes ("Selection Modes") tab visible.
			// General tab priority is: Group, Modes, Selected.
		}
		else
		{
			// most switching is from "Groups" tab.
			nextTabToShow = tab; // otherwise switch tabs.
		}
	}

	/// Clear the "nextTabToShow" value
	void resetNextTabToShow()
	{
		// "None" is a special state, not a tab change.
		// It often means the current tab switch is already committed to occur, clear nextTabToShow for the next tab in future.
		nextTabToShow = eTabs::None;
	}
}

/// <summary>
/// <see>namespace SelModes</see><br/>
/// - Entity selection modes - tab window, selection options<br/>
/// </summary>
/// Involving selecting entities:
/// <list type="bullet">
///   <item>any entity -  using rubberband</item>
///   <item>by type - using rubberband</item>
///   <item>by adjacent entities - via scan</item>
///   <item>by nearby entities - via scan</item>
///   <item>by radius - via scan</item>
/// </list>
namespace SelModes // SelectionModes
{
	/// Reset data when loading a different level.
	void init()
	{
		mode = Normal;
		iSelRadius = 250;
		bIncludeInvisibleObjs = false;
		bScanningActive = false;
		clearAllowedTypesAndSelections();
	}
	/// Returns: current selection mode.
	eModes getMode()
	{
		return mode;
	}
	/// Replaces CurrMode in 'modeUsed' with currently active 'mode', otherwise it remains unchanged.
	inline void ReplaceIfCurrent(eModes& modeUsed)
	{
		if (modeUsed == eModes::CurrMode)
			modeUsed = mode;
	}
	/// Returns: true if the current selection mode is the given mode.
	inline static bool currModeMatches(eModes m)
	{
		return mode == m;
	}
	/// Returns: true if the given selection mode is the given comparison mode.
	inline static bool modesMatch(eModes m1, eModes m2)
	{
		return m1 == m2;
	}
	/// Returns: true if the current selection mode is Normal (i.e. no special selection mode is active).
	inline bool mode_isNormal() // commonly used
	{
		return mode == Normal;
	}
	/// <summary>
	/// Returns: true if the given selection mode depends on rubber-band selection (i.e. not Ctrl+Click-based selection).
	/// </summary>
	/// Rubber-band dependent selection modes include: Normal, SameType.
	inline static bool isRubberBandDependent(eModes m)
	{
		return (m == Normal || m == SameType);
	}
	/// <summary>
	/// Returns: true if the given selection mode is Ctrl+Click dependent.
	/// </summary>
	/// Ctrl+Click dependent selection modes include: AdjacentTypes, NearbyLateral, Radius.
	inline static bool isCtrlClickDependent(eModes m)
	{
		return (m == AdjacentTypes || m == NearbyLateral || m == Radius);
	}
	/// Returns: the given int selection mode as an enum eModes value.
	inline static SelModes::eModes IntToMode(int intMode)
	{
		return static_cast<SelModes::eModes>(intMode);
	}

	/// Set the current rubberband object selection mode, and optionally cycle to the next mode.
	void switchMode(eModes newMode, bool useNextMode /* = false */)
	{
		// determine next mode
		if (useNextMode) // ignores 'newMode' parameter
		{
			// cycle to the next mode
			if (mode_isNormal())
				newMode = eModes::SameType;
			else if (currModeMatches(SameType))
				newMode = eModes::AdjacentTypes;
			else if (currModeMatches(AdjacentTypes))
				newMode = eModes::NearbyLateral;
			else if (currModeMatches(NearbyLateral))
				newMode = eModes::Radius;
			else
				newMode = eModes::Normal; // cycle back to start
		}

		mode = newMode;

		// Allow selection of different types or between different modes to allow continuation of selection.
		// If allowSelUsingMultModes is false, it probably means this feature is not complete or precise yet.
		constexpr bool allowSelUsingMultModes = false; // This could be a Settings option.
		if (!allowSelUsingMultModes) // if (!prefs.allowSelUsingMultModes)
		{
			// Clear lists when changing modes, for simplest usage.
			clearAllowedTypesAndSelections();

			/* // automatically select seed entity.
			 if (currModeMatches(SameType))
			{
				auto e = t.widget.pickedEntityIndex;
				addEntityType(e, e, viaCtrlClick);
			}
			*/
		}

		SelTools::setNextTabToShow(SelTools::eTabs::Modes);
	}

	/// <summary>
	/// Purpose: Add an entry to list of allowed types and adjust 'count'.<br/>
	/// </summary>
	/// Entry may include: bankindex, seedEntityId, PY, modeUsed, plus  'count'.<br/>
	/// Matching: 'bankindex' is used for determining the '.fpe' entity 'type' of the entity 'e'.
	void addEntityType(
		int e,
		int seedEntityId /* = 0 */,
		eAddSelMethods addSelMethod /* = viaScan */,
		eModes modeUsed /* = CurrMode */)
	{
		auto entityData = Match::getEntityData(e);
		if (!entityData.valid)
			return;

		ReplaceIfCurrent(modeUsed);

		// 'seedEntityId' is unused for rubber-band dependent selection modes. Even if it is > 0, it will not be used.
		if (isRubberBandDependent(modeUsed))
			seedEntityId = 0; // seedEntityId is not required.

		bool hasParentSeed = (seedEntityId > 0);
		bool isSeed = hasParentSeed && (e == seedEntityId);

		// Mode "SameType" should only have 1 seed entity (of any value) per bankindex, so ignore similar seed entries to avoid adding duplicates.
		// Mode "Radius" can have multiple seed entities per bankindex.
		bool findBySeed = false;

		// 1: Select via CtrlClick AND mode is not rubber-band dependent
		if (addSelMethod == viaCtrlClick && isCtrlClickDependent(modeUsed))
		{
			findBySeed = true;
		}

		// 2: this entity is a seed AND SameType mode
		if (isSeed)
		{
			if (modeUsed == SameType)
				findBySeed = true;
		}

		// 3: this entity has a parent Seed && 'modeUsed' is one of the CtrlClick modes (scan-based)
		if (hasParentSeed && isCtrlClickDependent(modeUsed))
		{
			findBySeed = true;
		}

		//DebugLocal::output_addEntityTypeInfo(e, seedEntityId, findBySeed, addSelMethod, hasParentSeed, isSeed, modeUsed);

		SelTypeEntry* pEntry = (findBySeed)
			? Match::findEntryBySeedEntityId(modeUsed, entityData.bankindex, seedEntityId)
			: Match::findEntryByBankindex(modeUsed, entityData.bankindex);

		// store seedEntityId & modeUsed value in entityrubberbandlist entry
		setSelVarsInRubberBandEntry(e, seedEntityId, modeUsed);

		if (!pEntry)
		{
			Match::addEntityType_StructEntry(
				entityData.bankindex,
				seedEntityId,
				entityData.PY,
				1, // sets count to 1
				modeUsed);
		}
		else
		{
			if (addSelMethod == viaScan) // use quicker estimated count.
				pEntry->typeCount++;
			else
				Match::countRubberBandTypes(); // prevents duplicate counts
		}
	}

	/// <summary>
	/// Purpose: Add an entry to list of allowed types and adjust 'count', based on the current selection mode.<br/>
	/// This is triggered via Ctrl+Click.<br/>
	/// </summary>
	/// This is used by all modes.<br/>
	/// 'seedEntityId' is unused for rubber-band dependent selection modes. Even if it is > 0, it will not be used.
	void addEntityTypeAndScan_viaCtrlClick(int e, int seedEntityId, SelModes::eModes modeUsed)
	{
		ReplaceIfCurrent(modeUsed);

		addEntityType(e, seedEntityId, viaCtrlClick, modeUsed);
		if (isCtrlClickDependent(modeUsed))
		{
			initiateScan(e, modeUsed, seedEntityId);
		}
	}

	/// <summary>
	/// Purpose: Add an entry to list of allowed types and adjust 'count', based on the current selection mode.<br/>
	/// This is triggered via rubber-band selection.<br/>
	/// </summary>
	/// This is only used for rubber-band dependent selection modes, since the other modes are 
	/// triggered via Ctrl+Click or property control changes.<br/>
	/// 'seedEntityId' is unused for rubber-band dependent selection modes. Even if it is > 0, it will not be used.
	void addEntityType_viaRubberBand(int e, int seedEntityId, SelModes::eModes modeUsed)
	{
		ReplaceIfCurrent(modeUsed);

		if (isRubberBandDependent(modeUsed))
		{
			addEntityType(e, seedEntityId, viaRubberBand, modeUsed);
		}
	}

	/// Purpose: Initiate a scan for adjacent or nearby entities, or within radius, based on the current selection mode.
	static void initiateScan(int e, SelModes::eModes modeUsed, int seedEntityId /* = 0 */, bool scanning /* = false */)
	{
		if (scanning)
			return;

		// add individual via rubber-band selection, or initiate scan
		if (isRubberBandDependent(modeUsed))
		{
			gridedit_addEntityToRubberBandHighlights(e);
		}
		else // if (isCtrlClickDependent(modeUsed))
		{
			if (modesMatch(modeUsed, AdjacentTypes))
			{
				Match::scanForAdjacentAABB_recusive(e, e);
			}
			else if (modesMatch(modeUsed, NearbyLateral))
			{
				Match::scanForNearbyPY_recusive(e, e);
			}
			else if (modesMatch(modeUsed, Radius))
			{
				Match::scanForObjectsWithinRadius(e);
			}
		}
		Match::countRubberBandTypes();
	}

	/// Purpose: clears the list of allowed types to match against, and resets the count for each type.
	void clearAllowedTypesAndSelections()
	{
		AllowedTypes.clear();
		AllowedTypes.shrink_to_fit();

		g.entityrubberbandlist.clear();
		g.entityrubberbandlist.shrink_to_fit();
	}

	/// Purpose: sets the SelModes-related variables in a single 'sRubberBandType' item.
	void setSelVarsInRubberBandEntry(int e, int seedEntityId, eModes modeUsed)
	{
		for (auto& item : g.entityrubberbandlist)
		{
			if (item.e == e)
			{
				item.seedEntityId = seedEntityId;
				item.modeUsed = modeUsed;
				break;
			}
		}
	}

	/// Purpose: checks if the list of allowed types is empty.
	bool allowedTypesIsEmpty()
	{
		return AllowedTypes.empty();
	}

	/// <summary>
	/// Purpose: checks if bankindex of entity is in the list of the allowed "types" to match.<br/>
	/// </summary>
	/// Matching: 'bankindex' is used for determining the '.fpe' entity 'type' of the entity 'e'.
	bool entityTypeIsAllowed(int e, eAddSelMethods addSelMethod)
	{
		auto entityData = Match::getEntityData(e);
		if (entityData.valid)
		{
			// objects in editor are often hidden underground with posY of -999999.0f or -48000.0f or similar, rather than having a property for this, like "inactive-"/"disabled-"/"invisible-" "AtGameStart".
			if (!SelModes::bIncludeInvisibleObjs && entityData.PY <= kfInvisibleObjsMax) // exclude underground objects, but allow partially submerged objects.
				return false; // ignore invisible objects unless user has enabled

			if (mode_isNormal())
				return true; // not in any special selection mode, so use default behavior - i.e. add to rubber band list

			if (currModeMatches(Radius) && addSelMethod == viaScan)
				return true; // does not require type matching, so allow

			if (addSelMethod == viaCtrlClick)
				return true; // all sel modes allow individual object selection - either to add to the rubberband list, or to become a seed (and maybe initiate a scan).

			// check if this entity's type is in the allowed types list
			auto* pEntry = Match::findEntryByBankindex(mode, entityData.bankindex);
			auto allowed = (pEntry != nullptr); // 'allowed' if found in list

			// If the user is in SameType mode, and user has not yet selected a seed entity, allow the currently picked entity to act as the seed entity.
			if (!allowed && currModeMatches(SameType))
			{
				auto e = t.widget.pickedEntityIndex;
				if (e > 0)
				{
					auto bankindex = t.entityelement[e].bankindex;
					allowed = (entityData.bankindex == bankindex);
				}
			}

			return allowed;
		}

		return false;
	}

	/// Purpose: draws a button for the given selection mode, and handles click events to set the mode.
	static void SelModesWindow::drawButtonForMode(eModes mode)
	{
		bool active = currModeMatches(mode);
		if (drawButtonDeactivated(ModeNames[mode].buttonLabel, active))
		{
			switchMode(mode);
		}
		ImGui::SameLine();
		if (ImGui::IsItemHovered())
		{
			// display information about mode purpose 
			ImGui::BeginTooltip();
			{
				Help::ImGui_fillRowBGColor(ImGuiCol_TabUnfocused, 6.0f);
				ImGui::Text("Switch to mode: \"%s\"", ModeNames[mode].longName);
				ImGui::Spacing();
				ImGui::Text("%s", ModeNames[mode].purposeDesc);
			}
			ImGui::EndTooltip();
		}
	}

	/// <summary>
	/// Draws the "Selection Modes" window (or tab item) to tab position under tab bar "Current Objects".<br/>
	/// </summary>
	/// Returns: true if the window was drawn and is therefore active, false otherwise.<br/>
	/// The window shows:<br/>
	///   • Mode buttons ("Normal", "Same Type", "Adjacent Type", "Nearby Lateral", "Radius")<br/>
	///   • A radius slider when Radius mode is active<br/>
	///   • A scrollable list of entity types, which will be used for choosing objects to select.<br/>
	///   • A "Clear" button - Clears the entity types list.<br/>
	///   • A "Help" button - Displays a information window describing each selection mode.<br/>
	bool SelModesWindow::drawWindow(int tabflags)
	{
		if (ImGui::BeginTabItem(SelModes::SelModesWindow::winName, nullptr, tabflags))
		{
			if (ImGui::IsItemHovered()) 
				ImGui::SetTooltip(SelModes::SelModesWindow::winName);

			auto& style = ImGui::GetStyle();

			// buttons row - modes
			for (int i = 0; i < eModes::Count; ++i)
			{
				drawButtonForMode(static_cast<eModes>(i));
			}
			ImGui::NewLine();

			if (mode_isNormal())
			{
				// objects in editor are often hidden underground with posY of -999999.0f or -48000.0f or similar, rather than having a property for this, like "inactive-"/"disabled-"/"invisible-" "AtGameStart".
				ImGui::Checkbox("Include underground objects.##InvisibleUnderground", &SelModes::bIncludeInvisibleObjs);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(
						"If unchecked, rubber-band selection will exclude underground \n"
						"(or very low, negative altitude) objects.\n"
						"\n"
						"These are normally invisible during a game, but it will include \n"
						"submerged objects (above %.0f).", kfInvisibleObjsMax);
			}

			if (currModeMatches(Radius))
			{
				static bool performScan_S = false;
				int iSelRadiusBefore = iSelRadius;

				// Show radius slider
				ImGui::Text("Radius"); ImGui::SameLine();
				if (ImGui::MaxSliderInputInt("##SelRadius", &SelModes::iSelRadius, 1, 2500, nullptr))
				{
					// treat slider as a stepped range and scale
					float fStep = 1.0f;
					if (iSelRadius <= 50.0f)
						fStep = 1.0f;
					else if (iSelRadius <= 100.0f)
						fStep = 5.0f;
					else if (iSelRadius <= 500.0f)
						fStep = 10.0f;
					else if (iSelRadius <= 750.0f)
						fStep = 25.0f;
					else // > last
						fStep = 50.0f;
					iSelRadius = roundl(iSelRadius / fStep) * fStep;

					// Prevent scan until bScanningActive msg is displayed & mouse is released.
					// Also check if value has changed. (For some reason, MaxSliderInputInt is returning true even when the value has not changed. Caused by ImGui::SliderInt().)
					if (!performScan_S && iSelRadius != iSelRadiusBefore)
					{
						performScan_S = true;
						bScanningActive = true;
					}
				}

				if (performScan_S &&
					(!ImGui::IsMouseDown(0) ||
						(std::chrono::steady_clock::now() - lastAdjustmentMs > std::chrono::milliseconds(100))))
				{
					Match::reapplyScans();
					Match::countRubberBandTypes();
					performScan_S = false;
					bScanningActive = false;
					lastAdjustmentMs = std::chrono::steady_clock::now();
				}
			}

			// calc child win Hgt
			float rowH = ImGui::GetFrameHeight() + style.ItemSpacing.y; //ImGui::GetTextLineHeightWithSpacing();
			float availH = ImGui::GetContentRegionAvail().y - rowH; // -rowH is for bottom row buttons which have not been drawn yet
			float minRows = 2.0f;
			float maxRows = 10.0f;

			float rowsFit = availH / rowH;
			float rowsVis = std::clamp(rowsFit, minRows, maxRows);
			float childH = rowH * rowsVis;

			// bullet list of types
			ImGui::BeginChild("ScrollSelList", ImVec2(0, childH), true);
			{
				bool emptyList = (AllowedTypes.size() == 0);
				if (currModeMatches(Normal))
				{
					if (emptyList)
					{
						ImGui::TextUnformatted("(No entities selected)");
						displayTip();
					}
				}
				else
				{
					bool emptyForMode = false;
					if (!emptyList)
						emptyForMode = (Match::countAllowedTypesForMode(mode) == 0);

					if (emptyList || emptyForMode)
					{
						const auto& fgColor = ImGui::GetStyleColorVec4(ImGuiCol_TabActive);
						ImGui::TextColored(fgColor, "(No seed entity added for '%s' mode yet.)", ModeNames[mode].shortName);
					}
					if (emptyList)
						displayTip();
				}

				for (auto& entry : AllowedTypes)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TabActive));
					ImGui::BulletText("(%d) ", entry.typeCount);
					ImGui::PopStyleColor();

					ImGui::SameLine();
					ImGui::TextUnformatted(t.entityprofileheader[entry.bankindex].desc_s.Get());

					ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TabActive));
					{
						constexpr bool allowSelUsingMultModes = false;
						// if (!prefs.allowSelUsingMultModes)
						if (allowSelUsingMultModes)
						{
							ImGui::SameLine();
							ImGui::Text(" (%s)", ModeNames[entry.modeUsed].shortName);
						}
						if (entry.seedEntityId > 0 && 
							entry.bankindex == t.entityelement[entry.seedEntityId].bankindex)
						{
							// Modes: Normal, are never "Seed". - does not require "Seed" msg at all.
							// Modes: Adjacent, Nearby, are always "Seed". - does not require full "Seed" msg, use "* ".
							// Modes: SameType, Radius, are not always "Seed". - requires "Seed" msg to know which is which.
							ImGui::SameLine();
							ImGui::TextUnformatted("  (Seed)");
						}
					}
					ImGui::PopStyleColor();
				}
			}
			ImGui::EndChild();

			// buttons row - Clear, Help
			if (ImGui::Button("Clear##BtnClearTypes"))
			{
				clearAllowedTypesAndSelections();
			}
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Clear the list of allowed types & selections.\nOr press [Spacebar] to clear.");
			ImGui::SameLine();

			if (ImGui::Button("Help##BtnSelModeHelp"))
			{
				Help::showWindow = true;
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Show help info for each mode");

			if (Help::showWindow)
			{
				if (!ImGui::IsPopupOpen(Help::winLabel))
					ImGui::OpenPopup(Help::winLabel);
				Help::drawWindow();
			}

			ImGui::EndTabItem();
			//if (ImGui::IsItemHovered()) ImGui::SetTooltip(SelModes::SelModesWindow::winName);
			return true;
		}
		return false;
	}

	/// Purpose: Decide whether to show the instruction for each mode
	static bool SelModesWindow::doShowTip()
	{
		if (currModeMatches(Radius))
			return true;

		auto size = AllowedTypes.size();
		if (isRubberBandDependent(mode) &&
			(size <= 4)) // show tip if list has 0 to 3 types, beyond this, it is assumed user will know what to press
		{
			return true;
		}

		// for remaining modes
		if (size == 0 || // list is empty
			(size <= 5 && AllowedTypes[0].typeCount <= 1)) // list[0] has less than 2 items. 2+ items would indicate a scan after C+LMB was pressed.
		{
			return true;
		}
		return false;
	}

	/// Draws the tip text since users may not realise this requirement.
	static void SelModesWindow::displayTip()
	{
		// don't show tip, if LMB feature is already in use
		if (doShowTip())
		{
			if (ImGui::GetContentRegionAvail().x < 230.0f)
				return; // available space is too narrow to bother with a tip.

			// Show "Press [Ctrl]+[Left - Click] to add an entity type to the selection list and to start scan"
			{
				ID3D11ShaderResourceView* iconID = nullptr;
				auto& style = ImGui::GetStyle();
				const auto iconSize = ImVec2(24.0f, 24.0f);

				static const char* modeText_S[] =
				{
					" to add a single entity to selections.", // Normal
					" to add an entity type to match against.", // SameType
					" to add an entity type, find and select adjacent aligned entities.", // AdjacentTypes
					" to add an entity type, find and select nearby lateral entities.", // NearbyLateral
					" to select all entities within the radius of the selected entity." // Radius
				};
				Help::drawKeyAndTextTip("Press", Help::eKeyCombo::Ctrl_LMB, modeText_S[mode]);
			}
		}
	}

	/// Draws a button, but also deactivates the active button to prevent reselect. These buttons are more like tabs.
	static bool SelModesWindow::drawButtonDeactivated(const char* label, bool active)
	{
		if (active) // draws the button with different colors, if active (button mode matches current mode).
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f); // Alpha

			// Background colors (Button / Hovered / Active)
			const ImVec4& bg = ImGui::GetStyleColorVec4(ImGuiCol_TabActive);
			ImGui::PushStyleColor(ImGuiCol_Button, bg);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, bg);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, bg);

			// Foreground (text) color
			// Unused. Intended for future use, when color themes are improved.
			//ImVec4 fg = ImGui::GetStyleColorVec4(ImGuiCol_TabActive);
			//ImGui::PushStyleColor(ImGuiCol_Text, fg);
		}

		// Handle real-time left panel width changes. Calc if button will fit on same line.
		float availWidth = ImGui::GetContentRegionAvail().x;
		if (availWidth < 350.0f) // >= 350.0f is arbitrary, but this width normally allows all buttons on the same line.
		{
			float buttonWidth = ImGui::CalcTextSize(label, nullptr, true).x + ImGui::GetStyle().FramePadding.x * 2.0f;
			float spacing = ImGui::GetStyle().ItemSpacing.x;
			float nextX = ImGui::GetCursorPosX() + buttonWidth + spacing;
			if (nextX > ImGui::GetWindowContentRegionMax().x &&
				ImGui::GetCursorPosX() > ImGui::GetWindowContentRegionMin().x)
				ImGui::NewLine();
		}

		bool clicked = ImGui::Button(label);

		if (active)
		{
			ImGui::PopStyleColor(3);   // BG colors: Button, Hovered, Active, Text
			ImGui::PopStyleVar();      // Alpha
			ImGui::PopItemFlag();
		}

		return clicked && !active;
	}

	/// Purpose: adds a new SelTypeEntry to the AllowedTypes vector.
	static inline void Match::addEntityType_StructEntry(
		int bankindex,
		int seedEntityId,
		float PY,
		int count,
		eModes modeUsed /* = CurrMode */)
	{
		ReplaceIfCurrent(modeUsed);
		// Store 0.0f if not required for this mode, or entity has no obj ID or obj, to simplify data.
		float fPYToUse = (modeUsed == NearbyLateral) ? PY : 0.0f;

		try
		{
			AllowedTypes.emplace_back(modeUsed, bankindex, seedEntityId, fPYToUse, count);
		}
		catch (const std::bad_alloc&)
		{
			// Log, warn user, and abort the add
			MessageBoxA(NULL, "Unable to add selection type due to low memory.", "Error", MB_OK);
		}
	}

	/// <Summary>
	/// Returns: a pointer to the SelTypeEntry in AllowedTypes that matches the given bankindex, 
	/// or nullptr if not found.
	/// </Summary>
	/// The bank index is used to match the entity "type". Essentially, the ".fpe" index.
	static SelTypeEntry* Match::findEntryByBankindex(eModes modeUsed, int bankindex)
	{
		if (bankindex == 0)
			return nullptr;

		for (auto& entry : AllowedTypes)
		{
			if (entry.modeUsed == modeUsed && entry.bankindex == bankindex)
				return &entry;
		}
		return nullptr;
	}

	/// <Summary>
	/// Returns: a pointer to the SelTypeEntry in AllowedTypes that matches the given seedEntityId, 
	/// or nullptr if not found.
	/// </Summary>
	static SelTypeEntry* Match::findEntryBySeedEntityId(eModes modeUsed, int bankindex, int entityId)
	{
		if (entityId == 0)
			return nullptr;

		for (auto& entry : AllowedTypes)
		{
			if (entry.modeUsed == modeUsed && entry.bankindex == bankindex && entry.seedEntityId == entityId)
				return &entry;
		}
		return nullptr;
	}

	/// <Summary>Checks if the given entity is already in the current rubberband selection list.</Summary>
	/// <Returns>true if the entity is in the selection list.</Returns>
	static bool Match::isEntityInRubberBandList(int e)
	{
		for (const auto& rb : g.entityrubberbandlist)
			if (rb.e == e)
				return true;
		return false;
	}

	/// <Summary>Counts the number of entities of each type in the current rubberband selection list, 
	/// and updates the AllowedTypes vector with the counts.</Summary>
	void Match::countRubberBandTypes()
	{
		bool changed = false;
		/*
		DebugLocal::output_allowedTypes(__func__, "before");
		DebugLocal::output_entityrubberbandlist();
		*/

		// Reset counts. Delay erasing zero counts until the end, to allow for reuse.
		for (auto& entry : AllowedTypes)
			entry.typeCount = 0;

		// Alternate idea on how to count types, but it is not as accurate as counting the actual entities in the rubberband list.
		//Match::reapplyScans();
		//return;

		// Count the number of entities of each type in the current rubberband selection list, and update the AllowedTypes vector with the counts.
		for (const auto& item : g.entityrubberbandlist)
		{
			if (item.e == 0)
				continue;

			//auto bankindex = t.entityelement[item.e].bankindex;
			int seedId = item.seedEntityId;
			SelModes::eModes modeUsed = IntToMode(item.modeUsed);

			addEntityType(item.e, seedId, viaScan, modeUsed);
			changed = true;
		}

		// Remove unused types with zero count
		for (auto entry = AllowedTypes.begin(); entry != AllowedTypes.end(); )
		{
			// if "SameType", it is probably worth keeping the already chosen seed types (don't erase), to not require reselecting seed types and to allow you to rubber-band select a different area or range. Same for "Radius"?
			bool allowDeletion = true; // !(entry->modeUsed == SameType && entry->typeCount == 0 && entry->seedEntityId > 0); // disable concept for now.

			if (entry->typeCount == 0 && allowDeletion)
			{
				entry = AllowedTypes.erase(entry); // erase returns the next iterator
				changed = true;
			}
			else
				++entry;
		}
		if (changed) {
			//DebugLocal::output_allowedTypes(__func__, "after");
		}
	}

	/// Returns: a count of AllowedTypes matching given selection mode.
	static int Match::countAllowedTypesForMode(eModes mode)
	{
		int c = 0;
		for (auto& entry : AllowedTypes)
		{
			if (entry.modeUsed == mode)
				++c;
		}
		return c;
	}

	/// Purpose: checks whether all entries in AllowedTypes list are of the given mode.
	// Unused. Intended for future use, when allowSelUsingMultModes is completed.
	static bool Match::allEntriesMatchMode(eModes mode_param)
	{
		bool allMatch = false;
		//if (modesMatch(mode_param, mode))
		//{
			// if list is empty, allMatch = false
			if (!AllowedTypes.empty())
			{
				allMatch = true;
				for (const auto& entry : AllowedTypes)
				{
					if (entry.modeUsed != mode_param)
					{
						allMatch = false;
						break;
					}
				}
			}
		//}
		return allMatch;
	}

	/// Returns: the verified data struct of the verified given entity with struct.valid = true, 
	/// or a data struct.valid = false if the entity is invalid.
	static Match::EntityData Match::getEntityData(int e)
	{
		if (e > 0 && e < t.entityelement.size()) // size includes +1 for unused elem[0], so 1 elem has size == 2.
		{
			auto& entity = t.entityelement[e];
			int obj = entity.obj;

			if ((obj > 0 && obj < g_iObjectListCount) // not all entities have an object (perhaps game elements)
				&& g_ObjectList[obj] != nullptr)
			{
				return Match::EntityData{
					true,
					entity.bankindex,
					entity.y,
					obj };
			}
		}

		static const EntityData invalid_entityData{ false, 0, 0.0f, 0 };
		return invalid_entityData;
	}

	/// Returns: the distance between the two objects' world positions relative to their origins (PX, PY, PZ).
	static float Match::getDist2Objs(int e1, int e2)
	{
		auto obj1 = t.entityelement[e1].obj;
		auto obj2 = t.entityelement[e2].obj;
		if (obj1 == 0 || obj2 == 0)
			return FLT_MAX; // no obj id

		auto distx_f = ObjectPositionX(obj2) - ObjectPositionX(obj1);
		auto disty_f = ObjectPositionY(obj2) - ObjectPositionY(obj1);
		auto distz_f = ObjectPositionZ(obj2) - ObjectPositionZ(obj1);
		auto dist_f = Sqrt(distx_f * distx_f + disty_f * disty_f + distz_f * distz_f);
		return dist_f;
	}

	/// Returns: true if at least one of the axis world origin positions (PX, PY, PZ) of the two objects' are the same, or along the same plane.
	static bool Match::samePlane2Objs(int e1, int e2)
	{
		auto obj1 = t.entityelement[e1].obj;
		auto obj2 = t.entityelement[e2].obj;

		auto same =
			(ObjectPositionX(obj2) == ObjectPositionX(obj1)) ||
			(ObjectPositionY(obj2) == ObjectPositionY(obj1)) ||
			(ObjectPositionZ(obj2) == ObjectPositionZ(obj1));

		return same;
	}

	/// Returns: true if the two objects are of the same type (i.e. same '.fpe' file, determined via bankindex).
	static bool Match::objectIsSameType(int e2, int bankindex_e1)
	{
		if (e2 == 0 || bankindex_e1 <= 0)
			return false;

		auto bankindex_e2 = t.entityelement[e2].bankindex;
		return (bankindex_e2 > 0 && bankindex_e2 == bankindex_e1);
	}

	/// Returns: true if the two objects are too far apart for selection purposes.
	static bool Match::objectIsTooFar(int e1, int e2)
	{
		if (!samePlane2Objs(e1, e2))
			return true; // not aligned

		auto obj1 = t.entityelement[e1].obj;
		auto obj2 = t.entityelement[e2].obj;
		if (obj1 == 0 || obj2 == 0 || obj1 >= g_iObjectListCount || obj2 >= g_iObjectListCount)
			return true; // no obj id

		auto* pObj1 = g_ObjectList[obj1];
		auto* pObj2 = g_ObjectList[obj2];
		if (!pObj1 || !pObj2)
			return true; // no obj

		float epsilon1 = 0.20f; // arbitrary, focuses mainly on: 0.01 to 0.20 misalignments.
		float portion2 = 1.10f;  // "1.1 *" would handle: "a wall, a 10% gap, and another wall".
		//float portion2 = 2.0f; // "2.0 *" would handle: "a wall, a wall gap, and another wall".

		// this is currently just a very rough estimate to exclude far objects.
		auto maxDist_f = pObj1->collision.fScaledRadius + pObj2->collision.fScaledRadius;
		// TODO: verify whether radius encompasses the entire bounding box (BB), or if it only encompasses the origins to the nearest 4 edges of BB.

		if (currModeMatches(AdjacentTypes))
		{
			// if (dist_f) > sum of their scaled BB radius + some epsilon, then too far
			maxDist_f = maxDist_f + epsilon1;
			// if too far, then skip the more expensive AABB check
		}
		else if (currModeMatches(NearbyLateral))
		{
			// if (dist_f) > double the sum of their scaled bounding box radius, then too far
			maxDist_f = (portion2 * maxDist_f);
			// if too far, then skip the (slightly) more expensive nearby PY check
		}

		auto dist_f = getDist2Objs(e1, e2); // distance between object origins

		if (dist_f > maxDist_f)
			return true; // too far
		return false; // suitable
	}

	/// Returns: true if the two intervals align.
	static inline bool Match::fullyAlign(float aMin, float aMax, float bMin, float bMax)
	{
		const float eps = 0.11f;
		// allow small epsilon slack
		return
			(fabs(aMin - bMin) <= eps) &&
			(fabs(aMax - bMax) <= eps);
	}

	/// Returns: true if the two faces touch.
	static inline bool Match::facesTouch(float aMin, float aMax, float bMin, float bMax)
	{
		const float eps = 0.10f;
		// allow small epsilon slack
		return
			(fabs(aMax - bMin) <= eps) ||
			(fabs(aMin - bMax) <= eps);
	}

	/// <Summary> Calculates the world-space AABB of the entity's object, (taking into account its 
	/// position and RY rotation).<br/>
	/// AABB = axis-aligned bounding box.</Summary>
	/// <Returns> the world-space AABB of the entity's object.</Returns>
	static bool Match::getWorldAABB(int e, GGVECTOR3& outMin, GGVECTOR3& outMax)
	{
		auto obj = t.entityelement[e].obj;
		if (obj == 0)
			return false; // no obj id

		auto* pObj = g_ObjectList[obj];
		if (!pObj)
			return false; // no obj

		auto isGridRot = [](int obj)
		{
			int ry = (int)std::lround(ObjectAngleY(obj));
			ry = (ry % 360 + 360) % 360;
			return (ry % 90) == 0;
		};

		if (!isGridRot(obj))
			return false; // not grid aligned

		// Local AABB (assumed in object local space)
		// TODO: check if this BB is scaled, otherwise find alt scaled BB.
		auto localMin = pObj->collision.vecMin;
		auto localMax = pObj->collision.vecMax;

		// World position
		GGVECTOR3 pos = { ObjectPositionX(obj), ObjectPositionY(obj), ObjectPositionZ(obj) };

		// Rotation (grid-aligned: 0, 90, 180, 270)
		int ry = static_cast<int>(std::lround(ObjectAngleY(obj))) % 360;
		if (ry < 0) ry += 360;

		// Build the 8 corners of the local AABB
		std::array<GGVECTOR3, 8> corners;
		int idx = 0;
		for (int xi = 0; xi < 2; ++xi)
			for (int yi = 0; yi < 2; ++yi)
				for (int zi = 0; zi < 2; ++zi)
				{
					corners[idx++] = {
						xi ? localMax.x : localMin.x,
						yi ? localMax.y : localMin.y,
						zi ? localMax.z : localMin.z
					};
				}

		// Transform corners by rotation (only Y rotation, axis-aligned)
		auto rotateY_axisAligned = [&](const GGVECTOR3& c)->GGVECTOR3
		{
			switch (ry)
			{
				case 0: return { c.x, c.y, c.z };
				case 90: return { -c.z, c.y, c.x }; // (x,z) -> (-z, x)
				case 180: return { -c.x, c.y, -c.z };
				case 270: return { c.z, c.y, -c.x }; // (x,z) -> (z, -x)
				default: return { c.x, c.y, c.z }; // Fallback: no rotation (shouldn't happen if grid-aligned)
			}
		};

		// Compute world-space min/max by rotating each corner and adding pos
		GGVECTOR3 wmin = { +FLT_MAX, +FLT_MAX, +FLT_MAX };
		GGVECTOR3 wmax = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

		for (const auto& c : corners)
		{
			GGVECTOR3 rc = rotateY_axisAligned(c);
			// add object position
			rc.x += pos.x;
			rc.y += pos.y;
			rc.z += pos.z;

			wmin.x = std::min(wmin.x, rc.x);
			wmin.y = std::min(wmin.y, rc.y);
			wmin.z = std::min(wmin.z, rc.z);

			wmax.x = std::max(wmax.x, rc.x);
			wmax.y = std::max(wmax.y, rc.y);
			wmax.z = std::max(wmax.z, rc.z);
		}

		outMin = wmin;
		outMax = wmax;
		return true; // success
	}

	/// Returns: true if the two objects' AABBs are adjacent (touching faces).
	static bool Match::adjacentAABB(int e1, int e2)
	{
		GGVECTOR3 aMin, aMax;
		GGVECTOR3 bMin, bMax;

		if (!getWorldAABB(e1, aMin, aMax))
			return false; // no obj id or obj
		if (!getWorldAABB(e2, bMin, bMax))
			return false; // no obj id or obj

		auto faceTouch_X = facesTouch(aMin.x, aMax.x, bMin.x, bMax.x);
		auto faceTouch_Y = facesTouch(aMin.y, aMax.y, bMin.y, bMax.y);
		auto faceTouch_Z = facesTouch(aMin.z, aMax.z, bMin.z, bMax.z);

		auto fullyAlign_X = fullyAlign(aMin.x, aMax.x, bMin.x, bMax.x);
		auto fullyAlign_Y = fullyAlign(aMin.y, aMax.y, bMin.y, bMax.y);
		auto fullyAlign_Z = fullyAlign(aMin.z, aMax.z, bMin.z, bMax.z);

		bool adjacent_X = faceTouch_X && fullyAlign_Y && fullyAlign_Z;
		bool adjacent_Y = faceTouch_Y && fullyAlign_X && fullyAlign_Z;
		bool adjacent_Z = faceTouch_Z && fullyAlign_X && fullyAlign_Y;

		return adjacent_X || adjacent_Y || adjacent_Z;
	}

	/// Returns: true if the two objects' world PY positions are almost the same.
	static bool Match::sharesSamePY(int e1, int e2)
	{
		auto obj1 = t.entityelement[e1].obj;
		auto obj2 = t.entityelement[e2].obj;
		if (obj1 == 0 || obj2 == 0)
			return false; // no obj id

		float epsilon = 0.11f; // arbitrary, focuses mainly on 0.01 to 0.11 misalignments.

		// World PY
		auto PY1 = ObjectPositionY(obj1);
		auto PY2 = ObjectPositionY(obj2);

		return (fabs(PY1 - PY2) <= epsilon);
	}

	/// <Summary>Use AABB of currently selected entity to (recursively) find adjacent entities of same type 
	/// and add to selection.</Summary><br/>
	/// BB RY can be at any right-angle to grid.<br/>
	/// AABB = Axis-Aligned Bounding Box.<br/>
	static void Match::scanForAdjacentAABB_recusive(int e1, int seedEntityId)
	{
		auto bankindex_e1 = t.entityelement[e1].bankindex;
		if (bankindex_e1 <= 0)
			return;

		// skip adding type, if already processed
		if (!isEntityInRubberBandList(e1))
		{
			gridedit_addEntityToRubberBandHighlights(e1, viaScan, seedEntityId, AdjacentTypes);
			addEntityType(e1, seedEntityId, viaScan, AdjacentTypes);
		}

		for (auto e2 = 1; e2 <= g.entityelementlist; e2++) // 'entityelementlist' means list 'size'
		{
			// checks are ordered to minimise time for large RubberBandList
			if (e2 == e1)
				continue;

			// Skip if different type
			if (!objectIsSameType(e2, bankindex_e1))
				continue;

			// Skip if too far
			if (objectIsTooFar(e1, e2))
				continue; // if too far away, skip the AABB check

			// Skip if already processed
			if (isEntityInRubberBandList(e2))
				continue;

			// Skip if 2 objects are not adjacent
			if (!adjacentAABB(e1, e2))
				continue;

			// Recursively add this object, then scan it's adjacent neighbors
			scanForAdjacentAABB_recusive(e2, seedEntityId);
		}
	}

	/// <Summary>Use PY of currently selected entity to find nearby entities (of same type) and add to selection.</Summary>
	/// BB RY can be at any angle.<br/>
	static void Match::scanForNearbyPY_recusive(int e1, int seedEntityId)
	{
		auto bankindex_e1 = t.entityelement[e1].bankindex;
		if (e1 == 0 || bankindex_e1 <= 0)
			return;

		// skip adding type, if already processed
		if (!isEntityInRubberBandList(e1))
		{
			gridedit_addEntityToRubberBandHighlights(e1, viaScan, seedEntityId, NearbyLateral);
			addEntityType(e1, seedEntityId, viaScan, NearbyLateral);
		}

		for (int e2 = 1; e2 <= g.entityelementlist; e2++) // 'entityelementlist' means list 'size'
		{
			// checks are ordered to minimise time for large RubberBandList
			if (e2 == e1)
				continue;

			// Skip if different type
			if (!objectIsSameType(e2, bankindex_e1))
				continue;

			// Skip if different PY
			if (!sharesSamePY(e1, e2))
				continue;

			// Skip if too far
			if (objectIsTooFar(e1, e2))
				continue; // if too far away, skip the Nearby PY check

			// Skip if already processed
			if (isEntityInRubberBandList(e2))
				continue;

			// Recursively add this object, then scan it's nearby neighbors
			scanForNearbyPY_recusive(e2, seedEntityId);
		}
	}

	/// Use the currently selected entity to find all entities within a given radius and add to selection.
	static void Match::scanForObjectsWithinRadius(int seedEntityId)
	{
		auto bankindex_e1 = t.entityelement[seedEntityId].bankindex;
		if (seedEntityId == 0 || bankindex_e1 <= 0)
			return;

		for (auto e2 = 1; e2 <= g.entityelementlist; e2++) // 'entityelementlist' means list 'size'
		{
			// Skip if too far
			// if (dist_f) > their origin position distance, then too far
			if (getDist2Objs(seedEntityId, e2) > SelModes::iSelRadius)
				continue; // if too far away, skip

			if (!isEntityInRubberBandList(e2))
			{
				// Add this entity type to the list
				gridedit_addEntityToRubberBandHighlights(e2, viaScan, seedEntityId, Radius);
				addEntityType(e2, seedEntityId, viaScan, Radius);
			}
		}
	}

	/// Purpose: Clears all scanning-related selections (Adjacent, Nearby, Radius), leaving rubber-band 
	/// related selections only (Normal, SameType).
	static void Match::clearScanningSelections()
	{
		for (auto entry = g.entityrubberbandlist.begin(); entry != g.entityrubberbandlist.end(); )
		{
			bool allowDeletion = !isRubberBandDependent(IntToMode(entry->modeUsed));

			if (entry->modeUsed == 0 && allowDeletion)
				entry = g.entityrubberbandlist.erase(entry); // erase returns the next iterator
			else
				++entry;
		}
	}

	/// Purpose: Clears all selections, then reapplies scans for each type to reselect entities using 
	/// current selection properties. Eg: radius.
	static void Match::reapplyScans()
	{
		constexpr bool allowSelUsingMultModes = false; // = prefs.allowSelUsingMultModes;

		if (allowSelUsingMultModes)
			Match::clearScanningSelections();
		else
			g.entityrubberbandlist.clear();

		// reapply each scan to reselect entities using current selection properties. Eg: radius.
		for (auto& entry : AllowedTypes)
		{
			auto eSeed = entry.seedEntityId;
			if (eSeed > 0)
			{
				if (allowSelUsingMultModes)
				{
					// clearScanningSelections() retains Normal and SameType non-scanning selections, so reapply scans for the other modes.
					//if (entry.modeUsed == Normal) {}
					//else if (entry.modeUsed == SameType) {} else

					if (entry.modeUsed == AdjacentTypes)
						Match::scanForAdjacentAABB_recusive(eSeed, eSeed);
					else if (entry.modeUsed == NearbyLateral)
						Match::scanForNearbyPY_recusive(eSeed, eSeed);
					else if (entry.modeUsed == Radius)
						Match::scanForObjectsWithinRadius(eSeed);
						/* TODO: for Radius, the rescan should only apply to the current Radius seed. 
							Consider the same for other modes too. 
							Perhaps save each modes' properties in the AllowedTypes struct.
						*/
				}
				else 
				{
					if (entry.modeUsed == Radius)
						Match::scanForObjectsWithinRadius(eSeed);
						// TODO: for Radius, the rescan should only apply to the current Radius seed.
				}
			}
		}
	}

	/// Purpose: Centers the modal window on the screen, regardless of the current window size or auto-sizing.
	void Help::centerModalWindow()
	{
		ImGuiViewport* vp = ImGui::GetMainViewport();
		ImVec2 center(vp->Pos.x + vp->Size.x * 0.5f,
			vp->Pos.y + vp->Size.y * 0.5f);

		ImVec2 size = ImGui::GetWindowSize();
		ImVec2 pos(center.x - size.x * 0.5f,
			center.y - size.y * 0.5f);

		ImGui::SetWindowPos(pos, ImGuiCond_Always);
	}

	/// Draws the instruction text for: "Press" [Ctrl]+[Left-Click] (action) "to add an entity type..." or similar.
	static inline void Help::drawPressLMBTip(const char* action /* = "" */)
	{
		Help::drawKeyAndTextTip("Press", Help::eKeyCombo::Ctrl_LMB, action);
	}

	/// Draws the instruction text for [Ctrl]+[Left-Click] to add an entity type to the selection list and to start scan.
	static void Help::drawKeyAndTextTip(const char* press, eKeyCombo keyComboId, const char* action)
	{
		// crop 1/6th of the blank border off for 60x60 icon, about 10px on each side
		const float crop = 1.0f / 6.0f;
		const ImVec2 iconSize(16.0f, 16.0f);
		const ImVec2 uv0(crop, crop);
		const ImVec2 uv1(1.0f - crop, 1.0f - crop);

		struct KeyIcon
		{
			int id;
			const char* alt;
		};

		// Two sequences, each containing three KeyIcon entries
		constexpr int size = 3;
		static constexpr KeyIcon keyComboSets[Help::eKeyCombo::Count][size] =
		{
			{
				{KEY_CONTROL, "Ctrl"},
				{KEY_SEPARATOR, "+"},
				{MOUSE_LMB, "Left-Click "}
			},
			{
				{0, ""},
				{0, ""},
				{0 /*KEY_SPACE*/, "Spacebar "} // not using KEY_SPACE icon - it is hard to read (and wrong symbol, should be square 'u' shape)
			}
		};

		ImGui::Text(press); // "Press"
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.84f, 0.84f, 0.84f, 1.0f));
		for (int i = 0; i < size; ++i)
		{
			const auto& k = keyComboSets[keyComboId][i];
			if (k.id || k.alt[0])
			{
				auto iconID = (k.id) ? GetImagePointerView(k.id) : 0;

				(iconID)
					? ImGui::Image(iconID, iconSize, uv0, uv1)
					: ImGui::Text(k.alt);
				ImGui::SameLine(0.0f, 0.0f); // spacing (2nd param) is zero to draw icons close together.
			}
		}
		ImGui::PopStyleColor();

		ImGui::TextWrapped(action); // "to do..."
	}

	/// Draws a filled rectangle behind the current row of text.
	// TODO: move to namespace ImGui, or to a new namespace ImGui_utils
	static void Help::ImGui_fillRowBGColor(ImGuiCol bgColor /* = ImGuiCol_MenuBarBg */, float rounding /* = 0.0f */)
	{
		// bg color
		const auto& color = ImGui::GetStyle().Colors[bgColor];
		ImU32 bgU32 = ImGui::ColorConvertFloat4ToU32(color);

		// row rectangle size
		const auto& p_min = ImGui::GetCursorScreenPos();
		const auto& p_max = ImVec2(
			p_min.x + ImGui::GetContentRegionAvail().x, // Full width of the content region
			p_min.y + ImGui::GetTextLineHeight()); // Height of a text row (matches ImGui::Text)

		ImGui::GetWindowDrawList()->AddRectFilled(p_min, p_max, bgU32, 6.0f);
	}

	/// Show header text for each modes help topic.
	static void Help::drawHeader(eModes mode)
	{
		if (SelModes::currModeMatches(mode))
		{
			Help::ImGui_fillRowBGColor(ImGuiCol_TabUnfocused);
			ImGui::TextCenter("%d. %s", mode, SelModes::ModeNames[mode].longName);

			ImGui::SameLine();
			const auto& fgColor = ImGui::GetStyle().Colors[ImGuiCol_CheckMark];
			ImGui::TextColored(fgColor, "  <-- Current Mode");
		}
		else
		{
			const auto& fgColor = ImGui::GetStyle().Colors[ImGuiCol_TabActive]; // not a good FG src
			ImGui::PushStyleColor(ImGuiCol_Text, fgColor);
			ImGui::TextCenter("%d. %s", mode, SelModes::ModeNames[mode].longName);
			ImGui::PopStyleColor();
		}
		ImGui::Spacing();
	}

	/// Show a colored bullet point text
	static void Help::drawBullet(const char* text)
	{
		const auto& captionColor = ImGui::GetStyle().Colors[ImGuiCol_TabActive]; // not a good FG src
		ImGui::PushStyleColor(ImGuiCol_Text, captionColor);
		ImGui::Bullet();
		ImGui::PopStyleColor();
		ImGui::TextUnformatted(text);
	}

	/// Show a button to close the help window, and also allow Esc key to close it.
	static void Help::checkForClose(bool* pShowWin)
	{
		bool doClose = false;
		if (ImGui::Button("Close (Esc)"))
			doClose = true;
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Close help window to continue");

		if (ImGui::GetIO().KeysDown[VK_ESCAPE])
			doClose = true;
		if (doClose)
		{
			if (pShowWin)
				*pShowWin = false;
			ImGui::CloseCurrentPopup();
		}
	}

	// Replaces 
	static std::string Help::ReplaceLFWithSpaces(const char* s)
	{
		std::string out(s);
		std::replace(out.begin(), out.end(), '\n', ' ');
		return out;
	}

	/// Selection Mode Help - popup modal window
	static void Help::drawWindow()
	{
		if (!Help::showWindow) return;

		auto& style = ImGui::GetStyle();
		ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, ImVec4(0, 0, 0, 0)); // remove overlay
		//ImGui::PushStyleColor(ImGuiCol_WindowBg, style.Colors[ImGuiCol_WindowBg]);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.05f, 0.05f, 0.10f, 1.0f));

		auto prevColor = style.Colors[ImGuiCol_PopupBg];
		style.Colors[ImGuiCol_PopupBg] = style.Colors[ImGuiCol_WindowBg];

		ImGui::SetNextWindowPosCenter(ImGuiCond_Always);

		const ImGuiWindowFlags flags =
			ImGuiWindowFlags_Modal |
			ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoMove;  // optional

		if (ImGui::BeginPopupModal(Help::winLabel, &Help::showWindow, flags))
		{
			//Help::centerModalWindow();
			Help::drawPressLMBTip(" means [Ctrl]+[Left-Mouse-Button].");
			ImGui::Separator();

			// 0. Normal Selection Mode
			Help::drawHeader(Normal);
			ImGui::Spacing();
			Help::drawPressLMBTip(" -and-drag to use rubberband selection.");
			ImGui::TextUnformatted(ReplaceLFWithSpaces(ModeNames[Normal].purposeDesc).c_str());
			Help::drawPressLMBTip(" to add a single 'entity' to the selections.");
			ImGui::Separator();

			// 1. Select by Type
			Help::drawHeader(SameType);
			ImGui::Spacing();
			Help::drawPressLMBTip(" to add an 'entity type'.");
			Help::drawPressLMBTip(" -and-drag to use rubberband selection.");
			ImGui::TextUnformatted(ReplaceLFWithSpaces(ModeNames[SameType].purposeDesc).c_str());
			ImGui::Separator();

			// 2. Select Adjacent Types
			Help::drawHeader(AdjacentTypes);
			ImGui::Spacing();
			Help::drawPressLMBTip(" to find matching entities.");
			ImGui::TextUnformatted(ReplaceLFWithSpaces(ModeNames[AdjacentTypes].purposeDesc).c_str());
			Help::drawBullet("Object must be physically adjacent to the selected entity, even vertically above.");
			Help::drawBullet("RY must be at a right-angle to grid."); // Unsure if this is correct.
			ImGui::Separator();

			// 3. Select Nearby Lateral Types
			Help::drawHeader(NearbyLateral);
			ImGui::Spacing();
			Help::drawPressLMBTip(" to find matching entities.");
			ImGui::TextUnformatted(ReplaceLFWithSpaces(ModeNames[NearbyLateral].purposeDesc).c_str());
			Help::drawBullet("PY must match, i.e. be \"Level\". Entity must be horizontally(laterally) aligned.");
			Help::drawBullet("RY can be at any angle.");
			Help::drawBullet("Follows nearby objects around corners.");
			Help::drawBullet("\"Nearby\" means maximum adjacent distance (eg: walls side-by-side) + 10%.");
			ImGui::Separator();

			// 4. Select by Radius
			Help::drawHeader(Radius);
			ImGui::Spacing();
			Help::drawPressLMBTip(" to find matching entities.");
			ImGui::TextUnformatted(ReplaceLFWithSpaces(ModeNames[Radius].purposeDesc).c_str());
			ImGui::TextUnformatted("Use the radius slider to expand or contract the radius.");
			ImGui::Separator();

			Help::drawKeyAndTextTip("Press", Help::eKeyCombo::Spacebar, "to clear selections, then press again to switch back to \"Normal\" selection mode.");

			Help::checkForClose(&Help::showWindow);
			ImGui::EndPopup();
		}
		ImGui::PopStyleColor(2);
		style.Colors[ImGuiCol_PopupBg] = prevColor;
	}
	/// <summary>
	///  Purpose: output AllowedTypes array contents to debug console
	/// </summary>
	static void DebugLocal::output_allowedTypes(
		const char* callerFuncName /* = "" */, 
		const char* text /* = "" */)
	{
		if (!IsDebuggerPresent() || AllowedTypes.size() == 0)
			return;

		if (callerFuncName && callerFuncName[0])
			Debug::debugOutput("%s(): %s", callerFuncName, text);
		Debug::debugOutput("AllowedTypes: ");

		int i = 0;
		for (const auto& entry : AllowedTypes)
		{
			int seedBankindex = entry.seedEntityId > 0 ? t.entityelement[entry.seedEntityId].bankindex : -1;
			Debug::debugOutput("\t[%d]: { %s, bank: %d, seed: %d, seed bank: %d, %s }, count: %d, [%s, %s] \t\t%s",
				i,
				ModeNames[entry.modeUsed].shortName,
				entry.bankindex,
				entry.seedEntityId,
				seedBankindex,
				(seedBankindex > 0 && seedBankindex == entry.bankindex) ? "isSeed" : "--",
				entry.typeCount,
				entry.bankindex > 0 ? t.entityprofileheader[entry.bankindex].desc_s.Lower().Get() : "",
				seedBankindex > 0 ? t.entityprofileheader[seedBankindex].desc_s.Lower().Get() : "",
				entry.seedEntityId > 0 ? "info." : "warn!" // colorize output (eg: ext VSColorOutput64)
			);
			++i;
		}
		Debug::debugOutput("");
	}
	/// <summary>
	///  Purpose: output entityrubberbandlist array contents to debug console
	/// </summary>
	static void DebugLocal::output_entityrubberbandlist(
		const char* callerFuncName /* = "" */, 
		const char* text /* = "" */)
	{
		if (!IsDebuggerPresent() || g.entityrubberbandlist.size() == 0)
			return;

		if (callerFuncName && callerFuncName[0])
			Debug::debugOutput("%s(): %s", callerFuncName, text);
		Debug::debugOutput("g.entityrubberbandlist: ");

		int i = 0;
		for (const auto& entry : g.entityrubberbandlist)
		{
			int eBankindex = t.entityelement[entry.e].bankindex;
			int seedBankindex = t.entityelement[entry.seedEntityId].bankindex;
			Debug::debugOutput("\t[%d]: { e: %d, %s, seed: %d%s }, [%s,%s]  \t\t%s",
				i,
				entry.e,
				ModeNames[entry.modeUsed].shortName,
				entry.seedEntityId,
				entry.seedEntityId == entry.e ? "==Same" : "",
				eBankindex > 0 ? t.entityprofileheader[eBankindex].desc_s.Lower().Get() : "",
				seedBankindex > 0 ? t.entityprofileheader[seedBankindex].desc_s.Lower().Get() : "",
				entry.seedEntityId > 0 ? "info." : "warn!" // colorize output
			);
			++i;
		}
		Debug::debugOutput("");
	}
	/// <summary>
	///  Purpose: output addEntityType() findBySeed-related data to debug console
	/// </summary>
	static void DebugLocal::output_addEntityTypeInfo(
		int e,
		int seedEntityId,
		bool findBySeed,
		eAddSelMethods addSelMethod,
		bool hasParentSeed,
		bool isSeed,
		eModes modeUsed)
	{
		if (!IsDebuggerPresent())
			return;

		Debug::debugOutput("> %s {e: %d, seed: %d, %s}, {has: %s, is: %s}, %s \t\t%s",
			findBySeed ? "bySeed" : "byBank",
			e,
			seedEntityId,
			AddSelMethods[addSelMethod],
			hasParentSeed ? "1" : "N", // "N" is more important
			isSeed ? "Y" : ".", // "Y" is more important
			ModeNames[modeUsed].shortName,
			!isSeed ? "" : "warn!");
	}
}

/// <summary>
/// <see>namespace Debug</see><br/>
/// </summary>
/// Used for debug functions and Output window handling.<br/>
namespace Debug
{
	/// Purpose: debug output to Output window using varargs
	void debugOutput(const char* fmt, ...)
	{
		if (!IsDebuggerPresent())
			return;
		char buffer[512];

		va_list args;
		va_start(args, fmt);
		vsnprintf(buffer, sizeof(buffer), fmt, args);
		va_end(args);

		strcat(buffer, "\n");
		OutputDebugStringA(buffer);
	}
}
