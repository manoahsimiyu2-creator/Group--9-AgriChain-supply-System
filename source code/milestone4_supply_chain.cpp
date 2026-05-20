// ============================================================
//  ICS 2276: Computer Programming II
//  Agricultural Supply Chain System
//  Milestone 4: Modular Architecture
//  ** UPDATED: 1000 Realistic Kenyan Datasets ** & System Robustness
//
//  CHANGES FROM MILESTONE 3:
//  - Custom exception classes for domain-specific errors
//  - Exception handling (try/catch) on all critical operations
//  - File handling: save and load inventory to/from CSV files
//  - FileLogger class: logs all system events to a log file
//  - IStorable interface: enforces save/load contract on classes
//  - IDisplayable interface: enforces display contract
//  - Modular design: each concern separated into clear sections
//  - Persistent data: inventory and orders survive program exit
// ============================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <ctime>
#include "KenyanDataGenerator.h"

using namespace std;

// ============================================================
//  CONSTANTS
// ============================================================
const int MAX_PRODUCTS  = 200;
const int MAX_ORDERS    = 750;
const int MAX_SUPPLIERS =  50;

const string INVENTORY_FILE = "inventory.csv";
const string ORDERS_FILE    = "orders.csv";
const string LOG_FILE       = "system.log";


// ============================================================
//  MODULE 1: CUSTOM EXCEPTION CLASSES
//  Domain-specific exceptions for the supply chain system
// ============================================================

// Base exception for all supply chain errors
class SupplyChainException : public exception {
protected:
    string message;
public:
    SupplyChainException(const string& msg) : message(msg) {}
    const char* what() const noexcept override { return message.c_str(); }
};

// Thrown when stock operation is invalid
class InsufficientStockException : public SupplyChainException {
public:
    InsufficientStockException(const string& product, int requested, int available)
        : SupplyChainException("Insufficient stock for '" + product +
                               "'. Requested: " + to_string(requested) +
                               " kg, Available: " + to_string(available) + " kg") {}
};

// Thrown when warehouse capacity is exceeded
class WarehouseCapacityException : public SupplyChainException {
public:
    WarehouseCapacityException(int incoming, int available)
        : SupplyChainException("Warehouse capacity exceeded. Incoming: " +
                               to_string(incoming) + " kg, Available space: " +
                               to_string(available) + " kg") {}
};

// Thrown when an invalid product/supplier ID is used
class InvalidEntityException : public SupplyChainException {
public:
    InvalidEntityException(const string& entity, int id)
        : SupplyChainException("Invalid " + entity + " ID: " + to_string(id)) {}
};

// Thrown for file I/O errors
class FileIOException : public SupplyChainException {
public:
    FileIOException(const string& filename, const string& operation)
        : SupplyChainException("File " + operation + " failed for: " + filename) {}
};

// Thrown for invalid input data
class InvalidDataException : public SupplyChainException {
public:
    InvalidDataException(const string& detail)
        : SupplyChainException("Invalid data: " + detail) {}
};


// ============================================================
//  MODULE 2: INTERFACES
//  Abstract interfaces enforcing contracts on classes
// ============================================================

// Interface: any class that can be saved/loaded from file
class IStorable {
public:
    virtual string toCSV()           const = 0;
    virtual void   fromCSV(const string& line) = 0;
    virtual ~IStorable() {}
};

// Interface: any class that can display itself
class IDisplayable {
public:
    virtual void display() const = 0;
    virtual ~IDisplayable() {}
};


// ============================================================
//  MODULE 3: FILE LOGGER
//  Logs all system events with timestamps to system.log
// ============================================================
class FileLogger {
private:
    string logFile;

    string getTimestamp() const {
        time_t now = time(0);
        char buf[20];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
        return string(buf);
    }

public:
    FileLogger(const string& filename) : logFile(filename) {}

    void log(const string& level, const string& message) {
        ofstream file(logFile, ios::app);
        if (file.is_open()) {
            file << "[" << getTimestamp() << "] [" << level << "] " << message << "\n";
            file.close();
        }
    }

    void info(const string& msg)    { log("INFO ", msg); }
    void warning(const string& msg) { log("WARN ", msg); }
    void error(const string& msg)   { log("ERROR", msg); }

