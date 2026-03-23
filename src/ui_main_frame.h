#pragma once
#include <wx/wx.h>
#include <wx/dataview.h>
#include "warehouse/storage.h"

class MainFrame : public wxFrame {
public:
    explicit MainFrame(Storage& storage);

private:
    void RefreshItems();
    void OnAddItem(wxCommandEvent& evt);

    Storage& storage_;
    wxDataViewListCtrl* list_ = nullptr;
    wxTextCtrl* searchBox_ = nullptr;

    wxDECLARE_EVENT_TABLE();
};
