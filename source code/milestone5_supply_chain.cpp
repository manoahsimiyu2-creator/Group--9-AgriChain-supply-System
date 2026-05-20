// ============================================================
//  ICS 2276: Computer Programming II
//  Agricultural Supply Chain System
//  Milestone 5: Concurrency & Advanced Computation
//
//  CHANGES FROM MILESTONE 4:
//  - Enumerations: ProductStatus, OrderStatus, ShipmentStatus
//    replace raw int constants for structured state management
//  - Generics (templates): GenericStorage<T> reusable container
//    replaces raw arrays for type-safe storage
//  - Multithreading: ShipmentTracker runs shipments on
//    separate threads concurrently
//  - Synchronization: mutex locks protect shared inventory
//    data from race conditions
//  - Lambdas: used for sorting, filtering, and reporting
//  - Functional programming: filter/sort inventory by criteria
//  - All M4 features (exceptions, file I/O, logging) retained
// ============================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <ctime>
#include <thread>
#include <mutex>
#include <vector>
#include <algorithm>
#include <functional>
#include "KenyanDataGenerator.h"
#include <chrono>

using namespace std;


// ============================================================
//  CONSTANTS
// ============================================================
const int MAX_PRODUCTS  = 200;
const int MAX_SUPPLIERS =  50;
const int MAX_SHIPMENTS = 20;

const string INVENTORY_FILE = "inventory.csv";
const string ORDERS_FILE    = "orders.csv";
const string LOG_FILE       = "system.log";


// ============================================================
//  MODULE 1: ENUMERATIONS
//  Replaces raw int constants with named, type-safe states
// ============================================================

// Product availability state
enum class ProductStatus {
    IN_STOCK,
    LOW_STOCK,
    OUT_OF_STOCK,
    REORDER_NEEDED
};

// Order lifecycle states
enum class OrderStatus {
    PENDING,
    APPROVED,
    REJECTED,
    FULFILLED,
    CANCELLED
};

// Shipment tracking states
enum class ShipmentStatus {
    PREPARING,
    IN_TRANSIT,
    ARRIVED,
    DELIVERED,
    DELAYED
};

// Helper: convert enums to strings
string productStatusToString(ProductStatus s) {
    switch(s) {
        case ProductStatus::IN_STOCK:      return "IN STOCK";
        case ProductStatus::LOW_STOCK:     return "LOW STOCK";
        case ProductStatus::OUT_OF_STOCK:  return "OUT OF STOCK";
        case ProductStatus::REORDER_NEEDED:return "REORDER NEEDED";
        default: return "UNKNOWN";
    }
}

string orderStatusToString(OrderStatus s) {
    switch(s) {
        case OrderStatus::PENDING:   return "PENDING";
        case OrderStatus::APPROVED:  return "APPROVED";
        case OrderStatus::REJECTED:  return "REJECTED";
        case OrderStatus::FULFILLED: return "FULFILLED";
        case OrderStatus::CANCELLED: return "CANCELLED";
        default: return "UNKNOWN";
    }
}

string shipmentStatusToString(ShipmentStatus s) {
    switch(s) {
        case ShipmentStatus::PREPARING:  return "PREPARING";
        case ShipmentStatus::IN_TRANSIT: return "IN TRANSIT";
        case ShipmentStatus::ARRIVED:    return "ARRIVED";
        case ShipmentStatus::DELIVERED:  return "DELIVERED";
        case ShipmentStatus::DELAYED:    return "DELAYED";
        default: return "UNKNOWN";
    }
}


// ============================================================
//  MODULE 2: CUSTOM EXCEPTIONS (carried from M4)
// ============================================================
class SupplyChainException : public exception {
protected:
    string message;
public:
    SupplyChainException(const string& msg) : message(msg) {}
    const char* what() const noexcept override { return message.c_str(); }
};

class InsufficientStockException : public SupplyChainException {
public:
    InsufficientStockException(const string& product, int requested, int available)
        : SupplyChainException("Insufficient stock for '" + product +
                               "'. Requested: " + to_string(requested) +
                               ", Available: " + to_string(available) + " kg") {}
};

class WarehouseCapacityException : public SupplyChainException {
public:
    WarehouseCapacityException(int incoming, int available)
        : SupplyChainException("Warehouse capacity exceeded. Incoming: " +
                               to_string(incoming) + ", Space: " +
                               to_string(available) + " kg") {}
};

class InvalidEntityException : public SupplyChainException {
public:
    InvalidEntityException(const string& entity, int id)
        : SupplyChainException("Invalid " + entity + ": " + to_string(id)) {}
};

class InvalidDataException : public SupplyChainException {
public:
    InvalidDataException(const string& detail)
        : SupplyChainException("Invalid data: " + detail) {}
};