    void displayLog() const {
        cout << "\n============================================================\n";
        cout << "  SYSTEM LOG (" << logFile << ")\n";
        cout << "============================================================\n";
        ifstream file(logFile);
        if (!file.is_open()) {
            cout << "  [No log file found]\n";
            return;
        }
        string line;
        while (getline(file, line)) {
            cout << "  " << line << "\n";
        }
        file.close();
        cout << "============================================================\n";
    }
};

// Global logger instance
FileLogger logger(LOG_FILE);


// ============================================================
//  MODULE 4: PRODUCT CLASSES (evolved from M3)
//  Now implement IStorable and IDisplayable interfaces
// ============================================================

class Product : public IDisplayable {
protected:
    int    productID;
    string name;
    string category;
    double unitPrice;
    int    quantityInStock;
    int    reorderLevel;

public:
    Product() : productID(0), name(""), category(""),
                unitPrice(0), quantityInStock(0), reorderLevel(0) {}

    Product(int id, string n, string cat, double price, int qty, int reorder)
        : productID(id), name(n), category(cat),
          unitPrice(price), quantityInStock(qty), reorderLevel(reorder) {}

    virtual ~Product() {}

    // Pure virtual methods
    virtual string getProductType() const = 0;
    virtual string getStorageInfo() const = 0;
    virtual void   display()        const = 0;
    virtual string toCSV()          const = 0;

    // Getters
    int    getID()           const { return productID; }
    string getName()         const { return name; }
    string getCategory()     const { return category; }
    double getUnitPrice()    const { return unitPrice; }
    int    getQuantity()     const { return quantityInStock; }
    int    getReorderLevel() const { return reorderLevel; }

    void setUnitPrice(double price) {
        if (price <= 0) throw InvalidDataException("Unit price must be positive");
        unitPrice = price;
    }

    double getTotalValue()   const { return quantityInStock * unitPrice; }
    bool   isReorderNeeded() const { return quantityInStock <= reorderLevel; }

    // addStock with exception handling
    void addStock(int qty) {
        if (qty <= 0) throw InvalidDataException("Stock quantity must be positive");
        quantityInStock += qty;
        logger.info("Added " + to_string(qty) + " kg to " + name);
    }

    void addStock(int qty, const string& supplierNote) {
        if (qty <= 0) throw InvalidDataException("Stock quantity must be positive");
        quantityInStock += qty;
        logger.info("Added " + to_string(qty) + " kg to " + name + " from " + supplierNote);
    }

    // dispatchStock with exception handling
    void dispatchStock(int qty) {
        if (qty <= 0) throw InvalidDataException("Dispatch quantity must be positive");
        if (qty > quantityInStock)
            throw InsufficientStockException(name, qty, quantityInStock);
        quantityInStock -= qty;
        logger.info("Dispatched " + to_string(qty) + " kg from " + name);
    }

    void displayBase() const {
        cout << fixed << setprecision(2);
        cout << "    ID           : " << productID           << "\n";
        cout << "    Type         : " << getProductType()    << "\n";
        cout << "    Name         : " << name                << "\n";
        cout << "    Category     : " << category            << "\n";
        cout << "    Unit Price   : KES " << unitPrice       << " / kg\n";
        cout << "    In Stock     : " << quantityInStock     << " kg\n";
        cout << "    Reorder Level: " << reorderLevel        << " kg\n";
        cout << "    Total Value  : KES " << getTotalValue() << "\n";
        cout << "    Storage Info : " << getStorageInfo()    << "\n";
        cout << "    Reorder Alert: " << (isReorderNeeded() ? "*** YES ***" : "No") << "\n";
    }
};


// FreshProduce (evolved from M3 — adds toCSV/fromCSV)
class FreshProduce : public Product {
private:
    int    shelfLifeDays;
    double storageTemp;

public:
    FreshProduce() : shelfLifeDays(0), storageTemp(0.0) {}

    FreshProduce(int id, string n, string cat, double price,
                 int qty, int reorder, int shelfLife, double temp)
        : Product(id, n, cat, price, qty, reorder),
          shelfLifeDays(shelfLife), storageTemp(temp) {}

    string getProductType() const override { return "FreshProduce"; }

    string getStorageInfo() const override {
        return "Temp: " + to_string((int)storageTemp) +
               "C | Shelf Life: " + to_string(shelfLifeDays) + " days";
    }

