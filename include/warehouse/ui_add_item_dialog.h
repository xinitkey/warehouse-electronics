#pragma once

#include "warehouse/models.h"
#include <wx/spinctrl.h>
#include <wx/wx.h>

class AddItemDialog : public wxDialog {
public:
  AddItemDialog(wxWindow *parent, const Item *existingItem = nullptr);

  bool GetItem(Item &out) const;

  void SetItem(const Item &item);

private:
  wxTextCtrl *nameCtrl_ = nullptr;
  wxTextCtrl *categoryCtrl_ = nullptr;
  wxTextCtrl *locationCtrl_ = nullptr;
  wxSpinCtrl *quantityCtrl_ = nullptr;
  wxTextCtrl *skuCtrl_ = nullptr;
  wxTextCtrl *priceCtrl_ = nullptr;
  wxTextCtrl *supplierCtrl_ = nullptr;
  wxSpinCtrl *minQuantityCtrl_ = nullptr;
  wxTextCtrl *descriptionCtrl_ = nullptr;
};