class FileIOException : public SupplyChainException {
public:
    FileIOException(const string& file, const string& op)
        : SupplyChainException("File " + op + " failed: " + file) {}
};


// ============================================================
//  MODULE 3: INTERFACES (carried from M4)
// ============================================================
class IStorable {
public:
    virtual string toCSV()                   const = 0;
    virtual void   fromCSV(const string& line)     = 0;
    virtual ~IStorable() {}
};

class IDisplayable {
public:
    virtual void display() const = 0;
    virtual ~IDisplayable() {}
};


// ============================================================
//  MODULE 4: FILE LOGGER (carried from M4)
// ============================================================
class FileLogger {
private:
    string logFile;
    mutex  logMutex;  // thread-safe logging

    string getTimestamp() const {
        time_t now = time(0);
        char buf[20];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
        return string(buf);
    }

public:
    FileLogger(const string& filename) : logFile(filename) {}

    void log(const string& level, const string& message) {
        lock_guard<mutex> lock(logMutex);  // thread-safe
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
        cout << "  SYSTEM LOG\n";
        cout << "============================================================\n";
        ifstream file(logFile);
        if (!file.is_open()) { cout << "  [No log found]\n"; return; }
        string line;
        while (getline(file, line)) cout << "  " << line << "\n";
        file.close();
        cout << "============================================================\n";
    }
};

FileLogger logger(LOG_FILE);


// ============================================================
//  MODULE 5: GENERIC STORAGE CONTAINER (Template/Generics)
//  Reusable type-safe container replacing raw arrays
// ============================================================
template <typename T>
class GenericStorage {
private:
    vector<T> items;
    int       maxSize;
    string    storageName;

public:
    GenericStorage(const string& name, int maxSz)
        : storageName(name), maxSize(maxSz) {}

    bool add(const T& item) {
        if ((int)items.size() >= maxSize) {
            cout << "  [WARN] " << storageName << " storage is full.\n";
            return false;
        }
        items.push_back(item);
        return true;
    }

    T& get(int index) {
        if (index < 0 || index >= (int)items.size())
            throw InvalidEntityException(storageName + " index", index);
        return items[index];
    }

    const T& get(int index) const {
        if (index < 0 || index >= (int)items.size())
            throw InvalidEntityException(storageName + " index", index);
        return items[index];
    }

    int  size()  const { return (int)items.size(); }
    bool empty() const { return items.empty(); }

    // Lambda-powered filter: returns items matching a predicate
    vector<T> filter(function<bool(const T&)> predicate) const {
        vector<T> result;
        for (const auto& item : items) {
            if (predicate(item)) result.push_back(item);
        }
        return result;
    }

    // Lambda-powered sort: sorts items by a comparator
    void sortBy(function<bool(const T&, const T&)> comparator) {
        sort(items.begin(), items.end(), comparator);
    }

    // Lambda-powered forEach
    void forEach(function<void(const T&)> action) const {
        for (const auto& item : items) action(item);
    }

    vector<T>& getAll() { return items; }
    const vector<T>& getAll() const { return items; }
};


// ============================================================
//  MODULE 6: PRODUCT CLASSES (evolved — uses ProductStatus enum)
// ============================================================
class Product : public IDisplayable {
protected:
    int           productID;
    string        name;
    string        category;
    double        unitPrice;
    int           quantityInStock;
    int           reorderLevel;
    ProductStatus status;

    void updateStatus() {
        if (quantityInStock == 0)
            status = ProductStatus::OUT_OF_STOCK;
        else if (quantityInStock <= reorderLevel)
            status = ProductStatus::REORDER_NEEDED;
        else if (quantityInStock <= reorderLevel * 2)
            status = ProductStatus::LOW_STOCK;
        else
            status = ProductStatus::IN_STOCK;
    }

public:
    Product() : productID(0), name(""), category(""), unitPrice(0),
                quantityInStock(0), reorderLevel(0),
                status(ProductStatus::OUT_OF_STOCK) {}

    Product(int id, string n, string cat, double price, int qty, int reorder)
        : productID(id), name(n), category(cat), unitPrice(price),
          quantityInStock(qty), reorderLevel(reorder) {
        updateStatus();
    }

    virtual ~Product() {}

    virtual string getProductType() const = 0;
    virtual string getStorageInfo() const = 0;
    virtual void   display()        const = 0;
    virtual string toCSV()          const = 0;

    int           getID()           const { return productID; }
    string        getName()         const { return name; }
    string        getCategory()     const { return category; }
    double        getUnitPrice()    const { return unitPrice; }
    int           getQuantity()     const { return quantityInStock; }
    int           getReorderLevel() const { return reorderLevel; }
    ProductStatus getStatus()       const { return status; }

    void setUnitPrice(double price) {
        if (price <= 0) throw InvalidDataException("Price must be positive");
        unitPrice = price;
    }

