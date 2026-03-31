#pragma once
#include <string>

struct Item {
    int         id;
    std::string name;
    std::string category;
    std::string location;
    int         quantity;
    
    // Extended fields
    std::string sku;          // Артикул/штрихкод
    double      price;        // Цена закупки
    std::string supplier;     // Поставщик
    int         minQuantity;  // Минимальный остаток для alerts
    std::string description;  // Описание
};
