#pragma once

#include <memory>
#include <optional>
#include <string>

#include <wx/dataview.h>
#include <wx/wx.h>

#include "warehouse/storage.h"

class MainFrame : public wxFrame {
public:
  explicit MainFrame(Storage &storage);

  void OnAddItem(wxCommandEvent &evt);
  void OnEditItem(wxCommandEvent &evt);
  void OnDeleteItem(wxCommandEvent &evt);

private:
  void RefreshItems();
  void RefreshItems(const std::string &filter);
  void SetupAccelerators();

  void OnItemActivated(wxDataViewEvent &evt);
  void OnSearch(wxCommandEvent &evt);
  void OnFocusSearch(wxCommandEvent &evt);
  void UpdateStatsPanel();

  std::optional<int> GetSelectedItemId() const;

  Storage &storage_;
  wxDataViewListCtrl *list_ = nullptr;
  wxTextCtrl *searchBox_ = nullptr;
  wxStaticText *statsLabel_ = nullptr;
  wxAcceleratorTable accelerators_;

  wxDECLARE_EVENT_TABLE();
};