    double getTotalValue()   const { return quantityInStock * unitPrice; }
    bool   isReorderNeeded() const {
        return status == ProductStatus::REORDER_NEEDED ||
               status == ProductStatus::OUT_OF_STOCK;
    }

    void addStock(int qty) {
        if (qty <= 0) throw InvalidDataException("Quantity must be positive");
        quantityInStock += qty;
        updateStatus();
        logger.info("Added " + to_string(qty) + " kg to " + name +
                    " | Status: " + productStatusToString(status));
    }

    void addStock(int qty, const string& supplierNote) {
        addStock(qty);
        logger.info("Supplier: " + supplierNote);
    }

    void dispatchStock(int qty) {
        if (qty <= 0) throw InvalidDataException("Quantity must be positive");
        if (qty > quantityInStock)
            throw InsufficientStockException(name, qty, quantityInStock);
        quantityInStock -= qty;
        updateStatus();
        logger.info("Dispatched " + to_string(qty) + " kg from " + name +
                    " | Status: " + productStatusToString(status));
    }

    void displayBase() const {
        cout << fixed << setprecision(2);
        cout << "    ID           : " << productID                       << "\n";
        cout << "    Type         : " << getProductType()                << "\n";
        cout << "    Name         : " << name                            << "\n";
        cout << "    Category     : " << category                        << "\n";
        cout << "    Unit Price   : KES " << unitPrice                   << " / kg\n";
        cout << "    In Stock     : " << quantityInStock                 << " kg\n";
        cout << "    Reorder Level: " << reorderLevel                    << " kg\n";
        cout << "    Total Value  : KES " << getTotalValue()             << "\n";
        cout << "    Storage Info : " << getStorageInfo()                << "\n";
        cout << "    Status       : " << productStatusToString(status)   << "\n";
    }
};


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
               "C | Shelf: " + to_string(shelfLifeDays) + " days";
    }

    void display() const override {
        cout << "  [FRESH PRODUCE]\n";
        displayBase();
        cout << "    Shelf Life   : " << shelfLifeDays << " days\n";
        cout << "    Storage Temp : " << storageTemp   << " C\n";
    }

    string toCSV() const override {
        return "FreshProduce," + to_string(productID) + "," + name + "," +
               category + "," + to_string(unitPrice) + "," +
               to_string(quantityInStock) + "," + to_string(reorderLevel) +
               "," + to_string(shelfLifeDays) + "," + to_string((int)storageTemp);
    }

    int getShelfLife() const { return shelfLifeDays; }
};


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
        return "Pack: " + packagingType +
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
};


// ============================================================
//  MODULE 7: SUPPLIER (carried from M4)
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
//  MODULE 8: WAREHOUSE (carried from M4)
// ============================================================
class Warehouse {
private:
    int    warehouseID;
    string location;
    int    capacity;

public:
    Warehouse(int id, string loc, int cap)
        : warehouseID(id), location(loc), capacity(cap) {}

    int    getID()       const { return warehouseID; }
    string getLocation() const { return location; }
    int    getCapacity() const { return capacity; }

    void canAccommodate(int currentStock, int incoming) const {
        int space = capacity - currentStock;
        if (incoming > space)
            throw WarehouseCapacityException(incoming, space);
    }

    int availableSpace(int currentStock) const { return capacity - currentStock; }

    double getUsagePercentage(int currentStock) const {
        return (static_cast<double>(currentStock) / capacity) * 100.0;
    }

    void display(int currentStock) const {
        cout << fixed << setprecision(2);
        cout << "    ID         : " << warehouseID << "\n";
        cout << "    Location   : " << location    << "\n";
        cout << "    Capacity   : " << capacity    << " kg\n";
        cout << "    In Use     : " << currentStock << " kg ("
             << getUsagePercentage(currentStock) << "%)\n";
        cout << "    Free Space : " << availableSpace(currentStock) << " kg\n";
    }
};


// ============================================================
//  MODULE 9: ORDER (evolved — uses OrderStatus enum)
// ============================================================
class Order : public IStorable, public IDisplayable {
private:
    int         orderID;
    int         productID;
    string      productName;
    int         quantity;
    double      totalCost;
    OrderStatus status;
    string      reason;

public:
    Order() : orderID(0), productID(0), productName(""),
              quantity(0), totalCost(0.0),
              status(OrderStatus::PENDING), reason("") {}

    Order(int oid, int pid, string pname, int qty, double unitPrice)
        : orderID(oid), productID(pid), productName(pname),
          quantity(qty), totalCost(qty * unitPrice),
          status(OrderStatus::PENDING), reason("") {}

    int         getID()        const { return orderID; }
    int         getProductID() const { return productID; }
    int         getQuantity()  const { return quantity; }
    double      getTotalCost() const { return totalCost; }
    OrderStatus getStatus()    const { return status; }
    string      getReason()    const { return reason; }

