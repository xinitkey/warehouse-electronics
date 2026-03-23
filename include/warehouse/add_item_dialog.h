#pragma once
#include <wx/wx.h>
#include "warehouse/models.h"

class AddItemDialog : public wxDialog {
public:
    AddItemDialog(wxWindow* parent);

    bool GetItem(Item& out) const;

private:
    wxTextCtrl* nameCtrl_;
    wxTextCtrl* categoryCtrl_;
    wxTextCtrl* locationCtrl_;
    wxSpinCtrl* quantityCtrl_;
};
