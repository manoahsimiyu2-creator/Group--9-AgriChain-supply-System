class Product {
private:
int quantityInStock; // hidden from outside
public:
bool addStock(int qty) {
if (qty <= 0) return false; // validation built in
quantityInStock += qty;
return true;
}
bool dispatchStock(int qty) {
if (qty > quantityInStock) return false; // prevents negative stock
quantityInStock -= qty;
return true;
}
};
