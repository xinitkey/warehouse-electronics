#include <wx/wx.h>
#include "ui_main_frame.h"
#include "warehouse/db.h"
#include "warehouse/storage.h"

class WarehouseApp : public wxApp {
public:
    bool OnInit() override {
        try {
            db_ = std::make_unique<Db>("warehouse.db");
            db_->initSchema(); 
            storage_ = std::make_unique<Storage>(*db_);
        } catch (const std::exception& ex) {
            wxMessageBox(ex.what(), "DB error", wxOK | wxICON_ERROR);
            return false;
        }

        auto* frame = new MainFrame(*storage_);
        frame->Show(true);
        return true;
    }

private:
    std::unique_ptr<Db>      db_;
    std::unique_ptr<Storage> storage_;
};

wxIMPLEMENT_APP(WarehouseApp);
