// ============================================================
//  ICS 2276: Computer Programming II
//  Agricultural Supply Chain System
//  Milestone 3: Data Structures & OOP Modeling
//
//  CHANGES FROM MILESTONE 2:
//  - Product is now an ABSTRACT BASE CLASS with pure virtual
//    methods (abstraction)
//  - FreshProduce and ProcessedGood INHERIT from Product
//    (inheritance hierarchy)
//  - Polymorphism: display() and getStorageInfo() are virtual
//  - Arrays of Product pointers to manage multiple products
//  - Array of Orders to maintain order history
//  - Array of Suppliers to manage multiple suppliers
//  - String handling: search products by name/category
//  - Method overloading: addStock() overloaded variants
//  - Recursive utility: calculate total inventory value
// ============================================================

#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

// ============================================================
//  CONSTANTS
// ============================================================
const int MAX_PRODUCTS  = 20;
const int MAX_ORDERS    = 50;
const int MAX_SUPPLIERS = 10;


// ============================================================
//  ABSTRACT BASE CLASS: Product
//  Defines the interface all product types must implement
// ============================================================
class Product {
protected:
    int    productID;
    string name;
    string category;
    double unitPrice;
    int    quantityInStock;
    int    reorderLevel;

public:
    // --- Constructor ---
    Product(int id, string n, string cat, double price, int qty, int reorder)
        : productID(id), name(n), category(cat),
          unitPrice(price), quantityInStock(qty), reorderLevel(reorder) {}

    // --- Virtual Destructor (required for polymorphism) ---
    virtual ~Product() {}

    // --- Pure Virtual Methods (must be implemented by subclasses) ---
    virtual string getProductType()  const = 0;
    virtual string getStorageInfo()  const = 0;
    virtual void   display()         const = 0;

    // --- Getters (shared by all subclasses) ---
    int    getID()           const { return productID; }
    string getName()         const { return name; }
    string getCategory()     const { return category; }
    double getUnitPrice()    const { return unitPrice; }
    int    getQuantity()     const { return quantityInStock; }
    int    getReorderLevel() const { return reorderLevel; }

    // --- Setters ---
    void setUnitPrice(double price) { unitPrice = price; }

    // --- Computed ---
    double getTotalValue()   const { return quantityInStock * unitPrice; }
    bool   isReorderNeeded() const { return quantityInStock <= reorderLevel; }

    // --- Stock Operations (Method Overloading) ---
    // Overload 1: add stock by quantity only
    bool addStock(int qty) {
        if (qty <= 0) return false;
        quantityInStock += qty;
        return true;
    }

    // Overload 2: add stock with a supplier note (logs who supplied)
    bool addStock(int qty, const string& supplierNote) {
        if (qty <= 0) return false;
        quantityInStock += qty;
        cout << "  [LOG] Stock added from: " << supplierNote << "\n";
        return true;
    }

    bool dispatchStock(int qty) {
        if (qty <= 0 || qty > quantityInStock) return false;
        quantityInStock -= qty;
        return true;
    }

    // --- Shared display helper ---
    void displayBase() const {
        cout << fixed << setprecision(2);
        cout << "    ID           : " << productID              << "\n";
        cout << "    Type         : " << getProductType()       << "\n";
        cout << "    Name         : " << name                   << "\n";
        cout << "    Category     : " << category               << "\n";
        cout << "    Unit Price   : KES " << unitPrice          << " / kg\n";
        cout << "    In Stock     : " << quantityInStock        << " kg\n";
        cout << "    Reorder Level: " << reorderLevel           << " kg\n";
        cout << "    Total Value  : KES " << getTotalValue()    << "\n";
        cout << "    Storage Info : " << getStorageInfo()       << "\n";
        cout << "    Reorder Alert: " << (isReorderNeeded() ? "*** YES ***" : "No") << "\n";
    }
};


