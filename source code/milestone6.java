// ============================================================
//  ICS 2276: Computer Programming II
//  Agricultural Supply Chain System
//  Milestone 6: Interface Integration & Capstone System
//
//  CHANGES FROM MILESTONE 5:
//  - Full Java Swing GUI replaces console interface
//  - Event-driven interaction model (ActionListeners)
//  - Tabbed interface: Dashboard, Inventory, Orders,
//    Suppliers, Shipments, Reports, System Log
//  - All system components integrated into one GUI
//  - Real-time simulation output panel
//  - Visual reorder alerts with color coding
//  - Complete integration of all M1-M5 features
// ============================================================

import javax.swing.*;
import javax.swing.border.*;
import javax.swing.table.*;
import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.*;
import java.util.List;
import java.util.concurrent.*;
import java.util.stream.*;
import java.util.function.*;


// ============================================================
//  ENUMERATIONS (from M5)
// ============================================================
enum ProductStatus  { IN_STOCK, LOW_STOCK, OUT_OF_STOCK, REORDER_NEEDED }
enum OrderStatus    { PENDING, APPROVED, REJECTED, FULFILLED, CANCELLED }
enum ShipmentStatus { PREPARING, IN_TRANSIT, ARRIVED, DELIVERED, DELAYED }


// ============================================================
//  CUSTOM EXCEPTIONS (from M4)
// ============================================================
class SupplyChainException extends Exception {
    public SupplyChainException(String msg) { super(msg); }
}
class InsufficientStockException extends SupplyChainException {
    public InsufficientStockException(String p, int req, int avail) {
        super("Insufficient stock for '" + p + "'. Requested: " + req + ", Available: " + avail + " kg");
    }
}
class WarehouseCapacityException extends SupplyChainException {
    public WarehouseCapacityException(int incoming, int space) {
        super("Warehouse capacity exceeded. Incoming: " + incoming + " kg, Space: " + space + " kg");
    }
}
class InvalidDataException extends SupplyChainException {
    public InvalidDataException(String detail) { super("Invalid data: " + detail); }
}


// ============================================================
//  PRODUCT HIERARCHY (from M3/M5)
// ============================================================
abstract class Product {
    protected int           productID;
    protected String        name;
    protected String        category;
    protected double        unitPrice;
    protected int           quantityInStock;
    protected int           reorderLevel;
    protected ProductStatus status;

    public Product(int id, String n, String cat, double price, int qty, int reorder) {
        this.productID       = id;
        this.name            = n;
        this.category        = cat;
        this.unitPrice       = price;
        this.quantityInStock = qty;
        this.reorderLevel    = reorder;
        updateStatus();
    }

    protected void updateStatus() {
        if      (quantityInStock == 0)               status = ProductStatus.OUT_OF_STOCK;
        else if (quantityInStock <= reorderLevel)    status = ProductStatus.REORDER_NEEDED;
        else if (quantityInStock <= reorderLevel * 2)status = ProductStatus.LOW_STOCK;
        else                                         status = ProductStatus.IN_STOCK;
    }

    public abstract String getProductType();
    public abstract String getStorageInfo();

    public int           getID()           { return productID; }
    public String        getName()         { return name; }
    public String        getCategory()     { return category; }
    public double        getUnitPrice()    { return unitPrice; }
    public int           getQuantity()     { return quantityInStock; }
    public int           getReorderLevel() { return reorderLevel; }
    public ProductStatus getStatus()       { return status; }
    public double        getTotalValue()   { return quantityInStock * unitPrice; }
    public boolean       isReorderNeeded(){ return status == ProductStatus.REORDER_NEEDED || status == ProductStatus.OUT_OF_STOCK; }

    public void setUnitPrice(double price) throws InvalidDataException {
        if (price <= 0) throw new InvalidDataException("Price must be positive");
        this.unitPrice = price;
        updateStatus();
    }

    public void addStock(int qty) throws InvalidDataException {
        if (qty <= 0) throw new InvalidDataException("Quantity must be positive");
        quantityInStock += qty;
        updateStatus();
    }

    public void dispatchStock(int qty) throws InsufficientStockException, InvalidDataException {
        if (qty <= 0) throw new InvalidDataException("Quantity must be positive");
        if (qty > quantityInStock) throw new InsufficientStockException(name, qty, quantityInStock);
        quantityInStock -= qty;
        updateStatus();
    }

    public String toCSV() {
        return getProductType() + "," + productID + "," + name + "," +
               category + "," + unitPrice + "," + quantityInStock + "," + reorderLevel;
    }
}

class FreshProduce extends Product {
    private int    shelfLifeDays;
    private double storageTemp;

    public FreshProduce(int id, String n, String cat, double price,
                        int qty, int reorder, int shelfLife, double temp) {
        super(id, n, cat, price, qty, reorder);
        this.shelfLifeDays = shelfLife;
        this.storageTemp   = temp;
    }

    public String getProductType() { return "Fresh Produce"; }
    public String getStorageInfo() {
        return "Temp: " + (int)storageTemp + "°C | Shelf: " + shelfLifeDays + " days";
    }
    public int    getShelfLife()   { return shelfLifeDays; }
    public double getStorageTemp() { return storageTemp; }
}

class ProcessedGood extends Product {
    private String packagingType;
    private int    expiryMonths;

    public ProcessedGood(int id, String n, String cat, double price,
                         int qty, int reorder, String packaging, int expiry) {
        super(id, n, cat, price, qty, reorder);
        this.packagingType = packaging;
        this.expiryMonths  = expiry;
    }

    public String getProductType()  { return "Processed Good"; }
    public String getStorageInfo()  { return "Pack: " + packagingType + " | Expiry: " + expiryMonths + " months"; }
    public String getPackagingType(){ return packagingType; }
    public int    getExpiryMonths() { return expiryMonths; }
}


// ============================================================
//  SUPPLIER
// ============================================================
class Supplier {
    private int    supplierID;
    private String name;
    private String location;
    private String contact;
    private double reliabilityRating;