    void display() const override {
        cout << "  [FRESH PRODUCE]\n";
        displayBase();
        cout << "    Shelf Life   : " << shelfLifeDays << " days\n";
        cout << "    Storage Temp : " << storageTemp   << " C\n";
    }

    // IStorable: serialize to CSV
    string toCSV() const override {
        return "FreshProduce," + to_string(productID) + "," + name + "," +
               category + "," + to_string(unitPrice) + "," +
               to_string(quantityInStock) + "," + to_string(reorderLevel) +
               "," + to_string(shelfLifeDays) + "," + to_string((int)storageTemp);
    }

    int getShelfLife() const { return shelfLifeDays; }
};


// ProcessedGood (evolved from M3 — adds toCSV/fromCSV)
class ProcessedGood : public Product {
private:
    string packagingType;
    int    expiryMonths;

public:
    ProcessedGood() : packagingType(""), expiryMonths(0) {}

    ProcessedGood(int id, string n, string cat, double price,
                  int qty, int reorder, string packaging, int expiry)
        : Product(id, n, cat, price, qty, reorder),
          packagingType(packaging), expiryMonths(expiry) {}

    string getProductType() const override { return "ProcessedGood"; }

    string getStorageInfo() const override {
        return "Packaging: " + packagingType +
               " | Expiry: " + to_string(expiryMonths) + " months";
    }

    void display() const override {
        cout << "  [PROCESSED GOOD]\n";
        displayBase();
        cout << "    Packaging    : " << packagingType << "\n";
        cout << "    Expiry       : " << expiryMonths  << " months\n";
    }

    string toCSV() const override {
        return "ProcessedGood," + to_string(productID) + "," + name + "," +
               category + "," + to_string(unitPrice) + "," +
               to_string(quantityInStock) + "," + to_string(reorderLevel) +
               "," + packagingType + "," + to_string(expiryMonths);
    }

    string getPackaging() const { return packagingType; }
};


// ============================================================
//  MODULE 5: SUPPLIER (evolved — adds toCSV)
// ============================================================
class Supplier : public IStorable, public IDisplayable {
private:
    int    supplierID;
    string name;
    string location;
    string contact;
    double reliabilityRating;

public:
    Supplier() : supplierID(0), name(""), location(""),
                 contact(""), reliabilityRating(0.0) {}

    Supplier(int id, string n, string loc, string con, double rating)
        : supplierID(id), name(n), location(loc),
          contact(con), reliabilityRating(rating) {}

    int    getID()       const { return supplierID; }
    string getName()     const { return name; }
    string getLocation() const { return location; }
    double getRating()   const { return reliabilityRating; }
    bool   isReliable()  const { return reliabilityRating >= 7.0; }

    void display() const override {
        cout << "    ID         : " << supplierID        << "\n";
        cout << "    Name       : " << name              << "\n";
        cout << "    Location   : " << location          << "\n";
        cout << "    Contact    : " << contact           << "\n";
        cout << "    Reliability: " << reliabilityRating << "/10 "
             << (isReliable() ? "[APPROVED]" : "[UNRELIABLE]") << "\n";
    }

    string toCSV() const override {
        return to_string(supplierID) + "," + name + "," + location +
               "," + contact + "," + to_string(reliabilityRating);
    }

    void fromCSV(const string& line) override {
        stringstream ss(line);
        string token;
        getline(ss, token, ','); supplierID        = stoi(token);
        getline(ss, name,  ',');
        getline(ss, location, ',');
        getline(ss, contact, ',');
        getline(ss, token, ','); reliabilityRating = stod(token);
    }
};


// ============================================================
//  MODULE 6: WAREHOUSE (evolved — adds toCSV)
// ============================================================
class Warehouse {
private:
    int    warehouseID;
    string location;
    int    capacity;
    int    currentStock;  // tracked internally

public:
    Warehouse(int id, string loc, int cap)
        : warehouseID(id), location(loc), capacity(cap), currentStock(0) {}

    int    getID()           const { return warehouseID; }
    string getLocation()     const { return location; }
    int    getCapacity()     const { return capacity; }
    int    getCurrentStock() const { return currentStock; }

    void setCurrentStock(int stock) { currentStock = stock; }

