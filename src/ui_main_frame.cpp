#include "ui_main_frame.h"
#include "warehouse/ui_add_item_dialog.h"
#include <string>

enum {
  ID_AddItem = wxID_HIGHEST + 1,
  ID_EditItem,
  ID_DeleteItem,
  ID_SearchBox,
  ID_Accel_New,
  ID_Accel_Edit,
  ID_Accel_Delete,
  ID_Accel_Search
};

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_BUTTON(ID_AddItem, MainFrame::OnAddItem)
    EVT_BUTTON(ID_EditItem, MainFrame::OnEditItem)
    EVT_BUTTON(ID_DeleteItem, MainFrame::OnDeleteItem)
    EVT_TEXT(ID_SearchBox, MainFrame::OnSearch)
    EVT_DATAVIEW_ITEM_ACTIVATED(wxID_ANY, MainFrame::OnItemActivated)
    EVT_MENU(ID_Accel_New, MainFrame::OnAddItem)
    EVT_MENU(ID_Accel_Edit, MainFrame::OnEditItem)
    EVT_MENU(ID_Accel_Delete, MainFrame::OnDeleteItem)
    EVT_MENU(ID_Accel_Search, MainFrame::OnFocusSearch)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(Storage &storage)
    : wxFrame(nullptr, wxID_ANY, "Warehouse Electronics", wxDefaultPosition,
              wxSize(900, 600)),
      storage_(storage) {
  auto *panel = new wxPanel(this);
  auto *mainSizer = new wxBoxSizer(wxVERTICAL);

  // Stats panel at top
  auto *statsPanel = new wxPanel(panel);
  auto *statsSizer = new wxBoxSizer(wxHORIZONTAL);
  statsLabel_ = new wxStaticText(statsPanel, wxID_ANY, "");
  statsLabel_->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
                              wxFONTWEIGHT_BOLD));
  statsSizer->Add(statsLabel_, 0, wxALL, 5);
  statsPanel->SetSizer(statsSizer);

  searchBox_ = new wxTextCtrl(panel, ID_SearchBox, "");
  searchBox_->SetHint("Search by name, category, or location...");
  list_ =
      new wxDataViewListCtrl(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize);

  list_->AppendTextColumn("ID", wxDATAVIEW_CELL_INERT, 60);
  list_->AppendTextColumn("Name", wxDATAVIEW_CELL_INERT, 200);
  list_->AppendTextColumn("Category", wxDATAVIEW_CELL_INERT, 120);
  list_->AppendTextColumn("Location", wxDATAVIEW_CELL_INERT, 120);
  list_->AppendTextColumn("Quantity", wxDATAVIEW_CELL_INERT, 80);

  auto *buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
  auto *addBtn = new wxButton(panel, ID_AddItem, "Add");
  auto *editBtn = new wxButton(panel, ID_EditItem, "Edit");
  auto *deleteBtn = new wxButton(panel, ID_DeleteItem, "Delete");

  buttonsSizer->Add(addBtn, 0, wxALL, 5);
  buttonsSizer->Add(editBtn, 0, wxALL, 5);
  buttonsSizer->Add(deleteBtn, 0, wxALL, 5);

  mainSizer->Add(statsPanel, 0, wxEXPAND | wxALL, 0);
  mainSizer->Add(searchBox_, 0, wxEXPAND | wxALL, 5);
  mainSizer->Add(list_, 1, wxEXPAND | wxALL, 5);
  mainSizer->Add(buttonsSizer, 0, wxALIGN_LEFT | wxALL, 5);

  panel->SetSizer(mainSizer);

  SetupAccelerators();

  RefreshItems();
  UpdateStatsPanel();
}

void MainFrame::SetupAccelerators() {
  wxAcceleratorEntry entries[] = {
      wxAcceleratorEntry(wxACCEL_CTRL, 'N', ID_Accel_New),
      wxAcceleratorEntry(wxACCEL_CTRL, 'E', ID_Accel_Edit),
      wxAcceleratorEntry(wxACCEL_CTRL, 'D', ID_Accel_Delete),
      wxAcceleratorEntry(wxACCEL_CTRL, 'F', ID_Accel_Search),
  };
  accelerators_ = wxAcceleratorTable(sizeof(entries) / sizeof(entries[0]),
                                     entries);
  SetAcceleratorTable(accelerators_);
}

void MainFrame::OnFocusSearch(wxCommandEvent &) {
  searchBox_->SetFocus();
  searchBox_->SelectAll();
}