    void evaluate(const Supplier& supplier, const Warehouse& warehouse, int currentStock) {
        try {
            if (!supplier.isReliable())
                throw InvalidDataException("Supplier reliability below 7.0");
            warehouse.canAccommodate(currentStock, quantity);
            status = OrderStatus::APPROVED;
            reason = "All checks passed";
            logger.info("Order " + to_string(orderID) + " APPROVED");
        } catch (const SupplyChainException& e) {
            status = OrderStatus::REJECTED;
            reason = e.what();
            logger.warning("Order " + to_string(orderID) + " REJECTED: " + reason);
        }
    }

    void fulfil() { status = OrderStatus::FULFILLED; }
    void cancel() { status = OrderStatus::CANCELLED; }

    void display() const override {
        cout << fixed << setprecision(2);
        cout << "    Order ID   : " << orderID                        << "\n";
        cout << "    Product    : " << productName                    << "\n";
        cout << "    Quantity   : " << quantity                       << " kg\n";
        cout << "    Total Cost : KES " << totalCost                  << "\n";
        cout << "    Status     : " << orderStatusToString(status)    << "\n";
        cout << "    Reason     : " << reason                         << "\n";
    }

    string toCSV() const override {
        return to_string(orderID) + "," + to_string(productID) + "," +
               productName + "," + to_string(quantity) + "," +
               to_string(totalCost) + "," + orderStatusToString(status);
    }

    void fromCSV(const string& line) override {
        stringstream ss(line);
        string token;
        getline(ss, token, ','); orderID     = stoi(token);
        getline(ss, token, ','); productID   = stoi(token);
        getline(ss, productName, ',');
        getline(ss, token, ','); quantity    = stoi(token);
        getline(ss, token, ','); totalCost   = stod(token);
        getline(ss, token, ',');
        if (token == "APPROVED")  status = OrderStatus::APPROVED;
        else if (token == "REJECTED") status = OrderStatus::REJECTED;
        else if (token == "FULFILLED") status = OrderStatus::FULFILLED;
        else status = OrderStatus::PENDING;
    }
};


// ============================================================
//  MODULE 10: SHIPMENT (NEW in M5)
//  Runs on its own thread — simulates concurrent transit
// ============================================================
class Shipment {
private:
    int            shipmentID;
    string         origin;
    string         destination;
    string         productName;
    int            quantity;
    ShipmentStatus status;
    int            transitTimeSeconds; // simulated transit time

public:
    Shipment() : shipmentID(0), origin(""), destination(""),
                 productName(""), quantity(0),
                 status(ShipmentStatus::PREPARING), transitTimeSeconds(3) {}

    Shipment(int id, string orig, string dest, string product,
             int qty, int transitSecs = 3)
        : shipmentID(id), origin(orig), destination(dest),
          productName(product), quantity(qty),
          status(ShipmentStatus::PREPARING),
          transitTimeSeconds(transitSecs) {}

    int            getID()       const { return shipmentID; }
    string         getProduct()  const { return productName; }
    int            getQuantity() const { return quantity; }
    ShipmentStatus getStatus()   const { return status; }

    // Run on a separate thread — simulates shipment lifecycle
    void track(mutex& consoleMutex) {
        // PREPARING
        {
            lock_guard<mutex> lock(consoleMutex);
            cout << "  [SHIPMENT-" << shipmentID << "] " << productName
                 << " (" << quantity << " kg): PREPARING...\n";
        }
        logger.info("Shipment " + to_string(shipmentID) + " PREPARING");
        this_thread::sleep_for(chrono::seconds(1));

        // IN TRANSIT
        status = ShipmentStatus::IN_TRANSIT;
        {
            lock_guard<mutex> lock(consoleMutex);
            cout << "  [SHIPMENT-" << shipmentID << "] " << productName
                 << ": IN TRANSIT from " << origin << " to " << destination << "\n";
        }
        logger.info("Shipment " + to_string(shipmentID) + " IN TRANSIT");
        this_thread::sleep_for(chrono::seconds(transitTimeSeconds));

        // ARRIVED
        status = ShipmentStatus::ARRIVED;
        {
            lock_guard<mutex> lock(consoleMutex);
            cout << "  [SHIPMENT-" << shipmentID << "] " << productName
                 << ": ARRIVED at " << destination << "\n";
        }

        // DELIVERED
        status = ShipmentStatus::DELIVERED;
        {
            lock_guard<mutex> lock(consoleMutex);
            cout << "  [SHIPMENT-" << shipmentID << "] " << productName
                 << ": DELIVERED successfully!\n";
        }
        logger.info("Shipment " + to_string(shipmentID) + " DELIVERED");
    }