    void canAccommodate(int incoming) const {
        int space = capacity - currentStock;
        if (incoming > space)
            throw WarehouseCapacityException(incoming, space);
    }

    // Keep overload for backward compatibility
    void canAccommodate(int stock, int incoming) const {
        int space = capacity - stock;
        if (incoming > space)
            throw WarehouseCapacityException(incoming, space);
    }

    int availableSpace() const { return capacity - currentStock; }

    double getUsagePercentage() const {
        return (static_cast<double>(currentStock) / capacity) * 100.0;
    }

    // display() with no parameters — satisfies IDisplayable contract
    void display() const {
        cout << fixed << setprecision(2);
        cout << "    ID         : " << warehouseID   << "\n";
        cout << "    Location   : " << location      << "\n";
        cout << "    Capacity   : " << capacity      << " kg\n";
        cout << "    In Use     : " << currentStock  << " kg ("
             << getUsagePercentage() << "%)\n";
        cout << "    Free Space : " << availableSpace() << " kg\n";
    }

    // Overload for explicit stock value (used by InventoryManager)
    void display(int stock) const {
        cout << fixed << setprecision(2);
        cout << "    ID         : " << warehouseID << "\n";
        cout << "    Location   : " << location    << "\n";
        cout << "    Capacity   : " << capacity    << " kg\n";
        cout << "    In Use     : " << stock << " kg ("
             << (static_cast<double>(stock)/capacity*100.0) << "%)\n";
        cout << "    Free Space : " << (capacity - stock) << " kg\n";
    }
};


// ============================================================
//  MODULE 7: ORDER (evolved — adds toCSV)
// ============================================================
class Order : public IStorable, public IDisplayable {
public:
    static const int PENDING  = 0;
    static const int APPROVED = 1;
    static const int REJECTED = 2;

private:
    int    orderID;
    int    productID;
    string productName;
    int    quantity;
    double totalCost;
    int    status;
    string reason;

public:
    Order() : orderID(0), productID(0), productName(""),
              quantity(0), totalCost(0.0), status(PENDING), reason("") {}

    Order(int oid, int pid, string pname, int qty, double unitPrice)
        : orderID(oid), productID(pid), productName(pname),
          quantity(qty), totalCost(qty * unitPrice),
          status(PENDING), reason("") {}

    int    getID()        const { return orderID; }
    int    getProductID() const { return productID; }
    int    getQuantity()  const { return quantity; }
    double getTotalCost() const { return totalCost; }
    int    getStatus()    const { return status; }

    string getStatusLabel() const {
        if (status == APPROVED) return "APPROVED";
        if (status == REJECTED) return "REJECTED";
        return "PENDING";
    }

    void evaluate(const Supplier& supplier, const Warehouse& warehouse, int currentStock) {
        try {
            if (!supplier.isReliable())
                throw InvalidDataException("Supplier reliability below threshold (7.0)");
            warehouse.canAccommodate(currentStock, quantity);
            status = APPROVED;
            reason = "All checks passed";
            logger.info("Order " + to_string(orderID) + " APPROVED for " + productName);
        } catch (const SupplyChainException& e) {
            status = REJECTED;
            reason = e.what();
            logger.warning("Order " + to_string(orderID) + " REJECTED: " + reason);
        }
    }

    void display() const override {
        cout << fixed << setprecision(2);
        cout << "    Order ID   : " << orderID          << "\n";
        cout << "    Product    : " << productName      << " (ID:" << productID << ")\n";
        cout << "    Quantity   : " << quantity         << " kg\n";
        cout << "    Total Cost : KES " << totalCost    << "\n";
        cout << "    Status     : " << getStatusLabel() << "\n";
        cout << "    Reason     : " << reason           << "\n";
    }

    string toCSV() const override {
        return to_string(orderID) + "," + to_string(productID) + "," +
               productName + "," + to_string(quantity) + "," +
               to_string(totalCost) + "," + getStatusLabel() + "," + reason;
    }

    void fromCSV(const string& line) override {
        stringstream ss(line);
        string token;
        getline(ss, token, ',');      orderID     = stoi(token);
        getline(ss, token, ',');      productID   = stoi(token);
        getline(ss, productName, ',');
        getline(ss, token, ',');      quantity    = stoi(token);
        getline(ss, token, ',');      totalCost   = stod(token);
        getline(ss, token, ',');
        if (token == "APPROVED")      status = APPROVED;
        else if (token == "REJECTED") status = REJECTED;
        else                          status = PENDING;
        getline(ss, reason, ',');
    }
};