// ============================================================
//  SUBCLASS: FreshProduce (inherits Product)
//  Represents perishable items: vegetables, fruits, dairy
// ============================================================
class FreshProduce : public Product {
private:
    int    shelfLifeDays;     // how many days before expiry
    double storageTemp;       // required storage temperature (°C)

public:
    FreshProduce(int id, string n, string cat, double price,
                 int qty, int reorder, int shelfLife, double temp)
        : Product(id, n, cat, price, qty, reorder),
          shelfLifeDays(shelfLife), storageTemp(temp) {}

    // --- Implement pure virtual methods ---
    string getProductType() const override {
        return "Fresh Produce";
    }

    string getStorageInfo() const override {
        return "Temp: " + to_string((int)storageTemp) + "C | Shelf Life: "
               + to_string(shelfLifeDays) + " days";
    }

    void display() const override {
        cout << "  [FRESH PRODUCE]\n";
        displayBase();
        cout << "    Shelf Life   : " << shelfLifeDays << " days\n";
        cout << "    Storage Temp : " << storageTemp   << " °C\n";
    }

    // --- Fresh-produce specific ---
    bool isExpiringSoon(int daysThreshold) const {
        return shelfLifeDays <= daysThreshold;
    }

    int getShelfLife() const { return shelfLifeDays; }
};


// ============================================================
//  SUBCLASS: ProcessedGood (inherits Product)
//  Represents non-perishable / processed items: grain, flour
// ============================================================
class ProcessedGood : public Product {
private:
    string packagingType;   // e.g., "Sack", "Crate", "Box"
    int    expiryMonths;    // months before expiry

public:
    ProcessedGood(int id, string n, string cat, double price,
                  int qty, int reorder, string packaging, int expiry)
        : Product(id, n, cat, price, qty, reorder),
          packagingType(packaging), expiryMonths(expiry) {}

    // --- Implement pure virtual methods ---
    string getProductType() const override {
        return "Processed Good";
    }

    string getStorageInfo() const override {
        return "Packaging: " + packagingType
               + " | Expiry: " + to_string(expiryMonths) + " months";
    }

    void display() const override {
        cout << "  [PROCESSED GOOD]\n";
        displayBase();
        cout << "    Packaging    : " << packagingType << "\n";
        cout << "    Expiry       : " << expiryMonths  << " months\n";
    }

    string getPackaging() const { return packagingType; }
};


// ============================================================
//  CLASS: Supplier (carried from M2, unchanged)
// ============================================================
class Supplier {
private:
    int    supplierID;
    string name;
    string location;
    string contact;
    double reliabilityRating;

public:
    Supplier() : supplierID(0), name(""), location(""), contact(""), reliabilityRating(0.0) {}

    Supplier(int id, string n, string loc, string con, double rating)
        : supplierID(id), name(n), location(loc),
          contact(con), reliabilityRating(rating) {}

    int    getID()       const { return supplierID; }
    string getName()     const { return name; }
    string getLocation() const { return location; }
    double getRating()   const { return reliabilityRating; }
    bool   isReliable()  const { return reliabilityRating >= 7.0; }

    void display() const {
        cout << "    ID         : " << supplierID        << "\n";
        cout << "    Name       : " << name              << "\n";
        cout << "    Location   : " << location          << "\n";
        cout << "    Contact    : " << contact           << "\n";
        cout << "    Reliability: " << reliabilityRating << "/10 "
             << (isReliable() ? "[APPROVED]" : "[UNRELIABLE]") << "\n";
    }
};


// ============================================================
//  CLASS: Warehouse (carried from M2, unchanged)
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

    bool canAccommodate(int currentStock, int incoming) const {
        return (currentStock + incoming) <= capacity;
    }

    int availableSpace(int currentStock) const {
        return capacity - currentStock;
    }

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
//  CLASS: Order (evolved from M2 — stores product name too)
// ============================================================
class Order {
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
    Order() : orderID(0), productID(0), productName(""), quantity(0),
              totalCost(0.0), status(PENDING), reason("") {}

    Order(int oid, int pid, string pname, int qty, double unitPrice)
        : orderID(oid), productID(pid), productName(pname),
          quantity(qty), totalCost(qty * unitPrice),
          status(PENDING), reason("") {}

