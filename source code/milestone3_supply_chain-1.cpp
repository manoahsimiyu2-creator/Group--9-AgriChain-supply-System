// ============================================================
//  ICS 2276: Computer Programming II
//  Agricultural Supply Chain System
//  Milestone 3: Data Structures & OOP Modeling
//  ** UPDATED: 1000 Realistic Kenyan Datasets **
//
//  Dataset: 50 Suppliers, 200 Products, 750 Orders = 1000 total
//  CHANGES: Abstract Product, FreshProduce/ProcessedGood,
//  runtime polymorphism, arrays, recursion, string search
// ============================================================

#include <iostream>
#include <string>
#include <iomanip>
#include <algorithm>
#include "KenyanDataGenerator.h"

using namespace std;

const int MAX_PRODUCTS  = 200;
const int MAX_ORDERS    = 750;
const int MAX_SUPPLIERS =  50;

// ══════════════════════════════════════════════════════════
//  ABSTRACT BASE CLASS: Product
// ══════════════════════════════════════════════════════════
class Product {
protected:
    int productID; string name, category;
    double unitPrice; int quantityInStock, reorderLevel;
public:
    Product(int id,string n,string cat,double p,int q,int r)
        :productID(id),name(n),category(cat),
         unitPrice(p),quantityInStock(q),reorderLevel(r){}
    virtual ~Product(){}
    virtual string getProductType() const=0;
    virtual string getStorageInfo() const=0;
    virtual void   display()        const=0;

    int    getID()           const{return productID;}
    string getName()         const{return name;}
    string getCategory()     const{return category;}
    double getUnitPrice()    const{return unitPrice;}
    int    getQuantity()     const{return quantityInStock;}
    int    getReorderLevel() const{return reorderLevel;}
    void   setUnitPrice(double p){unitPrice=p;}
    double getTotalValue()   const{return quantityInStock*unitPrice;}
    bool   isReorderNeeded() const{return quantityInStock<=reorderLevel;}

    bool addStock(int qty){
        if(qty<=0)return false;
        quantityInStock+=qty; return true;
    }
    bool addStock(int qty,const string& note){
        if(qty<=0)return false;
        quantityInStock+=qty;
        cout<<"  [LOG] Supplied by: "<<note<<"\n";
        return true;
    }
    bool dispatchStock(int qty){
        if(qty<=0||qty>quantityInStock)return false;
        quantityInStock-=qty; return true;
    }
    void displayBase() const{
        cout<<fixed<<setprecision(2);
        cout<<"    ID           : "<<productID<<"\n";
        cout<<"    Type         : "<<getProductType()<<"\n";
        cout<<"    Name         : "<<name<<"\n";
        cout<<"    Category     : "<<category<<"\n";
        cout<<"    Price        : KES "<<unitPrice<<" / kg\n";
        cout<<"    In Stock     : "<<quantityInStock<<" kg\n";
        cout<<"    Reorder Level: "<<reorderLevel<<" kg\n";
        cout<<"    Total Value  : KES "<<getTotalValue()<<"\n";
        cout<<"    Storage Info : "<<getStorageInfo()<<"\n";
        cout<<"    Alert        : "<<(isReorderNeeded()?"*** REORDER ***":"OK")<<"\n";
    }
};

// ══════════════════════════════════════════════════════════
//  FreshProduce
// ══════════════════════════════════════════════════════════
class FreshProduce:public Product{
    int shelfLifeDays; double storageTemp;
public:
    FreshProduce(int id,string n,string cat,double p,
                 int q,int r,int shelf,double temp)
        :Product(id,n,cat,p,q,r),shelfLifeDays(shelf),storageTemp(temp){}
    string getProductType()const override{return "Fresh Produce";}
    string getStorageInfo() const override{
        return "Temp:"+to_string((int)storageTemp)+
               "C|Shelf:"+to_string(shelfLifeDays)+"days";
    }
    void display()const override{
        cout<<"  [FRESH]\n"; displayBase();
        cout<<"    Shelf Life   : "<<shelfLifeDays<<" days\n";
        cout<<"    Storage Temp : "<<storageTemp<<" C\n";
    }
};