// ============================================================
//  MODULE 8: FILE PERSISTENCE MANAGER
//  Handles all save/load operations for the system
// ============================================================
class PersistenceManager {
public:
    // Save all products to CSV
    static void saveProducts(Product** products, int count) {
        ofstream file(INVENTORY_FILE);
        if (!file.is_open())
            throw FileIOException(INVENTORY_FILE, "write");

        file << "type,id,name,category,price,qty,reorder,extra1,extra2\n";
        for (int i = 0; i < count; i++) {
            file << products[i]->toCSV() << "\n";
        }
        file.close();
        logger.info("Inventory saved to " + INVENTORY_FILE);
        cout << "  [OK] Inventory saved to " << INVENTORY_FILE << "\n";
    }

    // Save all orders to CSV
    static void saveOrders(Order* orders, int count) {
        ofstream file(ORDERS_FILE);
        if (!file.is_open())
            throw FileIOException(ORDERS_FILE, "write");

        file << "orderID,productID,productName,quantity,totalCost,status,reason\n";
        for (int i = 0; i < count; i++) {
            file << orders[i].toCSV() << "\n";
        }
        file.close();
        logger.info("Orders saved to " + ORDERS_FILE);
        cout << "  [OK] Orders saved to " << ORDERS_FILE << "\n";
    }

    // Load orders from CSV
    static int loadOrders(Order* orders, int maxCount) {
        ifstream file(ORDERS_FILE);
        if (!file.is_open()) {
            logger.warning("Orders file not found. Starting fresh.");
            return 0;
        }

        string line;
        getline(file, line); // skip header
        int count = 0;
        while (getline(file, line) && count < maxCount) {
            if (!line.empty()) {
                orders[count].fromCSV(line);
                count++;
            }
        }
        file.close();
        logger.info("Loaded " + to_string(count) + " orders from " + ORDERS_FILE);
        cout << "  [OK] Loaded " << count << " orders from " << ORDERS_FILE << "\n";
        return count;
    }
};


// ============================================================
//  MODULE 9: INVENTORY MANAGER (evolved from M3)
//  Now uses exception handling and file persistence
// ============================================================
class InventoryManager {
private:
    Product*  products[MAX_PRODUCTS];
    int       productCount;
    Order     orders[MAX_ORDERS];
    int       orderCount;
    Supplier  suppliers[MAX_SUPPLIERS];
    int       supplierCount;
    Warehouse warehouse;
    int       orderCounter;

public:
    InventoryManager(Warehouse w)
        : warehouse(w), productCount(0),
          orderCount(0), supplierCount(0), orderCounter(8001) {}

    ~InventoryManager() {
        for (int i = 0; i < productCount; i++) delete products[i];
    }

    bool addProduct(Product* p) {
        if (productCount >= MAX_PRODUCTS) return false;
        products[productCount++] = p;
        return true;
    }

    bool addSupplier(Supplier s) {
        if (supplierCount >= MAX_SUPPLIERS) return false;
        suppliers[supplierCount++] = s;
        return true;
    }

    void loadKenyanData() {
        for (const auto& s : KENYAN_SUPPLIERS) {
            if (supplierCount >= MAX_SUPPLIERS) break;
            addSupplier(Supplier(s.id, s.name, s.location, s.contact, s.rating));
        }
        for (const auto& p : KENYAN_PRODUCTS) {
            if (productCount >= MAX_PRODUCTS) break;
            if (p.type == "Fresh")
                addProduct(new FreshProduce(p.id, p.name, p.category, p.unitPrice,
                    p.quantity, p.reorderLevel, p.shelfLifeDays, p.storageTemp));
            else
                addProduct(new ProcessedGood(p.id, p.name, p.category, p.unitPrice,
                    p.quantity, p.reorderLevel, p.packagingType, p.expiryMonths));
        }
        logger.info("Loaded " + to_string(productCount) + " products and " +
                    to_string(supplierCount) + " suppliers from Kenyan dataset");
        cout << "[INIT] Loaded " << supplierCount << " suppliers and "
             << productCount << " products.\n";
    }



