#include "warehouse/ui_add_item_dialog.h"

AddItemDialog::AddItemDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, "Add item",
               wxDefaultPosition, wxDefaultSize,
               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    auto* panel = new wxPanel(this);
    auto* topSizer = new wxBoxSizer(wxVERTICAL);

    auto* grid = new wxFlexGridSizer(4, 2, 5, 5);
    grid->AddGrowableCol(1, 1);

    // Name
    grid->Add(new wxStaticText(panel, wxID_ANY, "Name:"),
              0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
    nameCtrl_ = new wxTextCtrl(panel, wxID_ANY);
    grid->Add(nameCtrl_, 1, wxEXPAND);

    // Category
    grid->Add(new wxStaticText(panel, wxID_ANY, "Category:"),
              0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
    categoryCtrl_ = new wxTextCtrl(panel, wxID_ANY);
    grid->Add(categoryCtrl_, 1, wxEXPAND);

    // Location
    grid->Add(new wxStaticText(panel, wxID_ANY, "Location:"),
              0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
    locationCtrl_ = new wxTextCtrl(panel, wxID_ANY);
    grid->Add(locationCtrl_, 1, wxEXPAND);

    // Quantity
    grid->Add(new wxStaticText(panel, wxID_ANY, "Quantity:"),
              0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
    quantityCtrl_ = new wxSpinCtrl(panel, wxID_ANY);
    quantityCtrl_->SetRange(0, 1'000'000);
    quantityCtrl_->SetValue(1);
    grid->Add(quantityCtrl_, 1, wxEXPAND);

    topSizer->Add(grid, 1, wxEXPAND | wxALL, 10);

    // Кнопки: делаем их тоже дочерними panel
    auto* btnSizer = new wxStdDialogButtonSizer();
    auto* okBtn = new wxButton(panel, wxID_OK);
    auto* cancelBtn = new wxButton(panel, wxID_CANCEL);
    btnSizer->AddButton(okBtn);
    btnSizer->AddButton(cancelBtn);
    btnSizer->Realize();

    topSizer->Add(btnSizer, 0, wxALIGN_RIGHT | wxALL, 10);

    panel->SetSizer(topSizer);
    topSizer->Fit(panel);
    topSizer->SetSizeHints(panel);

    auto* dlgSizer = new wxBoxSizer(wxVERTICAL);
    dlgSizer->Add(panel, 1, wxEXPAND);
    SetSizerAndFit(dlgSizer);
    CentreOnParent();
}

bool AddItemDialog::GetItem(Item& out) const {
    wxString name = nameCtrl_->GetValue().Trim().Trim(false);
    if (name.IsEmpty()) {
        return false;
    }

    out.id       = 0;
    out.name     = name.ToStdString();
    out.category = categoryCtrl_->GetValue().ToStdString();
    out.location = locationCtrl_->GetValue().ToStdString();
    out.quantity = quantityCtrl_->GetValue();
    return true;
}