// ══════════════════════════════════════════════════════════
//  ProcessedGood
// ══════════════════════════════════════════════════════════
class ProcessedGood:public Product{
    string packagingType; int expiryMonths;
public:
    ProcessedGood(int id,string n,string cat,double p,
                  int q,int r,string pack,int exp)
        :Product(id,n,cat,p,q,r),packagingType(pack),expiryMonths(exp){}
    string getProductType()const override{return "Processed Good";}
    string getStorageInfo() const override{
        return "Pack:"+packagingType+"|Exp:"+to_string(expiryMonths)+"mo";
    }
    void display()const override{
        cout<<"  [PROCESSED]\n"; displayBase();
        cout<<"    Packaging    : "<<packagingType<<"\n";
        cout<<"    Expiry       : "<<expiryMonths<<" months\n";
    }
};

// ══════════════════════════════════════════════════════════
//  Supplier
// ══════════════════════════════════════════════════════════
class Supplier{
    int supplierID; string name,location,contact; double rating;
public:
    Supplier():supplierID(0),name(""),location(""),contact(""),rating(0){}
    Supplier(int id,string n,string l,string c,double r)
        :supplierID(id),name(n),location(l),contact(c),rating(r){}
    int    getID()      const{return supplierID;}
    string getName()    const{return name;}
    string getLocation()const{return location;}
    double getRating()  const{return rating;}
    bool   isReliable() const{return rating>=7.0;}
    void display()const{
        cout<<"    ID       : "<<supplierID<<"\n";
        cout<<"    Name     : "<<name<<"\n";
        cout<<"    Location : "<<location<<"\n";
        cout<<"    Rating   : "<<rating<<"/10 "
            <<(isReliable()?"[OK]":"[UNRELIABLE]")<<"\n";
    }
};

// ══════════════════════════════════════════════════════════
//  Warehouse
// ══════════════════════════════════════════════════════════
class Warehouse{
    int warehouseID; string location; int capacity;
public:
    Warehouse(int id,string loc,int cap)
        :warehouseID(id),location(loc),capacity(cap){}
    int    getCapacity()const{return capacity;}
    bool   canAccommodate(int stock,int incoming)const{
        return(stock+incoming)<=capacity;
    }
    int    availableSpace(int stock)const{return capacity-stock;}
    double getUsagePct(int stock)const{
        return(static_cast<double>(stock)/capacity)*100.0;
    }
    void display(int stock)const{
        cout<<fixed<<setprecision(2);
        cout<<"    ID       : "<<warehouseID<<"\n";
        cout<<"    Location : "<<location<<"\n";
        cout<<"    Capacity : "<<capacity<<" kg\n";
        cout<<"    In Use   : "<<stock<<" kg ("<<getUsagePct(stock)<<"%)\n";
        cout<<"    Free     : "<<availableSpace(stock)<<" kg\n";
    }
};

// ══════════════════════════════════════════════════════════
//  Order
// ══════════════════════════════════════════════════════════
class Order{
public:
    static const int PENDING=0,APPROVED=1,REJECTED=2;
private:
    int orderID,productID,quantity,status;
    string productName,reason;
    double totalCost;
public:
    Order():orderID(0),productID(0),quantity(0),status(PENDING),
            productName(""),reason(""),totalCost(0){}
    Order(int oid,int pid,string pn,int qty,double pr)
        :orderID(oid),productID(pid),quantity(qty),status(PENDING),
         productName(pn),reason(""),totalCost(qty*pr){}
    Order(int oid,int pid,string pn,int qty,double pr,string st,string rsn)
        :orderID(oid),productID(pid),quantity(qty),
         productName(pn),reason(rsn),totalCost(qty*pr){
        if(st=="APPROVED")status=APPROVED;
        else if(st=="REJECTED")status=REJECTED;
        else status=PENDING;
    }
    int    getID()       const{return orderID;}
    int    getStatus()   const{return status;}
    double getTotalCost()const{return totalCost;}
    string getStatusLabel()const{
        if(status==APPROVED)return"APPROVED";
        if(status==REJECTED)return"REJECTED";
        return"PENDING";
    }
    void evaluate(const Supplier&s,const Warehouse&w,int stock){
        if(!s.isReliable()){status=REJECTED;reason="Supplier reliability below 7.0";return;}
        if(!w.canAccommodate(stock,quantity)){status=REJECTED;reason="Insufficient warehouse space";return;}
        status=APPROVED;reason="All checks passed";
    }
    void display()const{
        cout<<fixed<<setprecision(2);
        cout<<"    Order ID : "<<orderID<<"\n";
        cout<<"    Product  : "<<productName<<" (ID:"<<productID<<")\n";
        cout<<"    Quantity : "<<quantity<<" kg\n";
        cout<<"    Cost     : KES "<<totalCost<<"\n";
        cout<<"    Status   : "<<getStatusLabel()<<"\n";
        cout<<"    Reason   : "<<reason<<"\n";
    }
};