    int    getID()          const { return orderID; }
    int    getProductID()   const { return productID; }
    int    getQuantity()    const { return quantity; }
    double getTotalCost()   const { return totalCost; }
    int    getStatus()      const { return status; }

    string getStatusLabel() const {
        if (status == APPROVED) return "APPROVED";
        if (status == REJECTED) return "REJECTED";
        return "PENDING";
    }

    void evaluate(const Supplier& supplier, const Warehouse& warehouse, int currentStock) {
        if (!supplier.isReliable()) {
            status = REJECTED;
            reason = "Supplier reliability below threshold";
            return;
        }
        if (!warehouse.canAccommodate(currentStock, quantity)) {
            status = REJECTED;
            reason = "Insufficient warehouse space";
            return;
        }
        status = APPROVED;
        reason = "All checks passed";
    }

    void display() const {
        cout << fixed << setprecision(2);
        cout << "    Order ID   : " << orderID          << "\n";
        cout << "    Product    : " << productName      << " (ID:" << productID << ")\n";
        cout << "    Quantity   : " << quantity         << " kg\n";
        cout << "    Total Cost : KES " << totalCost    << "\n";
        cout << "    Status     : " << getStatusLabel() << "\n";
        cout << "    Reason     : " << reason           << "\n";
    }
};


// ============================================================
//  INVENTORY MANAGER
//  Uses arrays of Product pointers (polymorphism in action)
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
          orderCount(0), supplierCount(0), orderCounter(3001) {}

    ~InventoryManager() {
        for (int i = 0; i < productCount; i++) {
            delete products[i];
        }
    }

    // --- Add entities ---
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

    // --- Get total stock across all products (recursive) ---
    int getTotalStock(int index = 0) const {
        if (index >= productCount) return 0;
        return products[index]->getQuantity() + getTotalStock(index + 1);
    }

    // --- Get total inventory value across all products (recursive) ---
    double getTotalValue(int index = 0) const {
        if (index >= productCount) return 0.0;
        return products[index]->getTotalValue() + getTotalValue(index + 1);
    }

    // --- Search by name (string handling) ---
    Product* findProductByName(const string& searchName) const {
        for (int i = 0; i < productCount; i++) {
            if (products[i]->getName() == searchName) return products[i];
        }
        return nullptr;
    }

    // --- Search by category ---
    void listProductsByCategory(const string& cat) const {
        bool found = false;
        cout << "\n  Products in category [" << cat << "]:\n";
        for (int i = 0; i < productCount; i++) {
            if (products[i]->getCategory() == cat) {
                products[i]->display();
                cout << "  " << string(50, '-') << "\n";
                found = true;
            }
        }
        if (!found) cout << "  No products found in this category.\n";
    }

    // --- Find supplier by ID ---
    Supplier* findSupplierByID(int id) {
        for (int i = 0; i < supplierCount; i++) {
            if (suppliers[i].getID() == id) return &suppliers[i];
        }
        return nullptr;
    }

    // --- Place order ---
    void placeOrder(int productIndex, int supplierID, int qty) {
        if (productIndex < 0 || productIndex >= productCount) {
            cout << "  [ERROR] Invalid product index.\n";
            return;
        }
        Supplier* s = findSupplierByID(supplierID);
        if (!s) {
            cout << "  [ERROR] Supplier not found.\n";
            return;
        }

        Product* p = products[productIndex];
        Order order(orderCounter++, p->getID(), p->getName(), qty, p->getUnitPrice());
        order.evaluate(*s, warehouse, getTotalStock());

        if (orderCount < MAX_ORDERS) {
            orders[orderCount++] = order;
        }

        cout << "\n  [ORDER RESULT]\n";
        order.display();

        if (order.getStatus() == Order::APPROVED) {
            p->addStock(qty, s->getName());
            cout << "  [OK] Stock updated. New stock: " << p->getQuantity() << " kg\n";
        }
    }

    // --- Display all products (polymorphism: each type displays differently) ---
    void displayAllProducts() const {
        cout << "\n============================================================\n";
        cout << "  PRODUCT INVENTORY (" << productCount << " products)\n";
        cout << "============================================================\n";
        for (int i = 0; i < productCount; i++) {
            cout << "\n  [" << i << "] ";
            products[i]->display();   // polymorphic call
            cout << "  " << string(50, '-') << "\n";
        }
        cout << "\n  Total Stock : " << getTotalStock() << " kg (recursive)\n";
        cout << "  Total Value : KES " << fixed << setprecision(2)
             << getTotalValue() << " (recursive)\n";
    }

    // --- Display all orders ---
    void displayAllOrders() const {
        cout << "\n============================================================\n";
        cout << "  ORDER HISTORY (" << orderCount << " orders)\n";
        cout << "============================================================\n";
        if (orderCount == 0) {
            cout << "  No orders placed yet.\n";
            return;
        }
        for (int i = 0; i < orderCount; i++) {
            cout << "\n  Order #" << (i + 1) << ":\n";
            orders[i].display();
            cout << "  " << string(50, '-') << "\n";
        }
    }

    // --- Display all suppliers ---
    void displayAllSuppliers() const {
        cout << "\n============================================================\n";
        cout << "  SUPPLIERS (" << supplierCount << ")\n";
        cout << "============================================================\n";
        for (int i = 0; i < supplierCount; i++) {
            cout << "\n  [" << i << "]\n";
            suppliers[i].display();
        }
    }

    // --- Dashboard summary ---
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
        bool anyAlert = false;
        for (int i = 0; i < productCount; i++) {
            if (products[i]->isReorderNeeded()) {
                cout << "  *** " << products[i]->getName()
                     << " (ID:" << products[i]->getID() << ") needs reorder!\n";
                anyAlert = true;
            }
        }
        if (!anyAlert) cout << "  All stock levels are sufficient.\n";
        cout << "============================================================\n";
    }

    // --- Getters for simulation ---
    int getProductCount()  const { return productCount; }
    int getSupplierCount() const { return supplierCount; }
    Product* getProduct(int i) { return (i >= 0 && i < productCount) ? products[i] : nullptr; }
    Supplier* getSupplier(int i) { return (i >= 0 && i < supplierCount) ? &suppliers[i] : nullptr; }
};