    int getTotalStock(int index = 0) const {
        if (index >= productCount) return 0;
        return products[index]->getQuantity() + getTotalStock(index + 1);
    }

    double getTotalValue(int index = 0) const {
        if (index >= productCount) return 0.0;
        return products[index]->getTotalValue() + getTotalValue(index + 1);
    }

    // Add stock with full exception handling
    void addStock(int productIndex, int qty) {
        try {
            if (productIndex < 0 || productIndex >= productCount)
                throw InvalidEntityException("product index", productIndex);
            warehouse.canAccommodate(getTotalStock(), qty);
            products[productIndex]->addStock(qty);
            warehouse.setCurrentStock(getTotalStock());
            cout << "  [OK] Added " << qty << " kg to "
                 << products[productIndex]->getName() << "\n";
        } catch (const WarehouseCapacityException& e) {
            logger.error(e.what());
            cout << "  [ERROR] " << e.what() << "\n";
        } catch (const InvalidDataException& e) {
            logger.error(e.what());
            cout << "  [ERROR] " << e.what() << "\n";
        } catch (const SupplyChainException& e) {
            logger.error(e.what());
            cout << "  [ERROR] " << e.what() << "\n";
        }
    }

    // Dispatch stock with full exception handling
    void dispatchStock(int productIndex, int qty) {
        try {
            if (productIndex < 0 || productIndex >= productCount)
                throw InvalidEntityException("product index", productIndex);
            products[productIndex]->dispatchStock(qty);
            cout << "  [OK] Dispatched " << qty << " kg from "
                 << products[productIndex]->getName() << "\n";
            if (products[productIndex]->isReorderNeeded()) {
                logger.warning("Reorder needed for " + products[productIndex]->getName());
                cout << "  *** REORDER ALERT for "
                     << products[productIndex]->getName() << "!\n";
            }
        } catch (const InsufficientStockException& e) {
            logger.error(e.what());
            cout << "  [ERROR] " << e.what() << "\n";
        } catch (const InvalidDataException& e) {
            logger.error(e.what());
            cout << "  [ERROR] " << e.what() << "\n";
        } catch (const SupplyChainException& e) {
            logger.error(e.what());
            cout << "  [ERROR] " << e.what() << "\n";
        }
    }

    // Place order with full exception handling
    void placeOrder(int productIndex, int supplierIndex, int qty) {
        try {
            if (productIndex < 0 || productIndex >= productCount)
                throw InvalidEntityException("product index", productIndex);
            if (supplierIndex < 0 || supplierIndex >= supplierCount)
                throw InvalidEntityException("supplier index", supplierIndex);

            Product*  p = products[productIndex];
            Supplier& s = suppliers[supplierIndex];

            Order order(orderCounter++, p->getID(), p->getName(), qty, p->getUnitPrice());
            order.evaluate(s, warehouse, getTotalStock());

            if (orderCount < MAX_ORDERS) orders[orderCount++] = order;

            cout << "\n  [ORDER RESULT]\n";
            order.display();

            if (order.getStatus() == Order::APPROVED) {
                p->addStock(qty, s.getName());
                cout << "  [OK] Stock updated. New stock: " << p->getQuantity() << " kg\n";
            }
        } catch (const SupplyChainException& e) {
            logger.error(e.what());
            cout << "  [ERROR] " << e.what() << "\n";
        }
    }

    // Save data to files
    void saveData() {
        try {
            PersistenceManager::saveProducts(products, productCount);
            PersistenceManager::saveOrders(orders, orderCount);
        } catch (const FileIOException& e) {
            logger.error(e.what());
            cout << "  [ERROR] " << e.what() << "\n";
        }
    }

    // Load orders from file
    void loadData() {
        try {
            orderCount = PersistenceManager::loadOrders(orders, MAX_ORDERS);
        } catch (const FileIOException& e) {
            logger.error(e.what());
            cout << "  [ERROR] " << e.what() << "\n";
        }
    }

    // Search
    Product* findProductByName(const string& name) const {
        for (int i = 0; i < productCount; i++)
            if (products[i]->getName() == name) return products[i];
        return nullptr;
    }

