#pragma once

#include "warehouse/models.h"
#include <wx/spinctrl.h>
#include <wx/wx.h>

class AddItemDialog : public wxDialog {
public:
  explicit AddItemDialog(wxWindow *parent);

  bool GetItem(Item &out) const;

  void SetName(const std::string &v) { nameCtrl_->SetValue(v); }
  void SetCategory(const std::string &v) { categoryCtrl_->SetValue(v); }
  void SetLocation(const std::string &v) { locationCtrl_->SetValue(v); }
  void SetQuantity(int q) { quantityCtrl_->SetValue(q); }

private:
  wxTextCtrl *nameCtrl_ = nullptr;
  wxTextCtrl *categoryCtrl_ = nullptr;
  wxTextCtrl *locationCtrl_ = nullptr;
  wxSpinCtrl *quantityCtrl_ = nullptr;
};