// ============================================================
//  INPUT VALIDATION HELPERS (carried from M2)
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
//  MENU
// ============================================================
void displayMenu() {
    cout << "\n---- MAIN MENU ----------------------------------------\n";
    cout << "  1. View all products (inventory)\n";
    cout << "  2. Add stock to a product\n";
    cout << "  3. Dispatch stock from a product\n";
    cout << "  4. Place a supply order\n";
    cout << "  5. View order history\n";
    cout << "  6. Search product by name\n";
    cout << "  7. List products by category\n";
    cout << "  8. View all suppliers\n";
    cout << "  9. View dashboard\n";
    cout << "  0. Exit\n";
    cout << "-------------------------------------------------------\n";
}


// ============================================================
//  SIMULATION LOOP
// ============================================================
void runSimulation(InventoryManager& mgr) {
    bool running = true;
    cout << "\n[SIM] Multi-product simulation started.\n";

    while (running) {
        displayMenu();
        int choice = getValidInt("  Enter choice: ", 0, 9);

        if (choice == 1) {
            mgr.displayAllProducts();

        } else if (choice == 2) {
            mgr.displayAllProducts();
            int idx = getValidInt("  Select product index: ", 0, mgr.getProductCount() - 1);
            int qty = getValidInt("  Enter quantity to add (kg): ", 1, 99999);
            if (mgr.getProduct(idx)->addStock(qty)) {
                cout << "  [OK] Added " << qty << " kg to "
                     << mgr.getProduct(idx)->getName() << "\n";
            }

        } else if (choice == 3) {
            mgr.displayAllProducts();
            int idx = getValidInt("  Select product index: ", 0, mgr.getProductCount() - 1);
            int qty = getValidInt("  Enter quantity to dispatch (kg): ", 1, 99999);
            if (mgr.getProduct(idx)->dispatchStock(qty)) {
                cout << "  [OK] Dispatched " << qty << " kg from "
                     << mgr.getProduct(idx)->getName() << "\n";
                if (mgr.getProduct(idx)->isReorderNeeded())
                    cout << "  *** REORDER ALERT for " << mgr.getProduct(idx)->getName() << "!\n";
            } else {
                cout << "  [ERROR] Insufficient stock.\n";
            }

        } else if (choice == 4) {
            mgr.displayAllProducts();
            int pidx = getValidInt("  Select product index: ", 0, mgr.getProductCount() - 1);
            mgr.displayAllSuppliers();
            int sidx = getValidInt("  Select supplier index: ", 0, mgr.getSupplierCount() - 1);
            int qty  = getValidInt("  Enter order quantity (kg): ", 1, 99999);
            mgr.placeOrder(pidx, mgr.getSupplier(sidx)->getID(), qty);

        } else if (choice == 5) {
            mgr.displayAllOrders();

        } else if (choice == 6) {
            string name = getValidString("  Enter product name to search: ");
            Product* found = mgr.findProductByName(name);
            if (found) {
                cout << "\n  [FOUND]\n";
                found->display();
            } else {
                cout << "  [NOT FOUND] No product named '" << name << "'.\n";
            }

        } else if (choice == 7) {
            string cat = getValidString("  Enter category to filter: ");
            mgr.listProductsByCategory(cat);

        } else if (choice == 8) {
            mgr.displayAllSuppliers();

        } else if (choice == 9) {
            mgr.displayDashboard();

        } else if (choice == 0) {
            running = false;
        }
    }
}