    void listProductsByCategory(const string& cat) const {
        bool found = false;
        cout << "\n  Products in [" << cat << "]:\n";
        for (int i = 0; i < productCount; i++) {
            if (products[i]->getCategory() == cat) {
                products[i]->display();
                cout << "  " << string(50, '-') << "\n";
                found = true;
            }
        }
        if (!found) cout << "  No products found.\n";
    }

    void displayAllProducts() const {
        cout << "\n============================================================\n";
        cout << "  PRODUCT INVENTORY (" << productCount << " products)\n";
        cout << "============================================================\n";
        for (int i = 0; i < productCount; i++) {
            cout << "\n  [" << i << "] ";
            products[i]->display();
            cout << "  " << string(50, '-') << "\n";
        }
        cout << "\n  Total Stock : " << getTotalStock() << " kg\n";
        cout << "  Total Value : KES " << fixed << setprecision(2) << getTotalValue() << "\n";
    }

    void displayAllOrders() const {
        cout << "\n============================================================\n";
        cout << "  ORDER HISTORY (" << orderCount << " orders)\n";
        cout << "============================================================\n";
        if (orderCount == 0) { cout << "  No orders yet.\n"; return; }
        for (int i = 0; i < orderCount; i++) {
            cout << "\n  Order #" << (i + 1) << ":\n";
            orders[i].display();
            cout << "  " << string(50, '-') << "\n";
        }
    }

    void displayAllSuppliers() const {
        cout << "\n============================================================\n";
        cout << "  SUPPLIERS (" << supplierCount << ")\n";
        cout << "============================================================\n";
        for (int i = 0; i < supplierCount; i++) {
            cout << "\n  [" << i << "]\n";
            suppliers[i].display();
        }
    }

    void displayDashboard() const {
        cout << "\n============================================================\n";
        cout << "                    SYSTEM DASHBOARD\n";
        cout << "============================================================\n";
        cout << fixed << setprecision(2);
        cout << "  Products Tracked : " << productCount  << "\n";
        cout << "  Suppliers        : " << supplierCount << "\n";
        cout << "  Orders Placed    : " << orderCount    << "\n";
        cout << "  Total Stock      : " << getTotalStock() << " kg\n";
        cout << "  Total Value      : KES " << getTotalValue() << "\n";
        cout << "\n  [WAREHOUSE]\n";
        warehouse.display(getTotalStock());
        cout << "\n  [REORDER ALERTS]\n";
        bool any = false;
        for (int i = 0; i < productCount; i++) {
            if (products[i]->isReorderNeeded()) {
                cout << "  *** " << products[i]->getName() << " needs reorder!\n";
                any = true;
            }
        }
        if (!any) cout << "  All stock levels sufficient.\n";
        cout << "============================================================\n";
    }

    int getProductCount()  const { return productCount; }
    int getSupplierCount() const { return supplierCount; }
    Product* getProduct(int i)  { return (i >= 0 && i < productCount)  ? products[i]  : nullptr; }
    Supplier* getSupplier(int i){ return (i >= 0 && i < supplierCount) ? &suppliers[i] : nullptr; }
};


// ============================================================
//  INPUT VALIDATION (carried from M3)
// ============================================================
int getValidInt(const string& prompt, int minVal, int maxVal) {
    int value;
    while (true) {
        cout << prompt;
        cin >> value;
        if (cin.fail() || value < minVal || value > maxVal) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "  [ERROR] Enter a number between " << minVal << " and " << maxVal << ".\n";
        } else {
            cin.ignore(1000, '\n');
            return value;
        }
    }
}

double getValidDouble(const string& prompt, double minVal) {
    double value;
    while (true) {
        cout << prompt;
        cin >> value;
        if (cin.fail() || value < minVal) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "  [ERROR] Enter a valid number >= " << minVal << ".\n";
        } else {
            cin.ignore(1000, '\n');
            return value;
        }
    }
}

string getValidString(const string& prompt) {
    string value;
    cout << prompt;
    cin.ignore();
    getline(cin, value);
    return value;
}