// ══════════════════════════════════════════════════════════
//  InventoryManager
// ══════════════════════════════════════════════════════════
class InventoryManager{
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
        :warehouse(w),productCount(0),orderCount(0),
         supplierCount(0),orderCounter(8001){}
    ~InventoryManager(){
        for(int i=0;i<productCount;i++) delete products[i];
    }

    void loadKenyanData(){
        for(const auto&s:KENYAN_SUPPLIERS){
            if(supplierCount>=MAX_SUPPLIERS)break;
            suppliers[supplierCount++]=Supplier(s.id,s.name,s.location,s.contact,s.rating);
        }
        for(const auto&p:KENYAN_PRODUCTS){
            if(productCount>=MAX_PRODUCTS)break;
            if(p.type=="Fresh")
                products[productCount++]=new FreshProduce(
                    p.id,p.name,p.category,p.unitPrice,
                    p.quantity,p.reorderLevel,p.shelfLifeDays,p.storageTemp);
            else
                products[productCount++]=new ProcessedGood(
                    p.id,p.name,p.category,p.unitPrice,
                    p.quantity,p.reorderLevel,p.packagingType,p.expiryMonths);
        }
        for(const auto&o:KENYAN_ORDERS){
            if(orderCount>=MAX_ORDERS)break;
            orders[orderCount++]=Order(o.orderID,o.productID,o.productName,
                o.quantity,o.unitPrice,o.status,o.reason);
        }
        cout<<"[INIT] Loaded "<<supplierCount<<" suppliers, "
            <<productCount<<" products, "<<orderCount<<" orders.\n";
    }

    int getTotalStock(int i=0)const{
        if(i>=productCount)return 0;
        return products[i]->getQuantity()+getTotalStock(i+1);
    }
    double getTotalValue(int i=0)const{
        if(i>=productCount)return 0.0;
        return products[i]->getTotalValue()+getTotalValue(i+1);
    }

    void addStock(int idx,int qty){
        if(idx<0||idx>=productCount){cout<<"  [ERROR] Bad index.\n";return;}
        if(!warehouse.canAccommodate(getTotalStock(),qty)){
            cout<<"  [ERROR] Warehouse full. Free: "
                <<warehouse.availableSpace(getTotalStock())<<" kg\n";return;
        }
        products[idx]->addStock(qty);
        cout<<"  [OK] Added "<<qty<<" kg to "<<products[idx]->getName()<<"\n";
    }

    void dispatchStock(int idx,int qty){
        if(idx<0||idx>=productCount){cout<<"  [ERROR] Bad index.\n";return;}
        if(!products[idx]->dispatchStock(qty)){
            cout<<"  [ERROR] Only "<<products[idx]->getQuantity()<<" kg available.\n";return;
        }
        cout<<"  [OK] Dispatched "<<qty<<" kg from "<<products[idx]->getName()<<"\n";
        if(products[idx]->isReorderNeeded())
            cout<<"  *** REORDER ALERT: "<<products[idx]->getName()<<"!\n";
    }

