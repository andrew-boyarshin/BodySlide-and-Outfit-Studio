/*
BodySlide and Outfit Studio
Copyright(C) 2017  Caliente & ousnius

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "PreviewWindow.h"
#include "OutfitStudio.h"
#include "PresetSaveDialog.h"
#include "GroupManager.h"
#include "../components/SliderData.h"
#include "../components/SliderManager.h"
#include "../components/SliderGroup.h"
#include "../components/SliderCategories.h"
#include "../files/TriFile.h"
#include "../utils/Log.h"

#include "FSEngine/FSManager.h"
#include "FSEngine/FSEngine.h"

#include <wx/wxprec.h>
#include <wx/srchctrl.h>
#include <wx/xrc/xmlres.h>
#include <wx/imagpng.h>
#include <wx/dcbuffer.h>
#include <wx/statline.h>
#include <wx/tokenzr.h>
#include <wx/cmdline.h>
#include <wx/html/htmlwin.h>
#include <wx/listctrl.h>
#include <wx/dir.h>
#include <wx/progdlg.h>
#include <wx/intl.h>
#include <wx/clrpicker.h>


class BodySlideFrame;

class BodySlideApp : public wxApp {
	/* UI Managers */
	BodySlideFrame* sliderView = nullptr;
	PreviewWindow* preview = nullptr;
	OutfitStudio* outfitStudio = nullptr;

	/* Command-Line Arguments */
	bool cmdOutfitStudio = false;
	wxString cmdGroupBuild;
	wxString cmdTargetDir;
	wxString cmdPreset;
	bool cmdTri = false;

	/* Localization */
	wxLocale* locale = nullptr;
	int language;

	/* Data Managers */
	SliderManager sliderManager;
	DiffDataSets dataSets;
	SliderSet activeSet;
	Log logger;

	/* Data Items */
	map<string, string> outfitNameSource;		// All currently defined outfits.
	vector<string> outfitNameOrder;				// All currently defined outfits, in their order of appearance.
	map<string, vector<string>> groupMembers;	// All currently defined groups.
	map<string, string> groupAlias;				// Group name aliases.
	vector<string> ungroupedOutfits;			// Outfits without a group.
	vector<string> filteredOutfits;				// Filtered outfit names.
	vector<string> presetGroups;
	vector<string> allGroups;
	SliderSetGroupCollection gCollection;

	map<string, vector<string>> outFileCount;	// Counts how many sets write to the same output file

	string previewBaseName;
	string previewSetName;
	NifFile* previewBaseNif = nullptr;
	NifFile PreviewMod;

	int CreateSetSliders(const string& outfit);

