#pragma once

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include "warehouse/models.h"

class AddItemDialog : public wxDialog {
public:
    explicit AddItemDialog(wxWindow* parent);

    bool GetItem(Item& out) const;

private:
    wxTextCtrl* nameCtrl_      = nullptr;
    wxTextCtrl* categoryCtrl_  = nullptr;
    wxTextCtrl* locationCtrl_  = nullptr;
    wxSpinCtrl* quantityCtrl_  = nullptr;
};