    public Supplier(int id, String n, String loc, String con, double rating) {
        this.supplierID        = id;
        this.name              = n;
        this.location          = loc;
        this.contact           = con;
        this.reliabilityRating = rating;
    }

    public int    getID()       { return supplierID; }
    public String getName()     { return name; }
    public String getLocation() { return location; }
    public String getContact()  { return contact; }
    public double getRating()   { return reliabilityRating; }
    public boolean isReliable() { return reliabilityRating >= 7.0; }
}


// ============================================================
//  ORDER
// ============================================================
class Order {
    private int         orderID;
    private String      productName;
    private int         quantity;
    private double      totalCost;
    private OrderStatus status;
    private String      reason;
    private String      timestamp;

    public Order(int oid, String pname, int qty, double unitPrice) {
        this.orderID     = oid;
        this.productName = pname;
        this.quantity    = qty;
        this.totalCost   = qty * unitPrice;
        this.status      = OrderStatus.PENDING;
        this.reason      = "";
        this.timestamp   = LocalDateTime.now().format(DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm"));
    }

    public void evaluate(Supplier supplier, int warehouseSpace) {
        if (!supplier.isReliable()) {
            status = OrderStatus.REJECTED;
            reason = "Supplier reliability below 7.0";
        } else if (quantity > warehouseSpace) {
            status = OrderStatus.REJECTED;
            reason = "Insufficient warehouse space";
        } else {
            status = OrderStatus.APPROVED;
            reason = "All checks passed";
        }
    }

    public int         getID()          { return orderID; }
    public String      getProductName() { return productName; }
    public int         getQuantity()    { return quantity; }
    public double      getTotalCost()   { return totalCost; }
    public OrderStatus getStatus()      { return status; }
    public String      getReason()      { return reason; }
    public String      getTimestamp()   { return timestamp; }
}


// ============================================================
//  SHIPMENT
// ============================================================
class Shipment {
    private int            shipmentID;
    private String         origin;
    private String         destination;
    private String         productName;
    private int            quantity;
    private ShipmentStatus status;

    public Shipment(int id, String orig, String dest, String product, int qty) {
        this.shipmentID   = id;
        this.origin       = orig;
        this.destination  = dest;
        this.productName  = product;
        this.quantity     = qty;
        this.status       = ShipmentStatus.PREPARING;
    }

    public int            getID()          { return shipmentID; }
    public String         getOrigin()      { return origin; }
    public String         getDestination() { return destination; }
    public String         getProductName() { return productName; }
    public int            getQuantity()    { return quantity; }
    public ShipmentStatus getStatus()      { return status; }
    public void           setStatus(ShipmentStatus s) { this.status = s; }
}


// ============================================================
//  SUPPLY CHAIN DATA MODEL
//  Central data store for the entire application
// ============================================================
class SupplyChainModel {
    private List<Product>  products  = new ArrayList<>();
    private List<Supplier> suppliers = new ArrayList<>();
    private List<Order>    orders    = new ArrayList<>();
    private List<Shipment> shipments = new ArrayList<>();
    private int warehouseCapacity    = 5000;
    private int orderCounter         = 5001;
    private int shipmentCounter      = 6001;

    public void initialize() {
        // Suppliers
        suppliers.add(new Supplier(501, "Rift Valley Farms Ltd",  "Nakuru",  "+254700000001", 8.5));
        suppliers.add(new Supplier(502, "Coastal Agro Supplies",  "Mombasa", "+254711000002", 6.0));
        suppliers.add(new Supplier(503, "Highland Fresh Produce", "Eldoret", "+254722000003", 9.2));

        // Products
        products.add(new FreshProduce(1001, "Tomatoes",    "Vegetable",  80.00, 200, 50,  7,  10.0));
        products.add(new FreshProduce(1002, "Bananas",     "Fruit",      35.00, 150, 40, 14,  13.0));
        products.add(new FreshProduce(1003, "Milk",        "Dairy",     120.00,  80, 30,  3,   4.0));
        products.add(new ProcessedGood(1004, "Maize",       "Grain",     45.50, 320, 100, "Sack",  12));
        products.add(new ProcessedGood(1005, "Wheat Flour", "Grain",     60.00, 250,  80, "Sack",   9));
        products.add(new ProcessedGood(1006, "Cooking Oil", "Processed",250.00,  60,  20, "Crate", 18));
    }

    public List<Product>  getProducts()  { return products; }
    public List<Supplier> getSuppliers() { return suppliers; }
    public List<Order>    getOrders()    { return orders; }
    public List<Shipment> getShipments() { return shipments; }

    public int getTotalStock() {
        return products.stream().mapToInt(Product::getQuantity).sum();
    }

    public double getTotalValue() {
        return products.stream().mapToDouble(Product::getTotalValue).sum();
    }

    public int getAvailableSpace() {
        return warehouseCapacity - getTotalStock();
    }

    public int getWarehouseCapacity() { return warehouseCapacity; }

    public List<Product> getReorderAlerts() {
        return products.stream().filter(Product::isReorderNeeded).collect(Collectors.toList());
    }

    public Order placeOrder(Product p, Supplier s, int qty) {
        Order order = new Order(orderCounter++, p.getName(), qty, p.getUnitPrice());
        order.evaluate(s, getAvailableSpace());
        orders.add(order);
        if (order.getStatus() == OrderStatus.APPROVED) {
            try { p.addStock(qty); } catch (Exception e) { /* handled in UI */ }
        }
        return order;
    }

    public Shipment createShipment(String origin, String dest, String product, int qty) {
        Shipment s = new Shipment(shipmentCounter++, origin, dest, product, qty);
        shipments.add(s);
        return s;
    }