public:
	virtual ~BodySlideApp();
	virtual bool OnInit();
	virtual void OnInitCmdLine(wxCmdLineParser& parser);
	virtual bool OnCmdLineParsed(wxCmdLineParser& parser);

	virtual bool OnExceptionInMainLoop();
	virtual void OnUnhandledException();
	virtual void OnFatalException();
	
	SliderCategoryCollection cCollection;
	TargetGame targetGame;

	bool SetDefaultConfig();
	bool ShowSetup();
	wxString GetGameDataPath(TargetGame targ);

	void InitLanguage();

	void InitArchives();
	void GetArchiveFiles(vector<string>& outList);

	void LoadData();
	void CharHook(wxKeyEvent& event) {
		wxWindow* w = (wxWindow*)event.GetEventObject();
		if (!w) {
			event.Skip();
			return;
		}

		wxString nm = w->GetName();
		if (event.GetKeyCode() == wxKeyCode::WXK_F5) {
			if (nm == "outfitChoice") {
				RefreshOutfitList();
			}
			event.Skip();
			return;
		}

		string stupidkeys = "0123456789-";
		bool stupidHack = false;
		if (event.GetKeyCode() < 256 && stupidkeys.find(event.GetKeyCode()) != string::npos)
			stupidHack = true;

		if (stupidHack && nm.EndsWith("|readout")) {
			wxTextCtrl* e = (wxTextCtrl*)w;
			HWND hwndEdit = e->GetHandle();
			::SendMessage(hwndEdit, WM_CHAR, event.GetKeyCode(), event.GetRawKeyFlags());
		}
		else {
			event.Skip();
		}
	}

	void LoadAllCategories();

	void SetPresetGroups(const string& setName);
	void LoadAllGroups();
	void GetAllGroupNames(vector<string>& outGroups);
	int SaveGroupList(const string& filename, const string& groupname);

	void ApplyOutfitFilter();
	int GetOutfits(vector<string>& outList);
	int GetFilteredOutfits(vector<string>& outList);

	void LoadPresets(const string& sliderSet);
	void PopulatePresetList(const string& select);
	void PopulateOutfitList(const string& select);
	void DisplayActiveSet();

	int LoadSliderSets();
	void RefreshOutfitList();

	void RefreshSliders();

	void ActivateOutfit(const string& outfitName);
	void ActivatePreset(const string& presetName, const bool updatePreview = true);

	void LaunchOutfitStudio();

	void ApplySliders(const string& targetShape, vector<Slider>& sliderSet, vector<Vector3>& verts, vector<ushort>& zapidx, vector<Vector2>* uvs = nullptr);
	int WriteMorphTRI(const string& triPath, SliderSet& sliderSet, NifFile& nif, unordered_map<string, vector<ushort>>& zapIndices);

	void CopySliderValues(bool toHigh);
	void ShowPreview();
	void InitPreview();
	void CleanupPreview();
	void ClosePreview() {
		// Calling Close() will cause PreviewClosed() to be called,
		// where we reset the preview window pointer to null
		if (preview)
			preview->Close();
	}
	void PreviewClosed() {
		preview = nullptr;
	}

	void CloseOutfitStudio(bool force = false) {
		if (force && outfitStudio)
			outfitStudio->Close();

		outfitStudio = nullptr;
	}

	bool IsOutfitStudioOpened() {
		return outfitStudio != nullptr;
	}

	void UpdatePreview();
	void RebuildPreviewMeshes();

	int BuildBodies(bool localPath = false, bool clean = false, bool tri = false);
	int BuildListBodies(vector<string>& outfitList, map<string, string>& failedOutfits, bool remove = false, bool tri = false, const string& custPath = "");
	void GroupBuild(const string& group);

	float GetSliderValue(const wxString& sliderName, bool isLo);
	bool IsUVSlider(const wxString& sliderName);
	vector<string> GetSliderZapToggles(const wxString& sliderName);
	void SetSliderValue(const wxString& sliderName, bool isLo, float val);
	void SetSliderChanged(const wxString& sliderName, bool isLo);

	int UpdateSliderPositions(const string& presetName);
	int SaveSliderPositions(const string& outputFile, const string& presetName, vector<string>& groups);
};

static const wxCmdLineEntryDesc g_cmdLineDesc[] = {
	{ wxCMD_LINE_SWITCH, "os", "outfitstudio", "launches straight into outfit studio" },
	{ wxCMD_LINE_OPTION, "gbuild", "groupbuild", "builds the specified group on launch", wxCMD_LINE_VAL_STRING },
	{ wxCMD_LINE_OPTION, "t", "targetdir", "build target directory, defaults to game data path", wxCMD_LINE_VAL_STRING },
	{ wxCMD_LINE_OPTION, "p", "preset", "preset used for the build, defaults to last used preset", wxCMD_LINE_VAL_STRING },
	{ wxCMD_LINE_SWITCH, "tri", "trimorphs", "enables tri morph output for the specified build" },
	{ wxCMD_LINE_NONE }
};

#define DELAYLOAD_TIMER 299
#define SLIDER_LO 1
#define SLIDER_HI 2

class BodySlideFrame : public wxFrame {
public:
	class SliderDisplay {
	public:
		bool isZap;
		bool oneSize;
		string sliderName;
		wxStaticText* lblSliderLo;
		wxSlider* sliderLo;
		wxTextCtrl* sliderReadoutLo;
		wxStaticText* lblSliderHi;
		wxSlider* sliderHi;
		wxTextCtrl* sliderReadoutHi;
		wxCheckBox* zapCheckHi;
		wxCheckBox* zapCheckLo;
		SliderDisplay() {
			isZap = false;
			oneSize = false;
			lblSliderLo = nullptr;
			sliderLo = nullptr;
			sliderReadoutLo = nullptr;
			lblSliderLo = nullptr;
			lblSliderHi = nullptr;
			sliderHi = nullptr;
			sliderReadoutLo = nullptr;
			lblSliderLo = nullptr;
			zapCheckHi = nullptr;
			zapCheckLo = nullptr;
		}
	};