    void display() const {
        cout << "    Shipment ID  : " << shipmentID                       << "\n";
        cout << "    Product      : " << productName                      << "\n";
        cout << "    Quantity     : " << quantity                         << " kg\n";
        cout << "    Route        : " << origin << " -> " << destination  << "\n";
        cout << "    Status       : " << shipmentStatusToString(status)   << "\n";
    }
};


// ============================================================
//  MODULE 11: SHIPMENT TRACKER (Multithreading)
//  Manages concurrent shipment threads
// ============================================================
class ShipmentTracker {
private:
    vector<Shipment> shipments;
    vector<thread>   threads;
    mutex            consoleMutex;  // protects console output
    mutex            dataMutex;     // protects shipment data
    int              shipmentCounter;

public:
    ShipmentTracker() : shipmentCounter(6001) {}

    // Add a new shipment
    void addShipment(const string& origin, const string& dest,
                     const string& product, int qty) {
        lock_guard<mutex> lock(dataMutex);
        shipments.emplace_back(shipmentCounter++, origin, dest, product, qty, 2);
        cout << "  [OK] Shipment created for " << qty << " kg of " << product << "\n";
    }

    // Launch ALL pending shipments concurrently on separate threads
    void dispatchAll() {
        cout << "\n  [TRACKER] Dispatching " << shipments.size()
             << " shipment(s) concurrently...\n";
        cout << "  " << string(50, '-') << "\n";

        for (auto& shipment : shipments) {
            // Each shipment runs on its own thread
            threads.emplace_back([&shipment, this]() {
                shipment.track(consoleMutex);
            });
        }

        // Wait for all threads to finish
        for (auto& t : threads) {
            if (t.joinable()) t.join();
        }
        threads.clear();

        cout << "  " << string(50, '-') << "\n";
        cout << "  [TRACKER] All shipments completed.\n";
        logger.info("All shipments dispatched and completed");
    }

    void displayAll() const {
        cout << "\n============================================================\n";
        cout << "  SHIPMENT TRACKER (" << shipments.size() << " shipments)\n";
        cout << "============================================================\n";
        if (shipments.empty()) { cout << "  No shipments yet.\n"; return; }
        for (const auto& s : shipments) {
            s.display();
            cout << "  " << string(50, '-') << "\n";
        }
    }

    int count() const { return (int)shipments.size(); }
};


// ============================================================
//  MODULE 12: INVENTORY MANAGER (evolved — uses GenericStorage
//  and lambda-powered filtering/sorting)
// ============================================================
class InventoryManager {
private:
    GenericStorage<Product*> products;
    GenericStorage<Supplier> suppliers;
    GenericStorage<Order>    orders;
    Warehouse                warehouse;
    ShipmentTracker          tracker;
    int                      orderCounter;
    mutex                    inventoryMutex;  // protects stock operations

public:
    InventoryManager(Warehouse w)
        : products("Products", MAX_PRODUCTS),
          suppliers("Suppliers", MAX_SUPPLIERS),
          orders("Orders", 50),
          warehouse(w),
          orderCounter(8001) {}

    ~InventoryManager() {
        for (int i = 0; i < products.size(); i++) delete products.get(i);
    }

    bool addProduct(Product* p) { return products.add(p); }
    bool addSupplier(Supplier s) { return suppliers.add(s); }

    int getTotalStock(int index = 0) const {
        if (index >= products.size()) return 0;
        return products.get(index)->getQuantity() + getTotalStock(index + 1);
    }

    double getTotalValue(int index = 0) const {
        if (index >= products.size()) return 0.0;
        return products.get(index)->getTotalValue() + getTotalValue(index + 1);
    }

    // Thread-safe add stock
    void addStock(int idx, int qty) {
        lock_guard<mutex> lock(inventoryMutex);
        try {
            if (idx < 0 || idx >= products.size())
                throw InvalidEntityException("product index", idx);
            warehouse.canAccommodate(getTotalStock(), qty);
            products.get(idx)->addStock(qty);
            cout << "  [OK] Added " << qty << " kg to "
                 << products.get(idx)->getName() << "\n";
        } catch (const SupplyChainException& e) {
            logger.error(e.what());
            cout << "  [ERROR] " << e.what() << "\n";
        }
    }

    // Thread-safe dispatch stock
    void dispatchStock(int idx, int qty) {
        lock_guard<mutex> lock(inventoryMutex);
        try {
            if (idx < 0 || idx >= products.size())
                throw InvalidEntityException("product index", idx);
            products.get(idx)->dispatchStock(qty);
            cout << "  [OK] Dispatched " << qty << " kg from "
                 << products.get(idx)->getName() << "\n";
            if (products.get(idx)->isReorderNeeded())
                cout << "  *** REORDER ALERT for "
                     << products.get(idx)->getName() << "!\n";
        } catch (const SupplyChainException& e) {
            logger.error(e.what());
            cout << "  [ERROR] " << e.what() << "\n";
        }
    }