    public void saveToFile() throws IOException {
        // Save inventory
        try (PrintWriter pw = new PrintWriter(new FileWriter("inventory.csv"))) {
            pw.println("type,id,name,category,price,qty,reorder,storageInfo");
            for (Product p : products) {
                pw.println(p.toCSV() + "," + p.getStorageInfo());
            }
        }
        // Save orders
        try (PrintWriter pw = new PrintWriter(new FileWriter("orders.csv"))) {
            pw.println("orderID,product,qty,cost,status,reason,timestamp");
            for (Order o : orders) {
                pw.printf("%d,%s,%d,%.2f,%s,%s,%s%n",
                    o.getID(), o.getProductName(), o.getQuantity(),
                    o.getTotalCost(), o.getStatus(), o.getReason(), o.getTimestamp());
            }
        }
    }
}


// ============================================================
//  MAIN GUI APPLICATION
// ============================================================
public class AgriculturalSupplyChainGUI extends JFrame {

    // --- Color palette ---
    private static final Color BG_DARK       = new Color(18, 32, 47);
    private static final Color BG_PANEL      = new Color(26, 45, 64);
    private static final Color BG_CARD       = new Color(34, 57, 82);
    private static final Color ACCENT_GREEN  = new Color(46, 204, 113);
    private static final Color ACCENT_BLUE   = new Color(52, 152, 219);
    private static final Color ACCENT_ORANGE = new Color(230, 126, 34);
    private static final Color ACCENT_RED    = new Color(231, 76, 60);
    private static final Color ACCENT_PURPLE = new Color(155, 89, 182);
    private static final Color TEXT_PRIMARY  = new Color(236, 240, 241);
    private static final Color TEXT_MUTED    = new Color(149, 165, 166);

    private SupplyChainModel model;
    private JTextArea        logArea;
    private JTabbedPane      tabbedPane;

    // Dashboard stat labels
    private JLabel lblTotalProducts, lblTotalStock, lblTotalValue,
                   lblAlerts, lblOrders, lblShipments;

    public AgriculturalSupplyChainGUI() {
        model = new SupplyChainModel();
        model.initialize();

        setTitle("Agricultural Supply Chain System v6.0 — ICS 2276");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setSize(1200, 750);
        setMinimumSize(new Dimension(900, 600));
        setLocationRelativeTo(null);
        getContentPane().setBackground(BG_DARK);

        initUI();
        refreshDashboard();
        log("System initialized with " + model.getProducts().size() +
            " products and " + model.getSuppliers().size() + " suppliers.");
        setVisible(true);
    }

    // ---- UI INITIALIZER ----
    private void initUI() {
        setLayout(new BorderLayout());

        // Header
        add(buildHeader(), BorderLayout.NORTH);

        // Tabbed pane
        tabbedPane = new JTabbedPane();
        tabbedPane.setBackground(BG_PANEL);
        tabbedPane.setForeground(TEXT_PRIMARY);
        tabbedPane.setFont(new Font("Segoe UI", Font.BOLD, 13));

        tabbedPane.addTab("📊 Dashboard",   buildDashboardTab());
        tabbedPane.addTab("📦 Inventory",   buildInventoryTab());
        tabbedPane.addTab("🛒 Orders",      buildOrdersTab());
        tabbedPane.addTab("🏭 Suppliers",   buildSuppliersTab());
        tabbedPane.addTab("🚚 Shipments",   buildShipmentsTab());
        tabbedPane.addTab("📋 Reports",     buildReportsTab());
        tabbedPane.addTab("📝 System Log",  buildLogTab());

        add(tabbedPane, BorderLayout.CENTER);

        // Status bar
        add(buildStatusBar(), BorderLayout.SOUTH);
    }

    // ---- HEADER ----
    private JPanel buildHeader() {
        JPanel header = new JPanel(new BorderLayout());
        header.setBackground(BG_CARD);
        header.setBorder(new EmptyBorder(12, 20, 12, 20));

        JLabel title = new JLabel("🌾 Agricultural Supply Chain System");
        title.setFont(new Font("Segoe UI", Font.BOLD, 20));
        title.setForeground(ACCENT_GREEN);

        JLabel subtitle = new JLabel("ICS 2276 | Milestone 6: Capstone System");
        subtitle.setFont(new Font("Segoe UI", Font.PLAIN, 12));
        subtitle.setForeground(TEXT_MUTED);

        JPanel titlePanel = new JPanel(new GridLayout(2, 1));
        titlePanel.setBackground(BG_CARD);
        titlePanel.add(title);
        titlePanel.add(subtitle);

        JButton btnSave = makeButton("💾 Save Data", ACCENT_BLUE);
        btnSave.addActionListener(e -> saveData());

        header.add(titlePanel, BorderLayout.WEST);
        header.add(btnSave, BorderLayout.EAST);
        return header;
    }

    // ---- DASHBOARD TAB ----
    private JPanel buildDashboardTab() {
        JPanel panel = new JPanel(new BorderLayout(10, 10));
        panel.setBackground(BG_DARK);
        panel.setBorder(new EmptyBorder(15, 15, 15, 15));

        // Stat cards row
        JPanel statsRow = new JPanel(new GridLayout(1, 6, 10, 0));
        statsRow.setBackground(BG_DARK);

        lblTotalProducts = buildStatCard(statsRow, "Products",  "0", ACCENT_BLUE);
        lblTotalStock    = buildStatCard(statsRow, "Total Stock","0 kg", ACCENT_GREEN);
        lblTotalValue    = buildStatCard(statsRow, "Total Value","KES 0", ACCENT_PURPLE);
        lblAlerts        = buildStatCard(statsRow, "Alerts",    "0", ACCENT_RED);
        lblOrders        = buildStatCard(statsRow, "Orders",    "0", ACCENT_ORANGE);
        lblShipments     = buildStatCard(statsRow, "Shipments", "0", ACCENT_BLUE);

        // Alerts panel
        JPanel alertsPanel = new JPanel(new BorderLayout());
        alertsPanel.setBackground(BG_PANEL);
        alertsPanel.setBorder(buildTitledBorder("⚠ Reorder Alerts"));

        DefaultListModel<String> alertModel = new DefaultListModel<>();
        JList<String> alertList = new JList<>(alertModel);
        alertList.setBackground(BG_PANEL);
        alertList.setForeground(ACCENT_ORANGE);
        alertList.setFont(new Font("Segoe UI", Font.PLAIN, 13));

        for (Product p : model.getReorderAlerts()) {
            alertModel.addElement("⚠  " + p.getName() + " — " +
                p.getQuantity() + " kg left [" + p.getStatus() + "]");
        }
        if (alertModel.isEmpty()) alertModel.addElement("✅  All stock levels are sufficient.");

        alertsPanel.add(new JScrollPane(alertList), BorderLayout.CENTER);

        // Warehouse usage bar
        JPanel warehousePanel = buildWarehousePanel();

        JPanel bottom = new JPanel(new GridLayout(1, 2, 10, 0));
        bottom.setBackground(BG_DARK);
        bottom.add(alertsPanel);
        bottom.add(warehousePanel);

        panel.add(statsRow, BorderLayout.NORTH);
        panel.add(bottom, BorderLayout.CENTER);
        return panel;
    }