	unordered_map<string, SliderDisplay*> sliderDisplays;

	wxTimer delayLoad;
	wxSearchCtrl* search;
	wxSearchCtrl* outfitsearch;
	wxCheckListBox* batchBuildList;

	BodySlideFrame(BodySlideApp* app, const wxSize& size);
	~BodySlideFrame() {
		ClearSliderGUI();
	}

	void ShowLowColumn(bool show);
	void AddCategorySliderUI(const wxString& name, bool show, bool oneSize);
	void AddSliderGUI(const wxString& name, const wxString& displayName, bool isZap, bool oneSize = false);

	BodySlideFrame::SliderDisplay* GetSliderDisplay(const string& name) {
		if (sliderDisplays.find(name) != sliderDisplays.end())
			return sliderDisplays[name];

		return nullptr;
	}

	void ClearPresetList();
	void ClearOutfitList();
	void ClearSliderGUI();

	void PopulateOutfitList(const wxArrayString& items, const wxString& selectItem);
	void PopulatePresetList(const wxArrayString& items, const wxString& selectItem);

	void SetSliderPosition(const wxString& name, float newValue, short HiLo);

	int lastScroll;
	int rowCount;

private:
	void OnExit(wxCommandEvent& event);
	void OnClose(wxCloseEvent& event);
	void OnActivateFrame(wxActivateEvent& event);
	void OnIconizeFrame(wxIconizeEvent& event);
	void PostIconizeFrame();

	void OnLinkClicked(wxHtmlLinkEvent& link);
	void OnEnterClose(wxKeyEvent& event);

	void OnEnterSliderWindow(wxMouseEvent& event);
	void OnSliderChange(wxScrollEvent& event);
	void OnSliderReadoutChange(wxCommandEvent& event);
	void OnSearchChange(wxCommandEvent& event);
	void OnOutfitSearchChange(wxCommandEvent& event);

	void OnZapCheckChanged(wxCommandEvent& event);
	void OnCategoryCheckChanged(wxCommandEvent& event);

	void OnEraseBackground(wxEraseEvent& event);

	void OnDelayLoad(wxTimerEvent& event);

	void OnChooseGroups(wxCommandEvent& event);
	void OnRefreshGroups(wxCommandEvent& event);
	void OnSaveGroups(wxCommandEvent& event);
	void OnRefreshOutfits(wxCommandEvent& event);

	void OnChooseOutfit(wxCommandEvent& event);
	void OnChoosePreset(wxCommandEvent& event);

	void OnSavePreset(wxCommandEvent& event);
	void OnSavePresetAs(wxCommandEvent& event);
	void OnGroupManager(wxCommandEvent& event);

	void OnPreview(wxCommandEvent& event);
	void OnHighToLow(wxCommandEvent& event);
	void OnLowToHigh(wxCommandEvent& event);
	void OnBuildBodies(wxCommandEvent& event);
	void OnBatchBuild(wxCommandEvent& event);
	void OnBatchBuildContext(wxMouseEvent& event);
	void OnBatchBuildSelect(wxCommandEvent& event);
	void OnOutfitStudio(wxCommandEvent& event);
	void SettingsFillDataFiles(wxCheckListBox* dataFileList, wxString& dataDir, int targetGame);
	void OnSettings(wxCommandEvent& event);
	void OnChooseTargetGame(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnMoveWindow(wxMoveEvent& event);
	void OnSetSize(wxSizeEvent& event);

	bool OutfitIsEmpty() {
		wxChoice* outfitChoice = (wxChoice*)FindWindowByName("outfitChoice");
		if (outfitChoice && !outfitChoice->GetStringSelection().empty())
			return false;

		return true;
	}

	BodySlideApp* app;

	wxDECLARE_EVENT_TABLE();
};