    void placeOrder(int pidx, int sidx, int qty) {
        try {
            if (pidx < 0 || pidx >= products.size())
                throw InvalidEntityException("product index", pidx);
            if (sidx < 0 || sidx >= suppliers.size())
                throw InvalidEntityException("supplier index", sidx);

            Product& p  = *products.get(pidx);
            Supplier& s = suppliers.get(sidx);

            Order order(orderCounter++, p.getID(), p.getName(), qty, p.getUnitPrice());
            order.evaluate(s, warehouse, getTotalStock());
            orders.add(order);

            cout << "\n  [ORDER RESULT]\n";
            order.display();

            if (order.getStatus() == OrderStatus::APPROVED) {
                p.addStock(qty, s.getName());
                cout << "  [OK] Stock updated: " << p.getQuantity() << " kg\n";
            }
        } catch (const SupplyChainException& e) {
            logger.error(e.what());
            cout << "  [ERROR] " << e.what() << "\n";
        }
    }

    // ---- LAMBDA-POWERED OPERATIONS ----

    // Filter products by status using a lambda
    void showProductsByStatus(ProductStatus targetStatus) {
        cout << "\n  Products with status ["
             << productStatusToString(targetStatus) << "]:\n";

        // Lambda: filter by status
        auto filtered = products.filter([targetStatus](Product* const& p) {
            return p->getStatus() == targetStatus;
        });

        if (filtered.empty()) { cout << "  None found.\n"; return; }
        for (auto* p : filtered) {
            p->display();
            cout << "  " << string(50, '-') << "\n";
        }
    }

    // Sort products by value descending using a lambda
    void sortProductsByValue() {
        products.sortBy([](Product* const& a, Product* const& b) {
            return a->getTotalValue() > b->getTotalValue();
        });
        cout << "  [OK] Products sorted by total value (highest first).\n";
        logger.info("Products sorted by value");
    }

    // Sort products by stock ascending using a lambda
    void sortProductsByStock() {
        products.sortBy([](Product* const& a, Product* const& b) {
            return a->getQuantity() < b->getQuantity();
        });
        cout << "  [OK] Products sorted by stock (lowest first).\n";
    }

    // Generate inventory report using lambda forEach
    void generateReport() {
        cout << "\n============================================================\n";
        cout << "  INVENTORY REPORT\n";
        cout << "============================================================\n";
        cout << fixed << setprecision(2);

        double grandTotal = 0;
        int    totalUnits = 0;

        // Lambda: accumulate totals and print each product
        products.forEach([&grandTotal, &totalUnits](Product* const& p) {
            cout << "  " << p->getName() << " | "
                 << p->getQuantity() << " kg | KES "
                 << p->getTotalValue() << " | "
                 << productStatusToString(p->getStatus()) << "\n";
            grandTotal += p->getTotalValue();
            totalUnits += p->getQuantity();
        });

        cout << "  " << string(50, '-') << "\n";
        cout << "  TOTAL UNITS : " << totalUnits << " kg\n";
        cout << "  TOTAL VALUE : KES " << grandTotal << "\n";
        cout << "============================================================\n";
    }

    // ---- SHIPMENT OPERATIONS ----
    void createShipment(const string& origin, const string& dest,
                        const string& product, int qty) {
        tracker.addShipment(origin, dest, product, qty);
    }

    void dispatchShipments() {
        if (tracker.count() == 0) {
            cout << "  [INFO] No shipments to dispatch.\n";
            return;
        }
        tracker.dispatchAll();
    }

    void viewShipments() { tracker.displayAll(); }

    // ---- FILE PERSISTENCE ----

    void loadKenyanData() {
        for (const auto& s : KENYAN_SUPPLIERS) {
            if (suppliers.size() >= (size_t)MAX_SUPPLIERS) break;
            suppliers.add(Supplier(s.id, s.name, s.location, s.contact, s.rating));
        }
        for (const auto& p : KENYAN_PRODUCTS) {
            if (products.size() >= (size_t)MAX_PRODUCTS) break;
            if (p.type == "Fresh")
                products.add(new FreshProduce(p.id, p.name, p.category,
                    p.unitPrice, p.quantity, p.reorderLevel,
                    p.shelfLifeDays, p.storageTemp));
            else
                products.add(new ProcessedGood(p.id, p.name, p.category,
                    p.unitPrice, p.quantity, p.reorderLevel,
                    p.packagingType, p.expiryMonths));
        }
        for (const auto& o : KENYAN_ORDERS) {
            if (orders.size() >= 750) break;
            Order ord(o.orderID, o.productID, o.productName,
                      o.quantity, o.unitPrice);
            orders.add(ord);
        }
        logger.info("Loaded " + to_string(products.size()) +
                    " products, " + to_string(suppliers.size()) + " suppliers");
        cout << "[INIT] Loaded " << suppliers.size() << " suppliers, "
             << products.size() << " products, "
             << orders.size() << " orders.\n";
    }