    void placeOrder(int pidx,int sidx,int qty){
        if(pidx<0||pidx>=productCount||sidx<0||sidx>=supplierCount){
            cout<<"  [ERROR] Bad index.\n";return;
        }
        Product&p=*products[pidx];
        Supplier&s=suppliers[sidx];
        Order order(orderCounter++,p.getID(),p.getName(),qty,p.getUnitPrice());
        order.evaluate(s,warehouse,getTotalStock());
        if(orderCount<MAX_ORDERS)orders[orderCount++]=order;
        cout<<"\n  [ORDER RESULT]\n";order.display();
        if(order.getStatus()==Order::APPROVED){
            p.addStock(qty,s.getName());
            cout<<"  [OK] New stock: "<<p.getQuantity()<<" kg\n";
        }
    }

    Product* findByName(const string&name)const{
        for(int i=0;i<productCount;i++)
            if(products[i]->getName()==name)return products[i];
        return nullptr;
    }

    void listByCategory(const string&cat)const{
        bool found=false;
        cout<<"\n  Products in ["<<cat<<"]:\n";
        for(int i=0;i<productCount;i++){
            if(products[i]->getCategory()==cat){
                products[i]->display();
                cout<<"  "<<string(50,'-')<<"\n";
                found=true;
            }
        }
        if(!found)cout<<"  None found.\n";
    }

    void displayProducts(int start=0,int count=20)const{
        int end=min(start+count,productCount);
        cout<<"\n========================================\n";
        cout<<"  INVENTORY ["<<(start+1)<<" to "<<end<<" of "<<productCount<<"]\n";
        cout<<"========================================\n";
        for(int i=start;i<end;i++){
            cout<<"\n  ["<<i<<"] ";products[i]->display();
            cout<<"  "<<string(50,'-')<<"\n";
        }
    }

    void displaySummary()const{
        cout<<fixed<<setprecision(2);
        cout<<"\n========================================\n";
        cout<<"         SYSTEM DASHBOARD\n";
        cout<<"========================================\n";
        cout<<"  Products  : "<<productCount<<"\n";
        cout<<"  Suppliers : "<<supplierCount<<"\n";
        cout<<"  Orders    : "<<orderCount<<"\n";
        cout<<"  Total Stock : "<<getTotalStock()<<" kg\n";
        cout<<"  Total Value : KES "<<getTotalValue()<<"\n";
        cout<<"\n  [WAREHOUSE]\n";warehouse.display(getTotalStock());
        cout<<"\n  [REORDER ALERTS]\n";
        int alerts=0;
        for(int i=0;i<productCount;i++)
            if(products[i]->isReorderNeeded()){
                cout<<"  *** "<<products[i]->getName()
                    <<" ("<<products[i]->getQuantity()<<" kg)\n";
                alerts++;
            }
        if(!alerts)cout<<"  All stock OK.\n";
        cout<<"  Total Alerts: "<<alerts<<"\n";
        cout<<"========================================\n";
    }

    void displaySuppliers(int start=0,int count=10)const{
        int end=min(start+count,supplierCount);
        cout<<"\n  SUPPLIERS ["<<(start+1)<<" to "<<end<<" of "<<supplierCount<<"]\n";
        for(int i=start;i<end;i++){cout<<"\n  ["<<i<<"]\n";suppliers[i].display();}
    }

    void displayOrders(int start=0,int count=20)const{
        int end=min(start+count,orderCount);
        cout<<"\n  ORDERS ["<<(start+1)<<" to "<<end<<" of "<<orderCount<<"]\n";
        for(int i=start;i<end;i++){
            cout<<"\n  Order #"<<(i+1)<<":\n";orders[i].display();
            cout<<"  "<<string(40,'-')<<"\n";
        }
    }

    void displayOrderStats()const{
        int ap=0,rj=0; double val=0;
        for(int i=0;i<orderCount;i++){
            if(orders[i].getStatus()==Order::APPROVED){ap++;val+=orders[i].getTotalCost();}
            else if(orders[i].getStatus()==Order::REJECTED)rj++;
        }
        cout<<"\n  [ORDER STATISTICS]\n";
        cout<<"  Total    : "<<orderCount<<"\n";
        cout<<"  Approved : "<<ap<<"\n";
        cout<<"  Rejected : "<<rj<<"\n";
        cout<<fixed<<setprecision(2);
        cout<<"  Approved Value: KES "<<val<<"\n";
    }

