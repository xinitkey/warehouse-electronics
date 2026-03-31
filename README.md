# Warehouse Electronics

A desktop application for managing electronics inventory in a warehouse. Built with C++, wxWidgets, and SQLite.

## Features

- **CRUD Operations** - Add, edit, delete, and view warehouse items
- **Search & Filter** - Real-time search by name, category, or location
- **Statistics Dashboard** - View total items, quantities, and category breakdown
- **Low Stock Alerts** - Visual warnings when items fall below threshold (default: 5 units)
- **Keyboard Shortcuts** - Quick access to common actions

## Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| `Ctrl+N` | Add new item |
| `Ctrl+E` | Edit selected item |
| `Ctrl+D` | Delete selected item |
| `Ctrl+F` | Focus search box |

## Database Schema

```sql
CREATE TABLE items (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    category TEXT,
    location TEXT,
    quantity INTEGER NOT NULL DEFAULT 0
);
```

## Building

### Prerequisites

- CMake 3.20+
- C++17 compiler
- wxWidgets
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
│   ├── models.h           # Data structures (Item, Stats)
│   ├── db.h               # Database wrapper
│   ├── storage.h          # Data access layer
│   ├── add_item_dialog.h  # Add/Edit dialog
│   └── ui_add_item_dialog.h
├── src/
│   ├── main.cpp           # Application entry point
│   ├── ui_main_frame.cpp  # Main window implementation
│   ├── ui_main_frame.h    # Main window header
│   └── warehouse/
│       ├── db.cpp         # SQLite initialization
│       ├── storage.cpp    # CRUD operations + search + stats
│       └── ui_add_item_dialog.cpp
├── CMakeLists.txt
└── warehouse.db           # SQLite database
```

## Usage

1. **Add Item** - Click "Add" button or press `Ctrl+N`
2. **Edit Item** - Select item and click "Edit" or press `Ctrl+E` (or double-click)
3. **Delete Item** - Select item and click "Delete" or press `Ctrl+D`
4. **Search** - Type in the search box or press `Ctrl+F` to focus it
5. **View Stats** - Statistics are displayed at the top of the window

## Future Enhancements

- [ ] Barcode/SKU support
- [ ] Price tracking (cost/sale price)
- [ ] Supplier management
- [ ] Import/Export (CSV, Excel)
- [ ] Operation history log
- [ ] PDF report generation
- [ ] Dark theme support
- [ ] Multi-user support with authentication

## License

MIT