    void saveData() {
        try {
            ofstream inv(INVENTORY_FILE);
            if (!inv.is_open()) throw FileIOException(INVENTORY_FILE, "write");
            inv << "type,id,name,category,price,qty,reorder,extra1,extra2\n";
            products.forEach([&inv](Product* const& p) {
                inv << p->toCSV() << "\n";
            });
            inv.close();

            ofstream ord(ORDERS_FILE);
            if (!ord.is_open()) throw FileIOException(ORDERS_FILE, "write");
            ord << "orderID,productID,name,qty,cost,status\n";
            orders.forEach([&ord](const Order& o) {
                ord << o.toCSV() << "\n";
            });
            ord.close();

            logger.info("Data saved to files");
            cout << "  [OK] Data saved to " << INVENTORY_FILE
                 << " and " << ORDERS_FILE << "\n";
        } catch (const FileIOException& e) {
            logger.error(e.what());
            cout << "  [ERROR] " << e.what() << "\n";
        }
    }

    void loadData() {
        ifstream ord(ORDERS_FILE);
        if (!ord.is_open()) { logger.warning("No orders file found."); return; }
        string line;
        getline(ord, line); // skip header
        int loaded = 0;
        while (getline(ord, line)) {
            if (!line.empty()) {
                Order o;
                o.fromCSV(line);
                orders.add(o);
                loaded++;
            }
        }
        ord.close();
        if (loaded > 0) cout << "  [OK] Loaded " << loaded << " orders.\n";
    }

    // ---- DISPLAY ----
    void displayAllProducts() const {
        cout << "\n============================================================\n";
        cout << "  PRODUCT INVENTORY (" << products.size() << " products)\n";
        cout << "============================================================\n";
        for (int i = 0; i < products.size(); i++) {
            cout << "\n  [" << i << "] ";
            products.get(i)->display();
            cout << "  " << string(50, '-') << "\n";
        }
        cout << "  Total Stock : " << getTotalStock() << " kg\n";
        cout << "  Total Value : KES " << fixed << setprecision(2)
             << getTotalValue() << "\n";
    }

    void displayAllOrders() const {
        cout << "\n============================================================\n";
        cout << "  ORDER HISTORY (" << orders.size() << " orders)\n";
        cout << "============================================================\n";
        if (orders.empty()) { cout << "  No orders yet.\n"; return; }
        for (int i = 0; i < orders.size(); i++) {
            cout << "\n  Order #" << (i+1) << ":\n";
            orders.get(i).display();
            cout << "  " << string(50,'-') << "\n";
        }
    }

    void displayAllSuppliers() const {
        cout << "\n============================================================\n";
        cout << "  SUPPLIERS (" << suppliers.size() << ")\n";
        cout << "============================================================\n";
        for (int i = 0; i < suppliers.size(); i++) {
            cout << "\n  [" << i << "]\n";
            suppliers.get(i).display();
        }
    }

    void displayDashboard() const {
        cout << "\n============================================================\n";
        cout << "                    SYSTEM DASHBOARD v5.0\n";
        cout << "============================================================\n";
        cout << fixed << setprecision(2);
        cout << "  Products         : " << products.size()  << "\n";
        cout << "  Suppliers        : " << suppliers.size() << "\n";
        cout << "  Orders           : " << orders.size()    << "\n";
        cout << "  Active Shipments : " << tracker.count()  << "\n";
        cout << "  Total Stock      : " << getTotalStock()  << " kg\n";
        cout << "  Total Value      : KES " << getTotalValue() << "\n";
        cout << "\n  [WAREHOUSE]\n";
        warehouse.display(getTotalStock());
        cout << "\n  [REORDER ALERTS]\n";
        bool any = false;
        for (int i = 0; i < products.size(); i++) {
            if (products.get(i)->isReorderNeeded()) {
                cout << "  *** " << products.get(i)->getName()
                     << " [" << productStatusToString(products.get(i)->getStatus()) << "]\n";
                any = true;
            }
        }
        if (!any) cout << "  All stock levels sufficient.\n";
        cout << "============================================================\n";
    }

    Product* findProductByName(const string& name) const {
        for (int i = 0; i < products.size(); i++)
            if (products.get(i)->getName() == name) return products.get(i);
        return nullptr;
    }

    int getProductCount()  const { return products.size(); }
    int getSupplierCount() const { return suppliers.size(); }
    Product* getProduct(int i)  { return products.get(i); }
    Supplier& getSupplier(int i) { return suppliers.get(i); }
};