    private JLabel buildStatCard(JPanel parent, String title, String value, Color color) {
        JPanel card = new JPanel(new GridLayout(2, 1));
        card.setBackground(BG_CARD);
        card.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(color, 2, true),
            new EmptyBorder(10, 10, 10, 10)
        ));

        JLabel lTitle = new JLabel(title, SwingConstants.CENTER);
        lTitle.setFont(new Font("Segoe UI", Font.PLAIN, 12));
        lTitle.setForeground(TEXT_MUTED);

        JLabel lValue = new JLabel(value, SwingConstants.CENTER);
        lValue.setFont(new Font("Segoe UI", Font.BOLD, 18));
        lValue.setForeground(color);

        card.add(lTitle);
        card.add(lValue);
        parent.add(card);
        return lValue;
    }

    private JPanel buildWarehousePanel() {
        JPanel panel = new JPanel(new BorderLayout(5, 5));
        panel.setBackground(BG_PANEL);
        panel.setBorder(buildTitledBorder("🏭 Warehouse Usage"));

        int used  = model.getTotalStock();
        int total = model.getWarehouseCapacity();
        int pct   = total > 0 ? (used * 100 / total) : 0;

        JProgressBar bar = new JProgressBar(0, 100);
        bar.setValue(pct);
        bar.setStringPainted(true);
        bar.setString(used + " / " + total + " kg  (" + pct + "%)");
        bar.setBackground(BG_CARD);
        bar.setForeground(pct > 80 ? ACCENT_RED : pct > 50 ? ACCENT_ORANGE : ACCENT_GREEN);
        bar.setFont(new Font("Segoe UI", Font.BOLD, 12));

        JLabel info = new JLabel("Available: " + model.getAvailableSpace() + " kg",
                                  SwingConstants.CENTER);
        info.setForeground(TEXT_MUTED);
        info.setFont(new Font("Segoe UI", Font.PLAIN, 12));

        panel.add(bar, BorderLayout.CENTER);
        panel.add(info, BorderLayout.SOUTH);
        return panel;
    }

    // ---- INVENTORY TAB ----
    private JPanel buildInventoryTab() {
        JPanel panel = new JPanel(new BorderLayout(5, 5));
        panel.setBackground(BG_DARK);
        panel.setBorder(new EmptyBorder(10, 10, 10, 10));

        // Table
        String[] cols = {"ID", "Name", "Type", "Category", "Price (KES)", "Stock (kg)",
                         "Reorder Level", "Total Value", "Status", "Storage Info"};
        DefaultTableModel tableModel = new DefaultTableModel(cols, 0) {
            public boolean isCellEditable(int r, int c) { return false; }
        };

        for (Product p : model.getProducts()) {
            tableModel.addRow(new Object[]{
                p.getID(), p.getName(), p.getProductType(), p.getCategory(),
                String.format("%.2f", p.getUnitPrice()), p.getQuantity(),
                p.getReorderLevel(), String.format("KES %.2f", p.getTotalValue()),
                p.getStatus(), p.getStorageInfo()
            });
        }

        JTable table = buildStyledTable(tableModel);

        // Color rows by status
        table.setDefaultRenderer(Object.class, new DefaultTableCellRenderer() {
            public Component getTableCellRendererComponent(JTable t, Object val,
                    boolean sel, boolean foc, int row, int col) {
                Component c = super.getTableCellRendererComponent(t, val, sel, foc, row, col);
                String status = t.getValueAt(row, 8).toString();
                if (!sel) {
                    if (status.equals("OUT_OF_STOCK"))   c.setBackground(new Color(80, 30, 30));
                    else if (status.equals("REORDER_NEEDED")) c.setBackground(new Color(80, 55, 20));
                    else if (status.equals("LOW_STOCK")) c.setBackground(new Color(60, 60, 20));
                    else                                 c.setBackground(BG_CARD);
                    c.setForeground(TEXT_PRIMARY);
                }
                return c;
            }
        });

        // Action buttons
        JPanel btnPanel = new JPanel(new FlowLayout(FlowLayout.LEFT, 8, 5));
        btnPanel.setBackground(BG_DARK);

        JButton btnAdd      = makeButton("➕ Add Stock",      ACCENT_GREEN);
        JButton btnDispatch = makeButton("📤 Dispatch Stock", ACCENT_ORANGE);
        JButton btnPrice    = makeButton("💰 Update Price",   ACCENT_BLUE);
        JButton btnRefresh  = makeButton("🔄 Refresh",        ACCENT_PURPLE);

        btnAdd.addActionListener(e -> {
            int row = table.getSelectedRow();
            if (row < 0) { showError("Please select a product."); return; }
            Product p = model.getProducts().get(row);
            String input = JOptionPane.showInputDialog(this,
                "Enter quantity to add (kg) for: " + p.getName(), "Add Stock",
                JOptionPane.PLAIN_MESSAGE);
            if (input != null) {
                try {
                    int qty = Integer.parseInt(input.trim());
                    p.addStock(qty);
                    log("Added " + qty + " kg to " + p.getName());
                    refreshAll(tableModel);
                } catch (Exception ex) { showError(ex.getMessage()); }
            }
        });

        btnDispatch.addActionListener(e -> {
            int row = table.getSelectedRow();
            if (row < 0) { showError("Please select a product."); return; }
            Product p = model.getProducts().get(row);
            String input = JOptionPane.showInputDialog(this,
                "Enter quantity to dispatch (kg) from: " + p.getName(), "Dispatch Stock",
                JOptionPane.PLAIN_MESSAGE);
            if (input != null) {
                try {
                    int qty = Integer.parseInt(input.trim());
                    p.dispatchStock(qty);
                    log("Dispatched " + qty + " kg from " + p.getName());
                    if (p.isReorderNeeded())
                        log("⚠ REORDER ALERT: " + p.getName() + " needs restocking!");
                    refreshAll(tableModel);
                } catch (Exception ex) { showError(ex.getMessage()); }
            }
        });

        btnPrice.addActionListener(e -> {
            int row = table.getSelectedRow();
            if (row < 0) { showError("Please select a product."); return; }
            Product p = model.getProducts().get(row);
            String input = JOptionPane.showInputDialog(this,
                "Enter new unit price (KES) for: " + p.getName(), "Update Price",
                JOptionPane.PLAIN_MESSAGE);
            if (input != null) {
                try {
                    double price = Double.parseDouble(input.trim());
                    p.setUnitPrice(price);
                    log("Updated price of " + p.getName() + " to KES " + price);
                    refreshAll(tableModel);
                } catch (Exception ex) { showError(ex.getMessage()); }
            }
        });

        btnRefresh.addActionListener(e -> refreshAll(tableModel));

        btnPanel.add(btnAdd);
        btnPanel.add(btnDispatch);
        btnPanel.add(btnPrice);
        btnPanel.add(btnRefresh);

        panel.add(btnPanel, BorderLayout.NORTH);
        panel.add(new JScrollPane(table), BorderLayout.CENTER);
        return panel;
    }

    // ---- ORDERS TAB ----
    private JPanel buildOrdersTab() {
        JPanel panel = new JPanel(new BorderLayout(5, 5));
        panel.setBackground(BG_DARK);
        panel.setBorder(new EmptyBorder(10, 10, 10, 10));

        String[] cols = {"Order ID", "Product", "Quantity (kg)", "Total Cost (KES)",
                         "Status", "Reason", "Timestamp"};
        DefaultTableModel tableModel = new DefaultTableModel(cols, 0) {
            public boolean isCellEditable(int r, int c) { return false; }
        };

        refreshOrderTable(tableModel);
        JTable table = buildStyledTable(tableModel);

        // Color rows by status
        table.setDefaultRenderer(Object.class, new DefaultTableCellRenderer() {
            public Component getTableCellRendererComponent(JTable t, Object val,
                    boolean sel, boolean foc, int row, int col) {
                Component c = super.getTableCellRendererComponent(t, val, sel, foc, row, col);
                String status = t.getValueAt(row, 4).toString();
                if (!sel) {
                    if (status.equals("APPROVED"))  c.setBackground(new Color(20, 60, 30));
                    else if (status.equals("REJECTED")) c.setBackground(new Color(70, 20, 20));
                    else                            c.setBackground(BG_CARD);
                    c.setForeground(TEXT_PRIMARY);
                }
                return c;
            }
        });

        // Place order form
        JPanel formPanel = new JPanel(new GridBagLayout());
        formPanel.setBackground(BG_PANEL);
        formPanel.setBorder(buildTitledBorder("Place New Order"));
        GridBagConstraints gbc = new GridBagConstraints();
        gbc.insets = new Insets(5, 5, 5, 5);
        gbc.fill   = GridBagConstraints.HORIZONTAL;

        String[] productNames  = model.getProducts().stream()
            .map(Product::getName).toArray(String[]::new);
        String[] supplierNames = model.getSuppliers().stream()
            .map(Supplier::getName).toArray(String[]::new);

        JComboBox<String> cmbProduct  = makeCombo(productNames);
        JComboBox<String> cmbSupplier = makeCombo(supplierNames);
        JTextField        txtQty      = makeTextField("100");

        gbc.gridx=0; gbc.gridy=0; formPanel.add(makeLabel("Product:"),  gbc);
        gbc.gridx=1;              formPanel.add(cmbProduct,             gbc);
        gbc.gridx=0; gbc.gridy=1; formPanel.add(makeLabel("Supplier:"), gbc);
        gbc.gridx=1;              formPanel.add(cmbSupplier,            gbc);
        gbc.gridx=0; gbc.gridy=2; formPanel.add(makeLabel("Qty (kg):"), gbc);
        gbc.gridx=1;              formPanel.add(txtQty,                 gbc);

        JButton btnOrder = makeButton("🛒 Place Order", ACCENT_GREEN);
        btnOrder.addActionListener(e -> {
            try {
                int pidx = cmbProduct.getSelectedIndex();
                int sidx = cmbSupplier.getSelectedIndex();
                int qty  = Integer.parseInt(txtQty.getText().trim());

                Product  p = model.getProducts().get(pidx);
                Supplier s = model.getSuppliers().get(sidx);
                Order order = model.placeOrder(p, s, qty);

                log("Order placed: " + qty + " kg of " + p.getName() +
                    " → " + order.getStatus() + " (" + order.getReason() + ")");
                refreshOrderTable(tableModel);
                refreshDashboard();

                JOptionPane.showMessageDialog(this,
                    "Order " + order.getStatus() + "\nReason: " + order.getReason(),
                    "Order Result",
                    order.getStatus() == OrderStatus.APPROVED ?
                        JOptionPane.INFORMATION_MESSAGE : JOptionPane.WARNING_MESSAGE);
            } catch (Exception ex) { showError(ex.getMessage()); }
        });

        gbc.gridx=0; gbc.gridy=3; gbc.gridwidth=2; formPanel.add(btnOrder, gbc);

        JPanel top = new JPanel(new BorderLayout());
        top.setBackground(BG_DARK);
        top.add(formPanel, BorderLayout.WEST);

        panel.add(top, BorderLayout.NORTH);
        panel.add(new JScrollPane(table), BorderLayout.CENTER);
        return panel;
    }

    // ---- SUPPLIERS TAB ----
    private JPanel buildSuppliersTab() {
        JPanel panel = new JPanel(new BorderLayout(5, 5));
        panel.setBackground(BG_DARK);
        panel.setBorder(new EmptyBorder(10, 10, 10, 10));

        String[] cols = {"ID", "Name", "Location", "Contact", "Reliability", "Status"};
        DefaultTableModel tableModel = new DefaultTableModel(cols, 0) {
            public boolean isCellEditable(int r, int c) { return false; }
        };

        for (Supplier s : model.getSuppliers()) {
            tableModel.addRow(new Object[]{
                s.getID(), s.getName(), s.getLocation(), s.getContact(),
                s.getRating() + "/10",
                s.isReliable() ? "✅ APPROVED" : "❌ UNRELIABLE"
            });
        }

        JTable table = buildStyledTable(tableModel);
        table.setDefaultRenderer(Object.class, new DefaultTableCellRenderer() {
            public Component getTableCellRendererComponent(JTable t, Object val,
                    boolean sel, boolean foc, int row, int col) {
                Component c = super.getTableCellRendererComponent(t, val, sel, foc, row, col);
                String status = t.getValueAt(row, 5).toString();
                if (!sel) {
                    c.setBackground(status.contains("APPROVED") ?
                        new Color(20, 60, 30) : new Color(70, 20, 20));
                    c.setForeground(TEXT_PRIMARY);
                }
                return c;
            }
        });

        panel.add(new JScrollPane(table), BorderLayout.CENTER);
        return panel;
    }

    // ---- SHIPMENTS TAB ----
    private JPanel buildShipmentsTab() {
        JPanel panel = new JPanel(new BorderLayout(5, 5));
        panel.setBackground(BG_DARK);
        panel.setBorder(new EmptyBorder(10, 10, 10, 10));

        String[] cols = {"ID", "Product", "Qty (kg)", "Origin", "Destination", "Status"};
        DefaultTableModel tableModel = new DefaultTableModel(cols, 0) {
            public boolean isCellEditable(int r, int c) { return false; }
        };

        refreshShipmentTable(tableModel);
        JTable table = buildStyledTable(tableModel);

        // Create shipment form
        JPanel formPanel = new JPanel(new GridBagLayout());
        formPanel.setBackground(BG_PANEL);
        formPanel.setBorder(buildTitledBorder("Create Shipment"));
        GridBagConstraints gbc = new GridBagConstraints();
        gbc.insets = new Insets(5, 5, 5, 5);
        gbc.fill   = GridBagConstraints.HORIZONTAL;

        JTextField txtOrigin  = makeTextField("Nakuru");
        JTextField txtDest    = makeTextField("Nairobi");
        JTextField txtProduct = makeTextField("Maize");
        JTextField txtQty     = makeTextField("100");

        gbc.gridx=0; gbc.gridy=0; formPanel.add(makeLabel("Origin:"),      gbc);
        gbc.gridx=1;              formPanel.add(txtOrigin,                  gbc);
        gbc.gridx=0; gbc.gridy=1; formPanel.add(makeLabel("Destination:"), gbc);
        gbc.gridx=1;              formPanel.add(txtDest,                    gbc);
        gbc.gridx=0; gbc.gridy=2; formPanel.add(makeLabel("Product:"),     gbc);
        gbc.gridx=1;              formPanel.add(txtProduct,                 gbc);
        gbc.gridx=0; gbc.gridy=3; formPanel.add(makeLabel("Qty (kg):"),    gbc);
        gbc.gridx=1;              formPanel.add(txtQty,                     gbc);

        JButton btnCreate   = makeButton("🚚 Create Shipment",  ACCENT_BLUE);
        JButton btnDispatch = makeButton("▶ Dispatch All",       ACCENT_GREEN);

        btnCreate.addActionListener(e -> {
            try {
                String origin  = txtOrigin.getText().trim();
                String dest    = txtDest.getText().trim();
                String product = txtProduct.getText().trim();
                int    qty     = Integer.parseInt(txtQty.getText().trim());
                model.createShipment(origin, dest, product, qty);
                log("Shipment created: " + qty + " kg of " + product +
                    " from " + origin + " to " + dest);
                refreshShipmentTable(tableModel);
                refreshDashboard();
            } catch (Exception ex) { showError(ex.getMessage()); }
        });

        btnDispatch.addActionListener(e -> {
            if (model.getShipments().isEmpty()) {
                showError("No shipments to dispatch.");
                return;
            }
            log("Dispatching " + model.getShipments().size() + " shipment(s) concurrently...");
            // Run concurrent shipment simulation on background threads
            ExecutorService executor = Executors.newFixedThreadPool(
                model.getShipments().size());
            for (Shipment s : model.getShipments()) {
                if (s.getStatus() == ShipmentStatus.PREPARING) {
                    executor.submit(() -> simulateShipment(s, tableModel));
                }
            }
            executor.shutdown();
        });

        gbc.gridx=0; gbc.gridy=4; formPanel.add(btnCreate,   gbc);
        gbc.gridy=5;               formPanel.add(btnDispatch, gbc);

        panel.add(formPanel, BorderLayout.WEST);
        panel.add(new JScrollPane(table), BorderLayout.CENTER);
        return panel;
    }

    private void simulateShipment(Shipment s, DefaultTableModel tableModel) {
        try {
            s.setStatus(ShipmentStatus.IN_TRANSIT);
            SwingUtilities.invokeLater(() -> {
                log("[SHIPMENT-" + s.getID() + "] " + s.getProductName() + " IN TRANSIT...");
                refreshShipmentTable(tableModel);
            });
            Thread.sleep(2000);

            s.setStatus(ShipmentStatus.ARRIVED);
            SwingUtilities.invokeLater(() -> {
                log("[SHIPMENT-" + s.getID() + "] " + s.getProductName() + " ARRIVED!");
                refreshShipmentTable(tableModel);
            });
            Thread.sleep(1000);

            s.setStatus(ShipmentStatus.DELIVERED);
            SwingUtilities.invokeLater(() -> {
                log("[SHIPMENT-" + s.getID() + "] " + s.getProductName() + " DELIVERED ✅");
                refreshShipmentTable(tableModel);
            });
        } catch (InterruptedException ex) {
            Thread.currentThread().interrupt();
        }
    }

    // ---- REPORTS TAB ----
    private JPanel buildReportsTab() {
        JPanel panel = new JPanel(new BorderLayout(5, 5));
        panel.setBackground(BG_DARK);
        panel.setBorder(new EmptyBorder(10, 10, 10, 10));

        JTextArea reportArea = new JTextArea();
        reportArea.setBackground(BG_CARD);
        reportArea.setForeground(ACCENT_GREEN);
        reportArea.setFont(new Font("Courier New", Font.PLAIN, 13));
        reportArea.setEditable(false);

        JPanel btnPanel = new JPanel(new FlowLayout(FlowLayout.LEFT, 8, 5));
        btnPanel.setBackground(BG_DARK);

        JButton btnInventory = makeButton("📦 Inventory Report", ACCENT_BLUE);
        JButton btnOrders    = makeButton("🛒 Orders Report",    ACCENT_GREEN);
        JButton btnAlerts    = makeButton("⚠ Alert Report",     ACCENT_ORANGE);

        btnInventory.addActionListener(e -> {
            StringBuilder sb = new StringBuilder();
            sb.append("============================================================\n");
            sb.append("  INVENTORY REPORT\n");
            sb.append("  Generated: ").append(LocalDateTime.now()
                .format(DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss"))).append("\n");
            sb.append("============================================================\n\n");
            sb.append(String.format("  %-15s %-12s %-10s %-15s %-20s%n",
                "Name", "Stock(kg)", "Price", "Total Value", "Status"));
            sb.append("  " + "-".repeat(75) + "\n");

            // Lambda: forEach to build report
            model.getProducts().forEach(p ->
                sb.append(String.format("  %-15s %-12d %-10.2f %-15.2f %-20s%n",
                    p.getName(), p.getQuantity(), p.getUnitPrice(),
                    p.getTotalValue(), p.getStatus()))
            );

            sb.append("  " + "-".repeat(75) + "\n");
            sb.append(String.format("  %-15s %-12d %-10s %-15.2f%n",
                "TOTAL", model.getTotalStock(), "", model.getTotalValue()));
            reportArea.setText(sb.toString());
            log("Inventory report generated.");
        });

        btnOrders.addActionListener(e -> {
            StringBuilder sb = new StringBuilder();
            sb.append("============================================================\n");
            sb.append("  ORDERS REPORT\n");
            sb.append("============================================================\n\n");
            if (model.getOrders().isEmpty()) {
                sb.append("  No orders placed yet.\n");
            } else {
                model.getOrders().forEach(o ->
                    sb.append(String.format("  [%d] %-15s %4d kg  KES %8.2f  %s%n",
                        o.getID(), o.getProductName(), o.getQuantity(),
                        o.getTotalCost(), o.getStatus()))
                );
            }
            reportArea.setText(sb.toString());
            log("Orders report generated.");
        });

        btnAlerts.addActionListener(e -> {
            StringBuilder sb = new StringBuilder();
            sb.append("============================================================\n");
            sb.append("  REORDER ALERTS REPORT\n");
            sb.append("============================================================\n\n");
            List<Product> alerts = model.getReorderAlerts();
            if (alerts.isEmpty()) {
                sb.append("  ✅ All stock levels are sufficient. No alerts.\n");
            } else {
                alerts.forEach(p ->
                    sb.append(String.format("  ⚠  %-15s | %d kg remaining | %s%n",
                        p.getName(), p.getQuantity(), p.getStatus()))
                );
            }
            reportArea.setText(sb.toString());
            log("Alerts report generated.");
        });

        btnPanel.add(btnInventory);
        btnPanel.add(btnOrders);
        btnPanel.add(btnAlerts);

        panel.add(btnPanel, BorderLayout.NORTH);
        panel.add(new JScrollPane(reportArea), BorderLayout.CENTER);
        return panel;
    }

    // ---- LOG TAB ----
    private JPanel buildLogTab() {
        JPanel panel = new JPanel(new BorderLayout(5, 5));
        panel.setBackground(BG_DARK);
        panel.setBorder(new EmptyBorder(10, 10, 10, 10));

        logArea = new JTextArea();
        logArea.setBackground(new Color(10, 20, 30));
        logArea.setForeground(ACCENT_GREEN);
        logArea.setFont(new Font("Courier New", Font.PLAIN, 12));
        logArea.setEditable(false);

        JButton btnClear = makeButton("🗑 Clear Log", ACCENT_RED);
        btnClear.addActionListener(e -> logArea.setText(""));

        panel.add(btnClear, BorderLayout.NORTH);
        panel.add(new JScrollPane(logArea), BorderLayout.CENTER);
        return panel;
    }

    // ---- STATUS BAR ----
    private JPanel buildStatusBar() {
        JPanel bar = new JPanel(new BorderLayout());
        bar.setBackground(BG_CARD);
        bar.setBorder(new EmptyBorder(5, 15, 5, 15));

        JLabel status = new JLabel("ICS 2276 | Agricultural Supply Chain System v6.0 | Ready");
        status.setFont(new Font("Segoe UI", Font.PLAIN, 11));
        status.setForeground(TEXT_MUTED);

        JLabel time = new JLabel(LocalDateTime.now()
            .format(DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm")));
        time.setFont(new Font("Segoe UI", Font.PLAIN, 11));
        time.setForeground(TEXT_MUTED);

        bar.add(status, BorderLayout.WEST);
        bar.add(time, BorderLayout.EAST);
        return bar;
    }


    // ---- HELPER METHODS ----

    private void refreshDashboard() {
        lblTotalProducts.setText(String.valueOf(model.getProducts().size()));
        lblTotalStock.setText(model.getTotalStock() + " kg");
        lblTotalValue.setText(String.format("KES %.0f", model.getTotalValue()));
        lblAlerts.setText(String.valueOf(model.getReorderAlerts().size()));
        lblOrders.setText(String.valueOf(model.getOrders().size()));
        lblShipments.setText(String.valueOf(model.getShipments().size()));
    }

    private void refreshAll(DefaultTableModel tableModel) {
        tableModel.setRowCount(0);
        for (Product p : model.getProducts()) {
            tableModel.addRow(new Object[]{
                p.getID(), p.getName(), p.getProductType(), p.getCategory(),
                String.format("%.2f", p.getUnitPrice()), p.getQuantity(),
                p.getReorderLevel(), String.format("KES %.2f", p.getTotalValue()),
                p.getStatus(), p.getStorageInfo()
            });
        }
        refreshDashboard();
    }

    private void refreshOrderTable(DefaultTableModel tableModel) {
        tableModel.setRowCount(0);
        for (Order o : model.getOrders()) {
            tableModel.addRow(new Object[]{
                o.getID(), o.getProductName(), o.getQuantity(),
                String.format("KES %.2f", o.getTotalCost()),
                o.getStatus(), o.getReason(), o.getTimestamp()
            });
        }
        refreshDashboard();
    }

    private void refreshShipmentTable(DefaultTableModel tableModel) {
        tableModel.setRowCount(0);
        for (Shipment s : model.getShipments()) {
            tableModel.addRow(new Object[]{
                s.getID(), s.getProductName(), s.getQuantity(),
                s.getOrigin(), s.getDestination(), s.getStatus()
            });
        }
        refreshDashboard();
    }

    private void saveData() {
        try {
            model.saveToFile();
            log("Data saved to inventory.csv and orders.csv");
            JOptionPane.showMessageDialog(this, "Data saved successfully!",
                "Save", JOptionPane.INFORMATION_MESSAGE);
        } catch (IOException ex) {
            showError("Save failed: " + ex.getMessage());
        }
    }

    private void log(String message) {
        if (logArea != null) {
            String ts = LocalDateTime.now().format(DateTimeFormatter.ofPattern("HH:mm:ss"));
            SwingUtilities.invokeLater(() -> {
                logArea.append("[" + ts + "] " + message + "\n");
                logArea.setCaretPosition(logArea.getDocument().getLength());
            });
        }
    }

    private void showError(String msg) {
        JOptionPane.showMessageDialog(this, msg, "Error", JOptionPane.ERROR_MESSAGE);
        log("ERROR: " + msg);
    }

    // ---- UI COMPONENT FACTORIES ----

    private JTable buildStyledTable(DefaultTableModel model) {
        JTable table = new JTable(model);
        table.setBackground(BG_CARD);
        table.setForeground(TEXT_PRIMARY);
        table.setGridColor(BG_PANEL);
        table.setRowHeight(28);
        table.setFont(new Font("Segoe UI", Font.PLAIN, 13));
        table.setSelectionBackground(ACCENT_BLUE);
        table.setSelectionForeground(Color.WHITE);
        table.setShowHorizontalLines(true);
        table.setShowVerticalLines(false);

        JTableHeader header = table.getTableHeader();
        header.setBackground(BG_PANEL);
        header.setForeground(ACCENT_BLUE);
        header.setFont(new Font("Segoe UI", Font.BOLD, 12));
        return table;
    }

    private JButton makeButton(String text, Color color) {
        JButton btn = new JButton(text);
        btn.setBackground(color);
        btn.setForeground(Color.WHITE);
        btn.setFont(new Font("Segoe UI", Font.BOLD, 12));
        btn.setBorder(new EmptyBorder(8, 14, 8, 14));
        btn.setFocusPainted(false);
        btn.setCursor(new Cursor(Cursor.HAND_CURSOR));
        return btn;
    }

    private JLabel makeLabel(String text) {
        JLabel lbl = new JLabel(text);
        lbl.setForeground(TEXT_PRIMARY);
        lbl.setFont(new Font("Segoe UI", Font.PLAIN, 13));
        return lbl;
    }

    private JTextField makeTextField(String defaultText) {
        JTextField tf = new JTextField(defaultText, 15);
        tf.setBackground(BG_CARD);
        tf.setForeground(TEXT_PRIMARY);
        tf.setCaretColor(TEXT_PRIMARY);
        tf.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(ACCENT_BLUE, 1),
            new EmptyBorder(4, 6, 4, 6)
        ));
        tf.setFont(new Font("Segoe UI", Font.PLAIN, 13));
        return tf;
    }

    private JComboBox<String> makeCombo(String[] items) {
        JComboBox<String> combo = new JComboBox<>(items);
        combo.setBackground(BG_CARD);
        combo.setForeground(TEXT_PRIMARY);
        combo.setFont(new Font("Segoe UI", Font.PLAIN, 13));
        return combo;
    }

    private TitledBorder buildTitledBorder(String title) {
        TitledBorder border = BorderFactory.createTitledBorder(
            BorderFactory.createLineBorder(ACCENT_BLUE, 1), title);
        border.setTitleColor(ACCENT_BLUE);
        border.setTitleFont(new Font("Segoe UI", Font.BOLD, 12));
        return border;
    }


    // ============================================================
    //  MAIN ENTRY POINT
    // ============================================================
    public static void main(String[] args) {
        // Use system look and feel as base
        try {
            UIManager.setLookAndFeel(UIManager.getCrossPlatformLookAndFeelClassName());
        } catch (Exception e) {
            // fallback to default
        }
        SwingUtilities.invokeLater(AgriculturalSupplyChainGUI::new);
    }
}