    int getProductCount() const{return productCount;}
    int getSupplierCount()const{return supplierCount;}
    int getOrderCount()   const{return orderCount;}
};

// ── Helpers ───────────────────────────────────────────────
int getValidInt(const string&p,int mn,int mx){
    int v;
    while(true){
        cout<<p;cin>>v;
        if(cin.fail()||v<mn||v>mx){cin.clear();cin.ignore(1000,'\n');
            cout<<"  [ERROR] Enter "<<mn<<" to "<<mx<<".\n";}
        else{cin.ignore(1000,'\n');return v;}
    }
}
string getValidString(const string&p){
    string v;cout<<p;cin.ignore();getline(cin,v);return v;
}

void displayMenu(){
    cout<<"\n---- MENU ---------------------------------------------\n";
    cout<<"  1. Dashboard summary\n";
    cout<<"  2. Browse products (paginated)\n";
    cout<<"  3. Add stock\n";
    cout<<"  4. Dispatch stock\n";
    cout<<"  5. Place order\n";
    cout<<"  6. View orders\n";
    cout<<"  7. Order statistics\n";
    cout<<"  8. Search by name\n";
    cout<<"  9. Filter by category\n";
    cout<<"  10. Browse suppliers\n";
    cout<<"  0. Exit\n";
    cout<<"-------------------------------------------------------\n";
}

void runSimulation(InventoryManager&mgr){
    bool running=true;
    cout<<"\n[SIM] Ready with 1,000 Kenyan datasets.\n";
    while(running){
        displayMenu();
        int c=getValidInt("  Choice: ",0,10);
        if(c==1)mgr.displaySummary();
        else if(c==2){
            int s=getValidInt("  Start index: ",0,mgr.getProductCount()-1);
            mgr.displayProducts(s,20);
        }
        else if(c==3){
            int idx=getValidInt("  Product index: ",0,mgr.getProductCount()-1);
            int qty=getValidInt("  Quantity (kg): ",1,999999);
            mgr.addStock(idx,qty);
        }
        else if(c==4){
            int idx=getValidInt("  Product index: ",0,mgr.getProductCount()-1);
            int qty=getValidInt("  Quantity (kg): ",1,999999);
            mgr.dispatchStock(idx,qty);
        }
        else if(c==5){
            int p=getValidInt("  Product index (0-199): ",0,mgr.getProductCount()-1);
            int s=getValidInt("  Supplier index (0-49): ",0,mgr.getSupplierCount()-1);
            int q=getValidInt("  Order qty (kg): ",1,999999);
            mgr.placeOrder(p,s,q);
        }
        else if(c==6){
            int s=getValidInt("  Start order: ",0,mgr.getOrderCount()-1);
            mgr.displayOrders(s,20);
        }
        else if(c==7)mgr.displayOrderStats();
        else if(c==8){
            string n=getValidString("  Product name: ");
            Product*p=mgr.findByName(n);
            if(p){cout<<"\n[FOUND]\n";p->display();}
            else cout<<"  Not found.\n";
        }
        else if(c==9){
            string cat=getValidString("  Category: ");
            mgr.listByCategory(cat);
        }
        else if(c==10){
            int s=getValidInt("  Start supplier: ",0,mgr.getSupplierCount()-1);
            mgr.displaySuppliers(s,10);
        }
        else if(c==0)running=false;
    }
}

int main(){
    cout<<"============================================================\n";
    cout<<"  AGRICULTURAL SUPPLY CHAIN SYSTEM v3.0\n";
    cout<<"  Milestone 3 | 1,000 Kenyan Datasets\n";
    cout<<"============================================================\n\n";
    Warehouse warehouse(301,"Nakuru National Warehouse",500000);
    InventoryManager mgr(warehouse);
    cout<<"[INIT] Loading 1,000 Kenyan datasets...\n";
    mgr.loadKenyanData();
    mgr.displaySummary();
    runSimulation(mgr);
    cout<<"\n[SYSTEM] Goodbye!\n";
    return 0;
}
