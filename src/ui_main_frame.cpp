#include "ui_main_frame.h"
#include "warehouse/ui_add_item_dialog.h"
#include <string>
#include <wx/filename.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>

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
    EVT_MENU(wxID_OPEN, MainFrame::OnImportCsv)
    EVT_MENU(wxID_SAVE, MainFrame::OnExportCsv)
    EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(Storage &storage)
    : wxFrame(nullptr, wxID_ANY, "Warehouse Electronics", wxDefaultPosition,
              wxSize(1000, 700)),
      storage_(storage) {
  // Create menu bar
  auto* menuBar = new wxMenuBar();
  
  // File menu
  auto* fileMenu = new wxMenu();
  fileMenu->Append(wxID_OPEN, "Import CSV\tCtrl+I", "Import items from CSV");
  fileMenu->Append(wxID_SAVE, "Export CSV\tCtrl+S", "Export items to CSV");
  fileMenu->AppendSeparator();
  fileMenu->Append(wxID_EXIT, "Exit\tAlt+X", "Exit application");
  menuBar->Append(fileMenu, "File");
  
  // Help menu
  auto* helpMenu = new wxMenu();
  helpMenu->Append(wxID_ABOUT, "About", "About this application");
  menuBar->Append(helpMenu, "Help");
  
  SetMenuBar(menuBar);
  
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
      wxAcceleratorEntry(wxACCEL_CTRL, 'I', wxID_OPEN),
      wxAcceleratorEntry(wxACCEL_CTRL, 'S', wxID_SAVE),
  };
  accelerators_ = wxAcceleratorTable(sizeof(entries) / sizeof(entries[0]),
                                     entries);
  SetAcceleratorTable(accelerators_);
}

void MainFrame::OnFocusSearch(wxCommandEvent &) {
  searchBox_->SetFocus();
  searchBox_->SelectAll();
}

void MainFrame::OnExportCsv(wxCommandEvent &) {
  wxFileDialog dlg(this, "Export to CSV", "", "warehouse_export.csv",
                   "CSV files (*.csv)|*.csv", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
  if (dlg.ShowModal() == wxID_OK) {
    try {
      storage_.exportToCsv(dlg.GetPath().ToStdString());
      wxMessageBox("Data exported successfully!", "Export", wxOK | wxICON_INFORMATION, this);
    } catch (const std::exception &ex) {
      wxMessageBox(ex.what(), "Export Error", wxOK | wxICON_ERROR, this);
    }
  }
}

void MainFrame::OnImportCsv(wxCommandEvent &) {
  wxFileDialog dlg(this, "Import from CSV", "", "",
                   "CSV files (*.csv)|*.csv", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  if (dlg.ShowModal() == wxID_OK) {
    int result = wxMessageBox(
        "Importing will add new items from the CSV file.\nContinue?",
        "Confirm Import", wxYES_NO | wxICON_QUESTION, this);
    if (result != wxYES) return;
    
    try {
      storage_.importFromCsv(dlg.GetPath().ToStdString());
      RefreshItems();
      UpdateStatsPanel();
      wxMessageBox("Data imported successfully!", "Import", wxOK | wxICON_INFORMATION, this);
    } catch (const std::exception &ex) {
      wxMessageBox(ex.what(), "Import Error", wxOK | wxICON_ERROR, this);
    }
  }
}

void MainFrame::OnAbout(wxCommandEvent &) {
  wxMessageBox(
    "Warehouse Electronics v1.1\n\n"
    "A desktop application for managing electronics inventory.\n\n"
    "Features:\n"
    "- CRUD operations with extended fields (SKU, price, supplier)\n"
    "- Search by name, category, location, SKU, supplier\n"
    "- Statistics dashboard with low stock alerts\n"
    "- CSV import/export\n"
    "- Operation log tracking\n\n"
    "Hotkeys:\n"
    "Ctrl+N - New item\n"
    "Ctrl+E - Edit item\n"
    "Ctrl+D - Delete item\n"
    "Ctrl+F - Focus search\n"
    "Ctrl+I - Import CSV\n"
    "Ctrl+S - Export CSV",
    "About", wxOK | wxICON_INFORMATION, this);
}

void MainFrame::UpdateStatsPanel() {
  try {
    auto stats = storage_.getStats(5);

    wxString statsText = wxString::Format(
        "Total items: %d | Total quantity: %d | Total value: $%.2f | Low stock alerts: %d",
        stats.totalItems, stats.totalQuantity, stats.totalValue, stats.lowStockItems);

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
      storage_.logOperation(0, item.name, OperationType::Purchase, item.quantity, "Initial stock");
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

  AddItemDialog dlg(this, &item);

  if (dlg.ShowModal() == wxID_OK) {
    Item updated;
    if (!dlg.GetItem(updated)) {
      wxMessageBox("Name is required", "Validation", wxOK | wxICON_WARNING,
                   this);
      return;
    }
    updated.id = id;

    try {
      // Log quantity change if it changed
      int qtyDiff = updated.quantity - item.quantity;
      if (qtyDiff > 0) {
        storage_.logOperation(id, updated.name, OperationType::Purchase, qtyDiff, "Stock adjustment");
      } else if (qtyDiff < 0) {
        storage_.logOperation(id, updated.name, OperationType::WriteOff, -qtyDiff, "Stock adjustment");
      }
      
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
