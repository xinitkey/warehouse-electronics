#pragma once
#include <wx/wx.h>
#include "warehouse/models.h"

class AddItemDialog : public wxDialog {
public:
    AddItemDialog(wxWindow* parent, const Item* existingItem = nullptr);

    bool GetItem(Item& out) const;
    
    void SetItem(const Item& item);

private:
    wxTextCtrl* nameCtrl_;
    wxTextCtrl* categoryCtrl_;
    wxTextCtrl* locationCtrl_;
    wxSpinCtrl* quantityCtrl_;
    wxTextCtrl* skuCtrl_;
    wxTextCtrl* priceCtrl_;
    wxTextCtrl* supplierCtrl_;
    wxSpinCtrl* minQuantityCtrl_;
    wxTextCtrl* descriptionCtrl_;
};
