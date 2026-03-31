#include "warehouse/ui_add_item_dialog.h"
#include <wx/notebook.h>

AddItemDialog::AddItemDialog(wxWindow* parent, const Item* existingItem)
    : wxDialog(parent, wxID_ANY, existingItem ? "Edit item" : "Add item",
               wxDefaultPosition, wxDefaultSize,
               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    auto* panel = new wxPanel(this);
    auto* topSizer = new wxBoxSizer(wxVERTICAL);

    // Create notebook for organized tabs
    auto* notebook = new wxNotebook(panel, wxID_ANY);
    
    // Tab 1: Basic Info
    auto* basicPanel = new wxPanel(notebook);
    auto* basicGrid = new wxFlexGridSizer(5, 2, 5, 5);
    basicGrid->AddGrowableCol(1, 1);

    // Name
    basicGrid->Add(new wxStaticText(basicPanel, wxID_ANY, "Name:*"),
                  0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
    nameCtrl_ = new wxTextCtrl(basicPanel, wxID_ANY);
    basicGrid->Add(nameCtrl_, 1, wxEXPAND);

    // SKU
    basicGrid->Add(new wxStaticText(basicPanel, wxID_ANY, "SKU/Barcode:"),
                  0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
    skuCtrl_ = new wxTextCtrl(basicPanel, wxID_ANY);
    basicGrid->Add(skuCtrl_, 1, wxEXPAND);

    // Category
    basicGrid->Add(new wxStaticText(basicPanel, wxID_ANY, "Category:"),
                  0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
    categoryCtrl_ = new wxTextCtrl(basicPanel, wxID_ANY);
    basicGrid->Add(categoryCtrl_, 1, wxEXPAND);

    // Location
    basicGrid->Add(new wxStaticText(basicPanel, wxID_ANY, "Location:"),
                  0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
    locationCtrl_ = new wxTextCtrl(basicPanel, wxID_ANY);
    basicGrid->Add(locationCtrl_, 1, wxEXPAND);

    // Quantity
    basicGrid->Add(new wxStaticText(basicPanel, wxID_ANY, "Quantity:"),
                  0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
    quantityCtrl_ = new wxSpinCtrl(basicPanel, wxID_ANY);
    quantityCtrl_->SetRange(0, 1'000'000);
    quantityCtrl_->SetValue(1);
    basicGrid->Add(quantityCtrl_, 1, wxEXPAND);

    basicPanel->SetSizer(basicGrid);
    notebook->AddPage(basicPanel, "Basic Info");

    // Tab 2: Pricing & Supplier
    auto* pricingPanel = new wxPanel(notebook);
    auto* pricingGrid = new wxFlexGridSizer(4, 2, 5, 5);
    pricingGrid->AddGrowableCol(1, 1);

    // Price
    pricingGrid->Add(new wxStaticText(pricingPanel, wxID_ANY, "Price (cost):"),
                    0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
    priceCtrl_ = new wxTextCtrl(pricingPanel, wxID_ANY, "0.00");
    pricingGrid->Add(priceCtrl_, 1, wxEXPAND);

    // Supplier
    pricingGrid->Add(new wxStaticText(pricingPanel, wxID_ANY, "Supplier:"),
                    0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
    supplierCtrl_ = new wxTextCtrl(pricingPanel, wxID_ANY);
    pricingGrid->Add(supplierCtrl_, 1, wxEXPAND);

    // Min Quantity
    pricingGrid->Add(new wxStaticText(pricingPanel, wxID_ANY, "Min Quantity:"),
                    0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
    minQuantityCtrl_ = new wxSpinCtrl(pricingPanel, wxID_ANY);
    minQuantityCtrl_->SetRange(0, 10000);
    minQuantityCtrl_->SetValue(5);
    pricingGrid->Add(minQuantityCtrl_, 1, wxEXPAND);

    pricingPanel->SetSizer(pricingGrid);
    notebook->AddPage(pricingPanel, "Pricing");

    // Tab 3: Description
    auto* descPanel = new wxPanel(notebook);
    auto* descSizer = new wxBoxSizer(wxVERTICAL);
    descriptionCtrl_ = new wxTextCtrl(descPanel, wxID_ANY, "",
                                      wxDefaultPosition, wxDefaultSize,
                                      wxTE_MULTILINE | wxTE_WORDWRAP);
    descSizer->Add(descriptionCtrl_, 1, wxEXPAND | wxALL, 5);
    descPanel->SetSizer(descSizer);
    notebook->AddPage(descPanel, "Description");

    topSizer->Add(notebook, 1, wxEXPAND | wxALL, 10);

    // Buttons
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

    // Set initial size
    SetSize(500, 400);

    // Load existing item data if provided
    if (existingItem) {
        SetItem(*existingItem);
    }
}

void AddItemDialog::SetItem(const Item& item) {
    nameCtrl_->SetValue(item.name);
    skuCtrl_->SetValue(item.sku);
    categoryCtrl_->SetValue(item.category);
    locationCtrl_->SetValue(item.location);
    quantityCtrl_->SetValue(item.quantity);
    priceCtrl_->SetValue(wxString::Format("%.2f", item.price));
    supplierCtrl_->SetValue(item.supplier);
    minQuantityCtrl_->SetValue(item.minQuantity);
    descriptionCtrl_->SetValue(item.description);
}

bool AddItemDialog::GetItem(Item& out) const {
    wxString name = nameCtrl_->GetValue().Trim().Trim(false);
    if (name.IsEmpty()) {
        return false;
    }

    out.id       = 0;
    out.name     = name.ToStdString();
    out.sku      = skuCtrl_->GetValue().ToStdString();
    out.category = categoryCtrl_->GetValue().ToStdString();
    out.location = locationCtrl_->GetValue().ToStdString();
    out.quantity = quantityCtrl_->GetValue();
    
    // Parse price
    wxString priceStr = priceCtrl_->GetValue();
    double price = 0.0;
    if (priceStr.ToDouble(&price)) {
        out.price = price;
    } else {
        out.price = 0.0;
    }
    
    out.supplier   = supplierCtrl_->GetValue().ToStdString();
    out.minQuantity = minQuantityCtrl_->GetValue();
    out.description = descriptionCtrl_->GetValue().ToStdString();
    
    return true;
}