// ============================================================
//  INPUT VALIDATION (carried from M4)
// ============================================================
int getValidInt(const string& prompt, int minVal, int maxVal) {
    int value;
    while (true) {
        cout << prompt;
        cin >> value;
        if (cin.fail() || value < minVal || value > maxVal) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "  [ERROR] Enter " << minVal << " to " << maxVal << ".\n";
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
    cout << "  1.  View all products\n";
    cout << "  2.  Add stock\n";
    cout << "  3.  Dispatch stock\n";
    cout << "  4.  Place supply order\n";
    cout << "  5.  View order history\n";
    cout << "  6.  View dashboard\n";
    cout << "  7.  Filter products by status\n";
    cout << "  8.  Sort products by value\n";
    cout << "  9.  Generate inventory report (lambda)\n";
    cout << "  10. Create shipment\n";
    cout << "  11. Dispatch all shipments (concurrent threads)\n";
    cout << "  12. View shipments\n";
    cout << "  13. Save data\n";
    cout << "  14. View system log\n";
    cout << "  0.  Exit\n";
    cout << "-------------------------------------------------------\n";
}

void runSimulation(InventoryManager& mgr) {
    bool running = true;
    cout << "\n[SIM] Concurrent simulation started.\n";

    while (running) {
        displayMenu();
        int choice = getValidInt("  Enter choice: ", 0, 14);

        if (choice == 1) {
            mgr.displayAllProducts();
        } else if (choice == 2) {
            mgr.displayAllProducts();
            int idx = getValidInt("  Product index: ", 0, mgr.getProductCount()-1);
            int qty = getValidInt("  Quantity (kg): ", 1, 99999);
            mgr.addStock(idx, qty);
        } else if (choice == 3) {
            mgr.displayAllProducts();
            int idx = getValidInt("  Product index: ", 0, mgr.getProductCount()-1);
            int qty = getValidInt("  Quantity (kg): ", 1, 99999);
            mgr.dispatchStock(idx, qty);
        } else if (choice == 4) {
            mgr.displayAllProducts();
            int pidx = getValidInt("  Product index: ", 0, mgr.getProductCount()-1);
            mgr.displayAllSuppliers();
            int sidx = getValidInt("  Supplier index: ", 0, mgr.getSupplierCount()-1);
            int qty  = getValidInt("  Order quantity (kg): ", 1, 99999);
            mgr.placeOrder(pidx, sidx, qty);
        } else if (choice == 5) {
            mgr.displayAllOrders();
        } else if (choice == 6) {
            mgr.displayDashboard();
        } else if (choice == 7) {
            cout << "  Status options:\n";
            cout << "  0=IN_STOCK  1=LOW_STOCK  2=OUT_OF_STOCK  3=REORDER_NEEDED\n";
            int s = getValidInt("  Select status: ", 0, 3);
            mgr.showProductsByStatus(static_cast<ProductStatus>(s));
        } else if (choice == 8) {
            mgr.sortProductsByValue();
            mgr.displayAllProducts();
        } else if (choice == 9) {
            mgr.generateReport();
        } else if (choice == 10) {
            string origin = getValidString("  Enter origin: ");
            string dest   = getValidString("  Enter destination: ");
            string prod   = getValidString("  Enter product name: ");
            int qty       = getValidInt("  Quantity (kg): ", 1, 99999);
            mgr.createShipment(origin, dest, prod, qty);
        } else if (choice == 11) {
            mgr.dispatchShipments();
        } else if (choice == 12) {
            mgr.viewShipments();
        } else if (choice == 13) {
            mgr.saveData();
        } else if (choice == 14) {
            logger.displayLog();
        } else if (choice == 0) {
            cout << "\n  [SYSTEM] Auto-saving before exit...\n";
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
    cout << "     AGRICULTURAL SUPPLY CHAIN SYSTEM v5.0 | 1,000 Kenyan Datasets\n";
    cout << "     ICS 2276 | Milestone 5: Concurrency & Advanced Computation\n";
    cout << "============================================================\n\n";

    logger.info("=== System v5.0 started ===");

    try {
        Warehouse        warehouse(301, "Nakuru Central Store", 5000);
        InventoryManager mgr(warehouse);

        // Load 1,000 realistic Kenyan datasets
        mgr.loadKenyanData();
        mgr.loadData();

        cout << "[INIT] System initialized: " << mgr.getProductCount()
             << " products, " << mgr.getSupplierCount() << " suppliers.\n";

        mgr.displayDashboard();
        runSimulation(mgr);

    } catch (const SupplyChainException& e) {
        logger.error("Fatal: " + string(e.what()));
        cout << "\n[FATAL] " << e.what() << "\n";
        return 1;
    } catch (const exception& e) {
        logger.error("Unexpected: " + string(e.what()));
        cout << "\n[ERROR] " << e.what() << "\n";
        return 1;
    }

    logger.info("=== System shutdown ===");
    cout << "\n[SYSTEM] Goodbye!\n";
    return 0;
}