// ============================================================
//  MAIN ENTRY POINT
// ============================================================
int main() {
    cout << "============================================================\n";
    cout << "     AGRICULTURAL SUPPLY CHAIN SYSTEM v3.0\n";
    cout << "     ICS 2276 | Milestone 3: Data Structures & OOP Modeling\n";
    cout << "============================================================\n\n";

    // --- Warehouse ---
    Warehouse warehouse(301, "Nakuru Central Store", 5000);

    // --- Inventory Manager ---
    InventoryManager mgr(warehouse);

    // --- Suppliers ---
    mgr.addSupplier(Supplier(501, "Rift Valley Farms Ltd",  "Nakuru, Kenya",  "+254700000001", 8.5));
    mgr.addSupplier(Supplier(502, "Coastal Agro Supplies",  "Mombasa, Kenya", "+254711000002", 6.0));
    mgr.addSupplier(Supplier(503, "Highland Fresh Produce", "Eldoret, Kenya", "+254722000003", 9.2));

    // --- Products: Polymorphic array of Product pointers ---
    // FreshProduce(id, name, category, price, qty, reorder, shelfLife, temp)
    mgr.addProduct(new FreshProduce(1001, "Tomatoes",  "Vegetable", 80.00, 200, 50,  7,  10.0));
    mgr.addProduct(new FreshProduce(1002, "Bananas",   "Fruit",     35.00, 150, 40,  14, 13.0));
    mgr.addProduct(new FreshProduce(1003, "Milk",      "Dairy",    120.00,  80, 30,  3,   4.0));

    // ProcessedGood(id, name, category, price, qty, reorder, packaging, expiryMonths)
    mgr.addProduct(new ProcessedGood(1004, "Maize",        "Grain",      45.50, 320, 100, "Sack",  12));
    mgr.addProduct(new ProcessedGood(1005, "Wheat Flour",  "Grain",      60.00, 250,  80, "Sack",  9));
    mgr.addProduct(new ProcessedGood(1006, "Cooking Oil",  "Processed", 250.00,  60,  20, "Crate", 18));

    cout << "[INIT] System initialized with " << mgr.getProductCount()
         << " products and " << mgr.getSupplierCount() << " suppliers.\n";

    // --- Show dashboard ---
    mgr.displayDashboard();

    // --- Run simulation ---
    runSimulation(mgr);

    cout << "\n[SYSTEM] Shutting down Agricultural Supply Chain System. Goodbye!\n";
    return 0;
}
