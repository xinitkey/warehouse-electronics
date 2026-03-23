#include "ui_main_frame.h"
#include "warehouse/ui_add_item_dialog.h"


enum {
    ID_AddItem = wxID_HIGHEST + 1
};

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_BUTTON(ID_AddItem, MainFrame::OnAddItem)
wxEND_EVENT_TABLE()


MainFrame::MainFrame(Storage& storage)
    : wxFrame(nullptr, wxID_ANY, "Warehouse Electronics",
              wxDefaultPosition, wxSize(900, 600))
    , storage_(storage)
{
    auto* panel = new wxPanel(this);
    auto* mainSizer = new wxBoxSizer(wxVERTICAL);

    searchBox_ = new wxTextCtrl(panel, wxID_ANY, "");
    list_ = new wxDataViewListCtrl(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize);

    list_->AppendTextColumn("ID",        wxDATAVIEW_CELL_INERT, 60);
    list_->AppendTextColumn("Name",      wxDATAVIEW_CELL_INERT, 200);
    list_->AppendTextColumn("Category",  wxDATAVIEW_CELL_INERT, 120);
    list_->AppendTextColumn("Location",  wxDATAVIEW_CELL_INERT, 120);
    list_->AppendTextColumn("Quantity",  wxDATAVIEW_CELL_INERT, 80);

    auto* buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
    auto* addBtn = new wxButton(panel, ID_AddItem, "Add");
    buttonsSizer->Add(addBtn, 0, wxALL, 5);

    mainSizer->Add(searchBox_, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(list_,      1, wxEXPAND | wxALL, 5); 
    mainSizer->Add(buttonsSizer, 0, wxALIGN_LEFT | wxALL, 5);

    panel->SetSizer(mainSizer);

    RefreshItems();
}

void MainFrame::OnAddItem(wxCommandEvent&) {
    AddItemDialog dlg(this);
    if (dlg.ShowModal() == wxID_OK) {
        Item item;
        if (!dlg.GetItem(item)) {
            wxMessageBox("Name is required", "Validation", wxOK | wxICON_WARNING, this);
            return;
        }

        try {
            storage_.addItem(item);
            RefreshItems();
        } catch (const std::exception& ex) {
            wxMessageBox(ex.what(), "DB error", wxOK | wxICON_ERROR, this);
        }
    }
}

void MainFrame::RefreshItems() {
    list_->DeleteAllItems();

    auto items = storage_.loadAll();
    for (const auto& it : items) {
        wxVector<wxVariant> row;
        row.push_back(wxVariant(wxString::Format("%d", it.id)));
        row.push_back(wxVariant(it.name));
        row.push_back(wxVariant(it.category));
        row.push_back(wxVariant(it.location));
        row.push_back(wxVariant(wxString::Format("%d", it.quantity)));
        list_->AppendItem(row);
    }
}
