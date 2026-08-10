#pragma once

#include <vector>
#include <chrono>
#include <array>

// Partial definition
namespace SelTools // SelectionTools
{
	enum eTabs : int
	{
		None = 0,
		Selected,
		Groups,
		Modes,
		Count
	};

	// public:
	bool knownActiveTab_is(eTabs tab);
	void setKnownActiveTab(eTabs tab);
	bool nextTabToShow_is(eTabs tab);
	void setNextTabToShow(eTabs tab);
	void resetNextTabToShow();
}

// Partial definition
namespace SelModes // SelectionModes
{
	enum eModes : int
	{
		Normal = 0,
		SameType,
		AdjacentTypes,
		NearbyLateral,
		Radius,
		Count,
		CurrMode
	};
	enum eAddSelMethods : int
	{
		viaRubberBand = 0,
		viaCtrlClick,
		viaScan,
		Count_Methods
	};

	// public:
	void init();
	eModes getMode();
	bool mode_isNormal();
	void switchMode(eModes newMode, bool useNextMode = false);
	void addEntityType(
		int e,
		int seedEntityId = 0,
		eAddSelMethods addSelMethod = viaScan,
		eModes modeUsed = eModes::CurrMode);
	void addEntityTypeAndScan_viaCtrlClick(int e, int seedEntityId, SelModes::eModes modeUsed);
	void addEntityType_viaRubberBand(int e, int seedEntityId, SelModes::eModes modeUsed);
	void clearAllowedTypesAndSelections();
	void setSelVarsInRubberBandEntry(int e, int seedEntityId, eModes modeUsed);
	bool allowedTypesIsEmpty();
	bool entityTypeIsAllowed(int e, eAddSelMethods addSelMethod);

	// Partial definition
	namespace Match
	{
		// public:
		void countRubberBandTypes();
	}
	
	// Partial definition
	namespace SelModesWindow
	{
		constexpr char* winName = " Selection Modes ";

		// public:
		bool drawWindow(int tabflags);
	}
}

// TODO: move/merge elsewhere with existing debug functions.
namespace Debug
{
	// public:
	void debugOutput(const char* fmt, ...);
}
