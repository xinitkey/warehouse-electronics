#pragma once

#include <memory>
#include <optional>

#include <wx/dataview.h>
#include <wx/wx.h>

#include "warehouse/storage.h"

class MainFrame : public wxFrame {
public:
  explicit MainFrame(Storage &storage);

private:
  void RefreshItems();

  void OnAddItem(wxCommandEvent &evt);
  void OnEditItem(wxCommandEvent &evt);
  void OnItemActivated(wxDataViewEvent &evt);

  std::optional<int> GetSelectedItemId() const;

  Storage &storage_;
  wxDataViewListCtrl *list_ = nullptr;
  wxTextCtrl *searchBox_ = nullptr;

  wxDECLARE_EVENT_TABLE();
};