void MainFrame::UpdateStatsPanel() {
  try {
    auto stats = storage_.getStats(5);

    wxString statsText = wxString::Format(
        "Total items: %d | Total quantity: %d | Low stock alerts: %d",
        stats.totalItems, stats.totalQuantity, stats.lowStockItems);

    if (!stats.itemsByCategory.empty()) {
      statsText += " | Categories: ";
      bool first = true;
      for (const auto &[category, count] : stats.itemsByCategory) {
        if (!first)
          statsText += ", ";
        statsText += wxString::Format("%s (%d)", category, count);
        first = false;
      }
    }

    statsLabel_->SetLabel(statsText);

    // Highlight low stock warning
    if (stats.lowStockItems > 0) {
      statsLabel_->SetForegroundColour(wxColour(200, 50, 50));
    } else {
      statsLabel_->SetForegroundColour(wxColour(50, 50, 50));
    }
    statsLabel_->Refresh();

  } catch (const std::exception &ex) {
    statsLabel_->SetLabel("Error loading stats");
  }
}

void MainFrame::OnAddItem(wxCommandEvent &) {
  AddItemDialog dlg(this);
  if (dlg.ShowModal() == wxID_OK) {
    Item item;
    if (!dlg.GetItem(item)) {
      wxMessageBox("Name is required", "Validation", wxOK | wxICON_WARNING,
                   this);
      return;
    }

    try {
      storage_.addItem(item);
      RefreshItems();
      UpdateStatsPanel();
    } catch (const std::exception &ex) {
      wxMessageBox(ex.what(), "DB error", wxOK | wxICON_ERROR, this);
    }
  }
}

void MainFrame::OnEditItem(wxCommandEvent &) {
  auto idOpt = GetSelectedItemId();
  if (!idOpt) {
    wxMessageBox("Select an item first", "Edit", wxOK | wxICON_INFORMATION,
                 this);
    return;
  }
  int id = *idOpt;

  Item item;
  if (!storage_.loadById(id, item)) {
    wxMessageBox("Item not found", "Edit", wxOK | wxICON_ERROR, this);
    return;
  }

  AddItemDialog dlg(this);

  dlg.SetName(item.name);
  dlg.SetCategory(item.category);
  dlg.SetLocation(item.location);
  dlg.SetQuantity(item.quantity);

  if (dlg.ShowModal() == wxID_OK) {
    Item updated;
    if (!dlg.GetItem(updated)) {
      wxMessageBox("Name is required", "Validation", wxOK | wxICON_WARNING,
                   this);
      return;
    }
    updated.id = id;

    try {
      storage_.updateItem(updated);
      RefreshItems();
      UpdateStatsPanel();
    } catch (const std::exception &ex) {
      wxMessageBox(ex.what(), "DB error", wxOK | wxICON_ERROR, this);
    }
  }
}

void MainFrame::OnDeleteItem(wxCommandEvent &) {
  auto idOpt = GetSelectedItemId();
  if (!idOpt) {
    wxMessageBox("Select an item first", "Delete", wxOK | wxICON_INFORMATION,
                 this);
    return;
  }
  int id = *idOpt;

  int result = wxMessageBox("Are you sure you want to delete this item?",
                            "Confirm Delete", wxYES_NO | wxICON_QUESTION, this);
  if (result != wxYES) {
    return;
  }

  try {
    storage_.deleteItem(id);
    RefreshItems();
    UpdateStatsPanel();
  } catch (const std::exception &ex) {
    wxMessageBox(ex.what(), "DB error", wxOK | wxICON_ERROR, this);
  }
}

void MainFrame::OnItemActivated(wxDataViewEvent &evt) {
  wxCommandEvent cmd;
  OnEditItem(cmd);
}

void MainFrame::OnSearch(wxCommandEvent &) {
  RefreshItems(searchBox_->GetValue().ToStdString());
}

void MainFrame::RefreshItems() { RefreshItems(""); }

void MainFrame::RefreshItems(const std::string &filter) {
  list_->DeleteAllItems();

  auto items = filter.empty() ? storage_.loadAll() : storage_.search(filter);
  for (const auto &it : items) {
    wxVector<wxVariant> row;
    row.push_back(wxVariant(wxString::Format("%d", it.id)));
    row.push_back(wxVariant(it.name));
    row.push_back(wxVariant(it.category));
    row.push_back(wxVariant(it.location));
    row.push_back(wxVariant(wxString::Format("%d", it.quantity)));
    list_->AppendItem(row);
  }
}

std::optional<int> MainFrame::GetSelectedItemId() const {
  wxDataViewItem sel = list_->GetSelection();
  if (!sel.IsOk()) {
    return std::nullopt;
  }

  wxVariant value;
  list_->GetValue(value, list_->ItemToRow(sel), 0);

  long id = 0;
  if (!value.GetString().ToLong(&id)) {
    return std::nullopt;
  }

  return static_cast<int>(id);
}
