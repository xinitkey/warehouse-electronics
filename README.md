# Warehouse Electronics

A desktop application for managing electronics inventory in a warehouse. Built with C++, wxWidgets, and SQLite.

![Version](https://img.shields.io/badge/version-1.1-blue)
![License](https://img.shields.io/badge/license-MIT-green)

## Features

- **CRUD Operations** - Add, edit, delete, and view warehouse items
- **Extended Item Fields** - SKU, price, supplier, minimum quantity, description
- **Search & Filter** - Real-time search by name, category, location, SKU, or supplier
- **Statistics Dashboard** - Total items, quantities, total value, and category breakdown
- **Low Stock Alerts** - Visual warnings when items fall below minimum threshold
- **CSV Import/Export** - Bulk data operations
- **Operation Log** - Track inventory movements (purchase, sale, return, write-off)
- **Keyboard Shortcuts** - Quick access to common actions
- **Tabbed Dialog** - Organized input with 3 tabs (Basic Info, Pricing, Description)

## Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| `Ctrl+N` | Add new item |
| `Ctrl+E` | Edit selected item |
| `Ctrl+D` | Delete selected item |
| `Ctrl+F` | Focus search box |
| `Ctrl+I` | Import from CSV |
| `Ctrl+S` | Export to CSV |
| `Alt+X` | Exit application |

## Database Schema

```sql
CREATE TABLE items (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    category TEXT,
    location TEXT,
    quantity INTEGER NOT NULL DEFAULT 0,
    sku TEXT DEFAULT '',
    price REAL DEFAULT 0.0,
    supplier TEXT DEFAULT '',
    min_quantity INTEGER DEFAULT 5,
    description TEXT DEFAULT ''
);

CREATE TABLE operation_logs (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    item_id INTEGER NOT NULL,
    item_name TEXT NOT NULL,
    operation_type INTEGER NOT NULL,  -- 0=Purchase, 1=Sale, 2=Return, 3=WriteOff
    quantity_change INTEGER NOT NULL,
    comment TEXT DEFAULT '',
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY(item_id) REFERENCES items(id)
);
```

## Building

### Prerequisites

- CMake 3.20+
- C++17 compiler
- wxWidgets 3.2+
- SQLite3

### Linux (Arch Linux)

```bash
# Install dependencies
sudo pacman -S wxwidgets-gtk3 wxwidgets-common sqlite cmake base-devel

# Build
cmake -B build-linux -S .
cmake --build build-linux --parallel

# Run
./build-linux/warehouse_app
```

### Windows (with vcpkg)

```powershell
# Install dependencies via vcpkg
vcpkg install wxwidgets sqlite3

# Build
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=[vcpkg-root]/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Release

# Run
.\build\Release\warehouse_app.exe
```

## Project Structure

```
warehouse-electronics/
├── include/warehouse/
│   ├── models.h              # Data structures (Item, Stats, OperationLog)
│   ├── db.h                  # Database wrapper
│   ├── storage.h             # Data access layer
│   ├── add_item_dialog.h     # Add/Edit dialog
│   └── ui_add_item_dialog.h
├── src/
│   ├── main.cpp              # Application entry point
│   ├── ui_main_frame.cpp     # Main window implementation
│   ├── ui_main_frame.h       # Main window header
│   └── warehouse/
│       ├── db.cpp            # SQLite initialization + migrations
│       ├── storage.cpp       # CRUD, search, stats, operations, CSV
│       └── ui_add_item_dialog.cpp
├── CMakeLists.txt
└── warehouse.db              # SQLite database
```

## Usage

### Basic Operations
1. **Add Item** - Click "Add" button or press `Ctrl+N`
2. **Edit Item** - Select item and click "Edit" or press `Ctrl+E` (or double-click)
3. **Delete Item** - Select item and click "Delete" or press `Ctrl+D`
4. **Search** - Type in the search box or press `Ctrl+F` to focus it
5. **View Stats** - Statistics displayed at the top of the window

### Import/Export
- **Export CSV** - `File → Export CSV` or `Ctrl+S`
- **Import CSV** - `File → Import CSV` or `Ctrl+I`

### CSV Format
```csv
ID,Name,Category,Location,Quantity,SKU,Price,Supplier,MinQuantity,Description
1,MacBook Pro 16" M3,Laptops,Shelf A1,8,LAP-001,2499.00,Apple,3,"Latest M3 chip, 18GB RAM, 512GB SSD"
2,Dell XPS 15,Laptops,Shelf A1,12,LAP-002,1799.00,Dell,5,"Intel i9, 32GB RAM, 1TB SSD"
3,Logitech MX Master 3S,Mice,Shelf C1,50,MOU-001,99.99,Logitech,15,"Wireless, 8000 DPI"
4,iPhone 15 Pro Max,Smartphones,Vault 1,25,PHN-001,1199.00,Apple,8,"256GB, Titanium"
5,Samsung 990 PRO 2TB,SSDs,Shelf F1,50,SSD-001,169.99,Samsung,15,"NVMe Gen4, 7450 MB/s"
```

## Table Columns

| Column | Description |
|--------|-------------|
| ID | Unique identifier |
| Name | Item name |
| SKU | Stock keeping unit / barcode |
| Category | Item category |
| Location | Warehouse location |
| Supplier | Supplier name |
| Price | Cost price |
| Qty | Current quantity |
| Min | Minimum stock threshold |
| Description | Item description |

## Version History

### v1.1 (Latest)
- ✅ Extended item fields (SKU, price, supplier, min_quantity, description)
- ✅ Database migration system
- ✅ Operation log tracking
- ✅ CSV import/export
- ✅ Tabbed add/edit dialog
- ✅ Menu bar with File and Help menus
- ✅ Total inventory value in stats
- ✅ Extended search (SKU, supplier)

### v1.0
- Basic CRUD operations
- Search functionality
- Statistics dashboard
- Low stock alerts
- Keyboard shortcuts

## Future Enhancements

- [ ] Barcode scanner support
- [ ] Sale price tracking
- [ ] PDF report generation
- [ ] Dark theme support
- [ ] Multi-user authentication
- [ ] Cloud sync
- [ ] Mobile companion app

## License

MIT