// ============================================================
//  MENU & SIMULATION
// ============================================================
void displayMenu() {
    cout << "\n---- MAIN MENU ----------------------------------------\n";
    cout << "  1. View all products\n";
    cout << "  2. Add stock\n";
    cout << "  3. Dispatch stock\n";
    cout << "  4. Place supply order\n";
    cout << "  5. View order history\n";
    cout << "  6. Search product by name\n";
    cout << "  7. List products by category\n";
    cout << "  8. View all suppliers\n";
    cout << "  9. View dashboard\n";
    cout << "  10. Save data to files\n";
    cout << "  11. View system log\n";
    cout << "  0. Exit\n";
    cout << "-------------------------------------------------------\n";
}

void runSimulation(InventoryManager& mgr) {
    bool running = true;
    cout << "\n[SIM] Simulation started with exception handling & file persistence.\n";

    while (running) {
        displayMenu();
        int choice = getValidInt("  Enter choice: ", 0, 11);

        if (choice == 1) {
            mgr.displayAllProducts();
        } else if (choice == 2) {
            mgr.displayAllProducts();
            int idx = getValidInt("  Select product index: ", 0, mgr.getProductCount() - 1);
            int qty = getValidInt("  Enter quantity (kg): ", 1, 99999);
            mgr.addStock(idx, qty);
        } else if (choice == 3) {
            mgr.displayAllProducts();
            int idx = getValidInt("  Select product index: ", 0, mgr.getProductCount() - 1);
            int qty = getValidInt("  Enter quantity (kg): ", 1, 99999);
            mgr.dispatchStock(idx, qty);
        } else if (choice == 4) {
            mgr.displayAllProducts();
            int pidx = getValidInt("  Select product index: ", 0, mgr.getProductCount() - 1);
            mgr.displayAllSuppliers();
            int sidx = getValidInt("  Select supplier index: ", 0, mgr.getSupplierCount() - 1);
            int qty  = getValidInt("  Enter order quantity (kg): ", 1, 99999);
            mgr.placeOrder(pidx, sidx, qty);
        } else if (choice == 5) {
            mgr.displayAllOrders();
        } else if (choice == 6) {
            string name = getValidString("  Enter product name: ");
            Product* found = mgr.findProductByName(name);
            if (found) { cout << "\n  [FOUND]\n"; found->display(); }
            else cout << "  [NOT FOUND] No product named '" << name << "'.\n";
        } else if (choice == 7) {
            string cat = getValidString("  Enter category: ");
            mgr.listProductsByCategory(cat);
        } else if (choice == 8) {
            mgr.displayAllSuppliers();
        } else if (choice == 9) {
            mgr.displayDashboard();
        } else if (choice == 10) {
            mgr.saveData();
        } else if (choice == 11) {
            logger.displayLog();
        } else if (choice == 0) {
            cout << "\n  [SYSTEM] Auto-saving data before exit...\n";
            mgr.saveData();
            running = false;
        }
    }
}


// ============================================================
//  MAIN ENTRY POINT
// ============================================================
int main() {
    cout << "============================================================\n";
    cout << "     AGRICULTURAL SUPPLY CHAIN SYSTEM v4.0 | 1,000 Kenyan Datasets\n";
    cout << "     ICS 2276 | Milestone 4: Modular Architecture
//  ** UPDATED: 1000 Realistic Kenyan Datasets **\n";
    cout << "============================================================\n\n";

    logger.info("=== System started ===");

    try {
        Warehouse warehouse(301, "Nakuru National Warehouse", 500000);
        InventoryManager mgr(warehouse);

        // Load 1,000 realistic Kenyan datasets
        mgr.loadKenyanData();
        // Load previously saved orders if they exist
        mgr.loadData();

        logger.info("System initialized with " + to_string(mgr.getProductCount()) + " products");
        cout << "[INIT] System initialized with " << mgr.getProductCount()
             << " products and " << mgr.getSupplierCount() << " suppliers.\n";

        mgr.displayDashboard();
        runSimulation(mgr);

    } catch (const SupplyChainException& e) {
        logger.error("Fatal error: " + string(e.what()));
        cout << "\n[FATAL ERROR] " << e.what() << "\n";
        return 1;
    } catch (const exception& e) {
        logger.error("Unexpected error: " + string(e.what()));
        cout << "\n[UNEXPECTED ERROR] " << e.what() << "\n";
        return 1;
    }

    logger.info("=== System shutdown ===");
    cout << "\n[SYSTEM] Shutting down. Goodbye!\n";
    return 0;
}
