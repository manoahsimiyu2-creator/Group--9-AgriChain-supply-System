// ============================================================
//  ICS 2276: Computer Programming II
//  Agricultural Supply Chain System
//  KenyanDataGenerator.h — 1000 Realistic Kenyan Datasets
//
//  Dataset breakdown:
//  - 50  Suppliers  (from counties across Kenya)
//  - 200 Products   (FreshProduce + ProcessedGood)
//  - 750 Orders     (transaction history)
//  Total: 1000 records
// ============================================================

#ifndef KENYAN_DATA_GENERATOR_H
#define KENYAN_DATA_GENERATOR_H

#include <string>
#include <vector>
using namespace std;

// ── Supplier record ─────────────────────────────────────────
struct SupplierData {
    int    id;
    string name;
    string location;
    string contact;
    double rating;
};

// ── Product record ──────────────────────────────────────────
struct ProductData {
    int    id;
    string type;         // "Fresh" or "Processed"
    string name;
    string category;
    double unitPrice;    // KES per kg
    int    quantity;
    int    reorderLevel;
    // Fresh only
    int    shelfLifeDays;
    double storageTemp;
    // Processed only
    string packagingType;
    int    expiryMonths;
};

// ── Order record ─────────────────────────────────────────────
struct OrderData {
    int    orderID;
    int    productID;
    string productName;
    int    supplierID;
    int    quantity;
    double unitPrice;
    string status;       // "APPROVED" or "REJECTED"
    string reason;
};


// ════════════════════════════════════════════════════════════
//  50 KENYAN SUPPLIERS
// ════════════════════════════════════════════════════════════
static const vector<SupplierData> KENYAN_SUPPLIERS = {
    // Rift Valley (major grain & dairy region)
    {501, "Rift Valley Farms Ltd",       "Nakuru, Rift Valley",   "+254700100001", 8.5},
    {502, "Nandi Hills Agro Supplies",   "Nandi, Rift Valley",    "+254700100002", 9.1},
    {503, "Eldoret Grain Merchants",     "Eldoret, Uasin Gishu",  "+254700100003", 7.8},
    {504, "Baringo County Produce",      "Kabarnet, Baringo",     "+254700100004", 6.5},
    {505, "Trans Nzoia Cereals Ltd",     "Kitale, Trans Nzoia",   "+254700100005", 8.9},
    {506, "Kericho Tea & Agri Co",       "Kericho, Rift Valley",  "+254700100006", 9.3},
    {507, "Bomet Fresh Produce",         "Bomet, Rift Valley",    "+254700100007", 7.2},
    {508, "Narok County Supplies",       "Narok, Rift Valley",    "+254700100008", 8.0},

    // Central Region (horticulture & dairy)
    {509, "Kiambu Horticulture Ltd",     "Kiambu, Central",       "+254700100009", 8.7},
    {510, "Murang'a Fresh Gardens",      "Murang'a, Central",     "+254700100010", 7.5},
    {511, "Nyeri Highland Farms",        "Nyeri, Central",        "+254700100011", 9.0},
    {512, "Kirinyaga Rice Millers",      "Kerugoya, Kirinyaga",   "+254700100012", 8.2},
    {513, "Thika Agro Processing",       "Thika, Kiambu",         "+254700100013", 7.8},
    {514, "Nyandarua Potato Farmers",    "Ol Kalou, Nyandarua",   "+254700100014", 8.4},

    // Eastern Region (dryland crops)
    {515, "Machakos Agri Collective",    "Machakos, Eastern",     "+254700100015", 6.8},
    {516, "Makueni Mango Growers",       "Wote, Makueni",         "+254700100016", 8.1},
    {517, "Kitui Dryland Produce",       "Kitui, Eastern",        "+254700100017", 6.2},
    {518, "Mwingi Agri Supplies",        "Mwingi, Kitui",         "+254700100018", 7.0},
    {519, "Embu Farm Fresh Ltd",         "Embu, Eastern",         "+254700100019", 8.6},
    {520, "Tharaka Nithi Cereals",       "Chuka, Tharaka Nithi",  "+254700100020", 7.3},

    // Coast Region (tropical produce)
    {521, "Kilifi Coconut Growers",      "Kilifi, Coast",         "+254700100021", 8.0},
    {522, "Kwale Cashew Association",    "Kwale, Coast",          "+254700100022", 7.6},
    {523, "Mombasa Agro Trading",        "Mombasa, Coast",        "+254700100023", 9.2},
    {524, "Taita Taveta Horticultural",  "Voi, Taita Taveta",     "+254700100024", 7.9},
    {525, "Lamu Coastal Produce",        "Lamu, Coast",           "+254700100025", 6.7},
    {526, "Tana River Farm Collective",  "Hola, Tana River",      "+254700100026", 6.3},

    // Nyanza Region (fishing & sugarcane)
    {527, "Kisumu Agri Supplies",        "Kisumu, Nyanza",        "+254700100027", 8.3},
    {528, "Siaya County Produce",        "Siaya, Nyanza",         "+254700100028", 7.7},
    {529, "Homa Bay Farm Collective",    "Homa Bay, Nyanza",      "+254700100029", 8.1},
    {530, "Migori Fresh Markets Ltd",    "Migori, Nyanza",        "+254700100030", 7.4},
    {531, "Kisii Highland Produce",      "Kisii, Nyanza",         "+254700100031", 8.8},
    {532, "Nyamira Tea Growers",         "Nyamira, Nyanza",       "+254700100032", 9.0},

    // Western Region (sugarcane & maize)
    {533, "Kakamega Agri Merchants",     "Kakamega, Western",     "+254700100033", 8.2},
    {534, "Bungoma Grain Suppliers",     "Bungoma, Western",      "+254700100034", 7.9},
    {535, "Vihiga Fresh Farms",          "Vihiga, Western",       "+254700100035", 7.1},
    {536, "Busia Borderline Agro",       "Busia, Western",        "+254700100036", 6.9},

    // North Eastern (livestock & drought crops)
    {537, "Garissa Livestock Trading",   "Garissa, North Eastern","+254700100037", 6.5},
    {538, "Wajir Dry Goods Supplies",    "Wajir, North Eastern",  "+254700100038", 6.1},
    {539, "Mandera Border Traders",      "Mandera, North Eastern","+254700100039", 5.8},

    // Upper Eastern (livestock & sorghum)
    {540, "Marsabit Livestock Co",       "Marsabit, Eastern",     "+254700100040", 6.8},
    {541, "Isiolo Dryland Agro",         "Isiolo, Eastern",       "+254700100041", 7.0},
    {542, "Samburu Herders Cooperative", "Maralal, Samburu",      "+254700100042", 6.4},

    // Nairobi & surroundings (processing & distribution)
    {543, "Nairobi Agro Distributors",   "Nairobi, Nairobi",      "+254700100043", 9.4},
    {544, "Ruiru Food Processing Ltd",   "Ruiru, Kiambu",         "+254700100044", 8.8},
    {545, "Athi River Mills",            "Athi River, Machakos",  "+254700100045", 8.5},
    {546, "Karen Organic Farms",         "Karen, Nairobi",        "+254700100046", 9.2},
    {547, "Westlands Agri Trading",      "Westlands, Nairobi",    "+254700100047", 8.0},

    // National processors
    {548, "Kenya Farmers Association",   "Nairobi, Kenya",        "+254700100048", 9.5},
    {549, "NCPB (National Cereals)",     "Nairobi, Kenya",        "+254700100049", 9.0},
    {550, "Kenya Cooperative Creameries","Nairobi, Kenya",        "+254700100050", 9.3},
};


// ════════════════════════════════════════════════════════════
//  200 KENYAN PRODUCTS
//  100 FreshProduce + 100 ProcessedGood
// ════════════════════════════════════════════════════════════
static const vector<ProductData> KENYAN_PRODUCTS = {

    // ── FRESH PRODUCE (100) ──────────────────────────────────
    // Vegetables (30)
    {1001,"Fresh","Tomatoes",       "Vegetable", 80.0,  500, 100,  7, 12.0, "", 0},
    {1002,"Fresh","Kales (Sukuma)", "Vegetable", 40.0,  400,  80,  5, 10.0, "", 0},
    {1003,"Fresh","Spinach",        "Vegetable", 50.0,  300,  60,  4, 10.0, "", 0},
    {1004,"Fresh","Cabbage",        "Vegetable", 35.0,  600, 120,  7, 10.0, "", 0},
    {1005,"Fresh","Onions",         "Vegetable", 60.0,  800, 150, 14, 15.0, "", 0},
    {1006,"Fresh","Garlic",         "Vegetable",120.0,  200,  40, 21, 15.0, "", 0},
    {1007,"Fresh","Carrots",        "Vegetable", 55.0,  500, 100, 10, 10.0, "", 0},
    {1008,"Fresh","Capsicum",       "Vegetable", 90.0,  300,  60,  7, 12.0, "", 0},
    {1009,"Fresh","Eggplant",       "Vegetable", 70.0,  250,  50,  7, 12.0, "", 0},
    {1010,"Fresh","Pumpkin",        "Vegetable", 30.0,  400,  80, 14, 15.0, "", 0},
    {1011,"Fresh","Sweet Pepper",   "Vegetable", 95.0,  200,  40,  7, 12.0, "", 0},
    {1012,"Fresh","Green Beans",    "Vegetable", 85.0,  300,  60,  7, 10.0, "", 0},
    {1013,"Fresh","Peas",           "Vegetable", 80.0,  250,  50,  5, 10.0, "", 0},
    {1014,"Fresh","Courgette",      "Vegetable", 75.0,  200,  40,  7, 10.0, "", 0},
    {1015,"Fresh","Broccoli",       "Vegetable",110.0,  150,  30,  7,  4.0, "", 0},
    {1016,"Fresh","Cauliflower",    "Vegetable",100.0,  150,  30,  7,  4.0, "", 0},
    {1017,"Fresh","Cucumber",       "Vegetable", 60.0,  300,  60,  7, 12.0, "", 0},
    {1018,"Fresh","Bitter Gourd",   "Vegetable", 70.0,  150,  30,  5, 12.0, "", 0},
    {1019,"Fresh","Leeks",          "Vegetable", 90.0,  100,  20,  7, 10.0, "", 0},
    {1020,"Fresh","Spring Onion",   "Vegetable", 65.0,  200,  40,  5, 10.0, "", 0},
    {1021,"Fresh","Lettuce",        "Vegetable", 80.0,  200,  40,  4,  4.0, "", 0},
    {1022,"Fresh","Celery",         "Vegetable", 90.0,  150,  30,  7,  4.0, "", 0},
    {1023,"Fresh","Beetroot",       "Vegetable", 60.0,  250,  50, 14, 10.0, "", 0},
    {1024,"Fresh","Radish",         "Vegetable", 50.0,  150,  30,  7, 10.0, "", 0},
    {1025,"Fresh","Sweet Corn",     "Vegetable", 45.0,  400,  80,  5, 12.0, "", 0},
    {1026,"Fresh","Arrowroot",      "Vegetable", 40.0,  300,  60, 14, 15.0, "", 0},
    {1027,"Fresh","Yam",            "Vegetable", 55.0,  300,  60, 14, 15.0, "", 0},
    {1028,"Fresh","Sweet Potato",   "Vegetable", 35.0,  500, 100, 14, 15.0, "", 0},
    {1029,"Fresh","Irish Potato",   "Vegetable", 30.0, 1000, 200, 21, 12.0, "", 0},
    {1030,"Fresh","Cassava",        "Vegetable", 25.0,  600, 120, 14, 15.0, "", 0},

    // Fruits (30)
    {1031,"Fresh","Bananas",        "Fruit",     35.0,  800, 150, 14, 13.0, "", 0},
    {1032,"Fresh","Mangoes",        "Fruit",     50.0,  600, 120,  7, 13.0, "", 0},
    {1033,"Fresh","Avocados",       "Fruit",     60.0,  500, 100,  7, 13.0, "", 0},
    {1034,"Fresh","Passion Fruit",  "Fruit",    100.0,  300,  60,  7, 13.0, "", 0},
    {1035,"Fresh","Pineapples",     "Fruit",     45.0,  400,  80, 10, 13.0, "", 0},
    {1036,"Fresh","Watermelon",     "Fruit",     25.0,  600, 120, 10, 13.0, "", 0},
    {1037,"Fresh","Papaya",         "Fruit",     40.0,  400,  80,  5, 13.0, "", 0},
    {1038,"Fresh","Guava",          "Fruit",     55.0,  300,  60,  5, 13.0, "", 0},
    {1039,"Fresh","Oranges",        "Fruit",     40.0,  500, 100, 14, 12.0, "", 0},
    {1040,"Fresh","Lemons",         "Fruit",     60.0,  300,  60, 14, 12.0, "", 0},
    {1041,"Fresh","Tangerines",     "Fruit",     55.0,  300,  60, 14, 12.0, "", 0},
    {1042,"Fresh","Grapes",         "Fruit",    150.0,  200,  40,  7,  4.0, "", 0},
    {1043,"Fresh","Strawberries",   "Fruit",    200.0,  100,  20,  3,  4.0, "", 0},
    {1044,"Fresh","Blueberries",    "Fruit",    350.0,   80,  20,  5,  4.0, "", 0},
    {1045,"Fresh","Plums",          "Fruit",    120.0,  150,  30,  7, 10.0, "", 0},
    {1046,"Fresh","Peaches",        "Fruit",    130.0,  150,  30,  5, 10.0, "", 0},
    {1047,"Fresh","Lychee",         "Fruit",    180.0,  100,  20,  5, 10.0, "", 0},
    {1048,"Fresh","Jackfruit",      "Fruit",     50.0,  200,  40,  5, 15.0, "", 0},
    {1049,"Fresh","Tamarind",       "Fruit",     90.0,  150,  30, 14, 15.0, "", 0},
    {1050,"Fresh","Coconut",        "Fruit",     30.0,  500, 100, 30, 20.0, "", 0},
    {1051,"Fresh","Dates",          "Fruit",    200.0,  150,  30, 60, 20.0, "", 0},
    {1052,"Fresh","Figs",           "Fruit",    180.0,  100,  20,  5, 10.0, "", 0},
    {1053,"Fresh","Apples",         "Fruit",    120.0,  200,  40, 30,  4.0, "", 0},
    {1054,"Fresh","Pears",          "Fruit",    110.0,  150,  30, 14,  4.0, "", 0},
    {1055,"Fresh","Kiwi",           "Fruit",    200.0,  100,  20, 14,  4.0, "", 0},
    {1056,"Fresh","Melon",          "Fruit",     45.0,  300,  60, 14, 13.0, "", 0},
    {1057,"Fresh","Pomegranate",    "Fruit",    160.0,  100,  20, 14, 10.0, "", 0},
    {1058,"Fresh","Star Fruit",     "Fruit",    150.0,   80,  20,  5, 13.0, "", 0},
    {1059,"Fresh","Dragon Fruit",   "Fruit",    300.0,   80,  20,  7, 13.0, "", 0},
    {1060,"Fresh","Baobab Fruit",   "Fruit",    120.0,  100,  20, 30, 20.0, "", 0},

    // Dairy & Protein (20)
    {1061,"Fresh","Fresh Milk",     "Dairy",     55.0,  500, 100,  3,  4.0, "", 0},
    {1062,"Fresh","Goat Milk",      "Dairy",     70.0,  200,  40,  3,  4.0, "", 0},
    {1063,"Fresh","Camel Milk",     "Dairy",    100.0,  150,  30,  3,  4.0, "", 0},
    {1064,"Fresh","Fresh Cream",    "Dairy",    150.0,  100,  20,  5,  4.0, "", 0},
    {1065,"Fresh","Farm Butter",    "Dairy",    200.0,  100,  20, 14,  4.0, "", 0},
    {1066,"Fresh","Fresh Yoghurt",  "Dairy",    120.0,  200,  40,  7,  4.0, "", 0},
    {1067,"Fresh","Cottage Cheese", "Dairy",    180.0,  100,  20,  7,  4.0, "", 0},
    {1068,"Fresh","Free Range Eggs","Poultry",   15.0, 2000, 400, 14, 10.0, "", 0},
    {1069,"Fresh","Chicken Meat",   "Poultry",  250.0,  300,  60,  3,  4.0, "", 0},
    {1070,"Fresh","Tilapia Fish",   "Seafood",  200.0,  300,  60,  2,  2.0, "", 0},
    {1071,"Fresh","Nile Perch",     "Seafood",  250.0,  200,  40,  2,  2.0, "", 0},
    {1072,"Fresh","Omena (Dagaa)",  "Seafood",   80.0,  400,  80,  3,  4.0, "", 0},
    {1073,"Fresh","Beef",           "Meat",     350.0,  200,  40,  3,  2.0, "", 0},
    {1074,"Fresh","Goat Meat",      "Meat",     400.0,  150,  30,  3,  2.0, "", 0},
    {1075,"Fresh","Pork",           "Meat",     300.0,  150,  30,  3,  2.0, "", 0},
    {1076,"Fresh","Lamb",           "Meat",     450.0,  100,  20,  3,  2.0, "", 0},
    {1077,"Fresh","Camel Meat",     "Meat",     380.0,   80,  20,  3,  2.0, "", 0},
    {1078,"Fresh","Rabbit",         "Meat",     280.0,  100,  20,  3,  2.0, "", 0},
    {1079,"Fresh","Turkey",         "Poultry",  350.0,   80,  20,  3,  2.0, "", 0},
    {1080,"Fresh","Duck",           "Poultry",  320.0,   80,  20,  3,  2.0, "", 0},

    // Fresh Herbs & Spices (20)
    {1081,"Fresh","Dhania (Coriander)","Herb",   60.0,  200,  40,  5, 10.0, "", 0},
    {1082,"Fresh","Mint",           "Herb",      55.0,  150,  30,  5, 10.0, "", 0},
    {1083,"Fresh","Basil",          "Herb",      70.0,  100,  20,  5, 10.0, "", 0},
    {1084,"Fresh","Rosemary",       "Herb",      80.0,  100,  20,  7, 10.0, "", 0},
    {1085,"Fresh","Thyme",          "Herb",      85.0,  100,  20,  7, 10.0, "", 0},
    {1086,"Fresh","Parsley",        "Herb",      60.0,  150,  30,  5, 10.0, "", 0},
    {1087,"Fresh","Chives",         "Herb",      75.0,  100,  20,  5, 10.0, "", 0},
    {1088,"Fresh","Dill",           "Herb",      80.0,  100,  20,  5, 10.0, "", 0},
    {1089,"Fresh","Sage",           "Herb",      90.0,   80,  20,  7, 10.0, "", 0},
    {1090,"Fresh","Lemongrass",     "Herb",      50.0,  150,  30,  7, 12.0, "", 0},
    {1091,"Fresh","Ginger Root",    "Spice",    120.0,  200,  40, 14, 12.0, "", 0},
    {1092,"Fresh","Turmeric Root",  "Spice",    150.0,  150,  30, 14, 12.0, "", 0},
    {1093,"Fresh","Chilli Pepper",  "Spice",     90.0,  200,  40,  7, 12.0, "", 0},
    {1094,"Fresh","Black Pepper",   "Spice",    200.0,  100,  20, 21, 15.0, "", 0},
    {1095,"Fresh","Cardamom",       "Spice",    400.0,   80,  20, 30, 15.0, "", 0},
    {1096,"Fresh","Cinnamon Sticks","Spice",    300.0,   80,  20, 60, 15.0, "", 0},
    {1097,"Fresh","Cloves",         "Spice",    350.0,   60,  15, 60, 15.0, "", 0},
    {1098,"Fresh","Vanilla Pods",   "Spice",    800.0,   40,  10, 90, 15.0, "", 0},
    {1099,"Fresh","Nutmeg",         "Spice",    450.0,   60,  15, 90, 15.0, "", 0},
    {1100,"Fresh","Saffron",        "Spice",   2500.0,   10,   3,180, 15.0, "", 0},

    // ── PROCESSED GOODS (100) ────────────────────────────────
    // Grains & Cereals (25)
    {1101,"Processed","Maize Flour",         "Grain",  55.0, 2000, 400, 0,0, "Sack",    12},
    {1102,"Processed","Wheat Flour",         "Grain",  65.0, 1500, 300, 0,0, "Sack",     9},
    {1103,"Processed","Rice (Pishori)",      "Grain",  90.0, 1000, 200, 0,0, "Sack",    18},
    {1104,"Processed","Rice (Basmati)",      "Grain", 120.0,  800, 150, 0,0, "Sack",    18},
    {1105,"Processed","Sorghum",             "Grain",  45.0,  800, 150, 0,0, "Sack",    24},
    {1106,"Processed","Millet",              "Grain",  50.0,  600, 120, 0,0, "Sack",    24},
    {1107,"Processed","Barley",              "Grain",  55.0,  500, 100, 0,0, "Sack",    18},
    {1108,"Processed","Oats",                "Grain",  80.0,  400,  80, 0,0, "Box",     12},
    {1109,"Processed","Semolina",            "Grain",  70.0,  400,  80, 0,0, "Sack",    12},
    {1110,"Processed","Ugali Flour (Sifted)","Grain",  50.0, 2500, 500, 0,0, "Sack",    12},
    {1111,"Processed","Posho",               "Grain",  48.0, 2000, 400, 0,0, "Sack",    12},
    {1112,"Processed","Corn Meal",           "Grain",  52.0, 1000, 200, 0,0, "Sack",    12},
    {1113,"Processed","Buckwheat",           "Grain", 100.0,  200,  40, 0,0, "Bag",     18},
    {1114,"Processed","Quinoa",              "Grain", 250.0,  150,  30, 0,0, "Bag",     24},
    {1115,"Processed","Amaranth",            "Grain", 150.0,  200,  40, 0,0, "Bag",     18},
    {1116,"Processed","Cassava Flour",       "Grain",  45.0,  800, 150, 0,0, "Sack",    12},
    {1117,"Processed","Soy Flour",           "Grain",  80.0,  400,  80, 0,0, "Sack",    12},
    {1118,"Processed","Groundnut Flour",     "Grain",  90.0,  300,  60, 0,0, "Sack",     9},
    {1119,"Processed","Green Gram Flour",    "Grain",  75.0,  300,  60, 0,0, "Bag",     12},
    {1120,"Processed","Arrowroot Flour",     "Grain",  60.0,  200,  40, 0,0, "Bag",     12},
    {1121,"Processed","Chia Seeds",          "Grain", 200.0,  100,  20, 0,0, "Bag",     24},
    {1122,"Processed","Sesame Seeds",        "Grain", 180.0,  150,  30, 0,0, "Bag",     18},
    {1123,"Processed","Sunflower Seeds",     "Grain", 100.0,  200,  40, 0,0, "Bag",     18},
    {1124,"Processed","Pumpkin Seeds",       "Grain", 150.0,  150,  30, 0,0, "Bag",     18},
    {1125,"Processed","Flaxseeds",           "Grain", 180.0,  100,  20, 0,0, "Bag",     24},

    // Legumes & Pulses (15)
    {1126,"Processed","Kidney Beans",        "Legume",  75.0, 600, 120, 0,0, "Sack",   24},
    {1127,"Processed","Black Beans",         "Legume",  80.0, 400,  80, 0,0, "Sack",   24},
    {1128,"Processed","Chickpeas",           "Legume",  90.0, 400,  80, 0,0, "Sack",   24},
    {1129,"Processed","Lentils (Red)",       "Legume",  85.0, 500, 100, 0,0, "Sack",   24},
    {1130,"Processed","Lentils (Green)",     "Legume",  80.0, 400,  80, 0,0, "Sack",   24},
    {1131,"Processed","Green Gram (Ndengu)", "Legume",  90.0, 500, 100, 0,0, "Sack",   18},
    {1132,"Processed","Pigeon Peas (Njahi)", "Legume",  85.0, 400,  80, 0,0, "Sack",   24},
    {1133,"Processed","Soy Beans",           "Legume",  70.0, 600, 120, 0,0, "Sack",   24},
    {1134,"Processed","Black Eyed Peas",     "Legume",  80.0, 400,  80, 0,0, "Sack",   24},
    {1135,"Processed","Groundnuts (Peanuts)","Legume", 120.0, 500, 100, 0,0, "Sack",   12},
    {1136,"Processed","Bambara Nuts",        "Legume",  95.0, 200,  40, 0,0, "Sack",   24},
    {1137,"Processed","Dolichos Beans",      "Legume",  88.0, 300,  60, 0,0, "Sack",   24},
    {1138,"Processed","White Beans",         "Legume",  78.0, 400,  80, 0,0, "Sack",   24},
    {1139,"Processed","Broad Beans",         "Legume",  82.0, 300,  60, 0,0, "Sack",   24},
    {1140,"Processed","Cowpeas",             "Legume",  76.0, 350,  70, 0,0, "Sack",   24},

    // Oils & Fats (10)
    {1141,"Processed","Sunflower Oil",       "Oil",    180.0, 500, 100, 0,0, "Jerry Can",18},
    {1142,"Processed","Palm Oil",            "Oil",    160.0, 600, 120, 0,0, "Jerry Can",24},
    {1143,"Processed","Coconut Oil",         "Oil",    250.0, 300,  60, 0,0, "Bottle",  18},
    {1144,"Processed","Olive Oil",           "Oil",    500.0, 150,  30, 0,0, "Bottle",  24},
    {1145,"Processed","Soy Oil",             "Oil",    170.0, 400,  80, 0,0, "Jerry Can",18},
    {1146,"Processed","Groundnut Oil",       "Oil",    200.0, 250,  50, 0,0, "Bottle",  18},
    {1147,"Processed","Canola Oil",          "Oil",    190.0, 200,  40, 0,0, "Bottle",  18},
    {1148,"Processed","Sesame Oil",          "Oil",    350.0, 100,  20, 0,0, "Bottle",  24},
    {1149,"Processed","Margarine",           "Oil",    200.0, 300,  60, 0,0, "Crate",    9},
    {1150,"Processed","Ghee (Samli)",        "Oil",    400.0, 150,  30, 0,0, "Tin",     18},

    // Dairy Products (10)
    {1151,"Processed","UHT Milk",            "Dairy",   75.0, 1000, 200, 0,0, "Carton",   6},
    {1152,"Processed","Powdered Milk",       "Dairy",  350.0,  400,  80, 0,0, "Tin",     18},
    {1153,"Processed","Condensed Milk",      "Dairy",  180.0,  300,  60, 0,0, "Tin",     24},
    {1154,"Processed","Evaporated Milk",     "Dairy",  150.0,  400,  80, 0,0, "Tin",     24},
    {1155,"Processed","Mozzarella Cheese",   "Dairy",  500.0,   80,  20, 0,0, "Pack",     6},
    {1156,"Processed","Cheddar Cheese",      "Dairy",  600.0,   60,  15, 0,0, "Pack",    12},
    {1157,"Processed","Commercial Butter",   "Dairy",  350.0,  150,  30, 0,0, "Pack",     9},
    {1158,"Processed","Commercial Yoghurt",  "Dairy",  150.0,  300,  60, 0,0, "Carton",   3},
    {1159,"Processed","Ice Cream",           "Dairy",  300.0,  100,  20, 0,0, "Crate",    6},
    {1160,"Processed","Whey Protein",        "Dairy",  800.0,   50,  10, 0,0, "Bag",     18},

    // Sugar & Sweeteners (5)
    {1161,"Processed","White Sugar",         "Sugar",   95.0, 2000, 400, 0,0, "Sack",   24},
    {1162,"Processed","Brown Sugar",         "Sugar",  110.0,  800, 150, 0,0, "Sack",   24},
    {1163,"Processed","Honey",               "Sugar",  500.0,  200,  40, 0,0, "Jar",    36},
    {1164,"Processed","Molasses",            "Sugar",   60.0,  500, 100, 0,0, "Drum",   12},
    {1165,"Processed","Icing Sugar",         "Sugar",  120.0,  300,  60, 0,0, "Bag",    18},

    // Salt & Condiments (5)
    {1166,"Processed","Table Salt",          "Condiment",20.0,1000, 200, 0,0, "Sack",   60},
    {1167,"Processed","Rock Salt",           "Condiment",15.0, 800, 150, 0,0, "Sack",   60},
    {1168,"Processed","Tomato Sauce",        "Condiment",150.0, 300, 60, 0,0, "Carton",  12},
    {1169,"Processed","Chilli Sauce",        "Condiment",180.0, 200, 40, 0,0, "Carton",  12},
    {1170,"Processed","Soy Sauce",           "Condiment",200.0, 150, 30, 0,0, "Carton",  18},

    // Beverages (10)
    {1171,"Processed","Black Tea (Dust)",    "Beverage", 200.0, 500, 100, 0,0, "Carton", 24},
    {1172,"Processed","Green Tea",           "Beverage", 300.0, 300,  60, 0,0, "Box",    18},
    {1173,"Processed","Coffee Beans",        "Beverage", 800.0, 200,  40, 0,0, "Sack",   12},
    {1174,"Processed","Instant Coffee",      "Beverage", 600.0, 200,  40, 0,0, "Tin",    18},
    {1175,"Processed","Cocoa Powder",        "Beverage", 400.0, 150,  30, 0,0, "Bag",    18},
    {1176,"Processed","Fruit Juice (Mango)", "Beverage", 120.0, 400,  80, 0,0, "Carton",  6},
    {1177,"Processed","Fruit Juice (Passion)","Beverage",130.0, 300,  60, 0,0, "Carton",  6},
    {1178,"Processed","Fortified Porridge",  "Beverage",  90.0, 500, 100, 0,0, "Box",    12},
    {1179,"Processed","Herbal Tea Mix",      "Beverage", 250.0, 150,  30, 0,0, "Box",    18},
    {1180,"Processed","Malted Drink Mix",    "Beverage", 300.0, 200,  40, 0,0, "Tin",    18},

    // Animal Feed (10)
    {1181,"Processed","Dairy Meal",          "Feed",    55.0, 2000, 400, 0,0, "Sack",   12},
    {1182,"Processed","Poultry Grower Mash", "Feed",    60.0, 1500, 300, 0,0, "Sack",   12},
    {1183,"Processed","Pig Finisher",        "Feed",    58.0, 1000, 200, 0,0, "Sack",   12},
    {1184,"Processed","Fish Meal",           "Feed",   150.0,  500, 100, 0,0, "Sack",    9},
    {1185,"Processed","Rabbit Pellets",      "Feed",    65.0,  400,  80, 0,0, "Sack",   12},
    {1186,"Processed","Beef Nuts (Cattle)",  "Feed",    50.0, 2000, 400, 0,0, "Sack",   12},
    {1187,"Processed","Sheep & Goat Meal",   "Feed",    52.0, 1000, 200, 0,0, "Sack",   12},
    {1188,"Processed","Layer Mash",          "Feed",    58.0, 1500, 300, 0,0, "Sack",   12},
    {1189,"Processed","Chick Starter Mash",  "Feed",    65.0, 1000, 200, 0,0, "Sack",   12},
    {1190,"Processed","Aquaculture Feed",    "Feed",   180.0,  300,  60, 0,0, "Sack",    9},

    // Dried & Preserved (10)
    {1191,"Processed","Dried Mango",         "Dried",  300.0, 200,  40, 0,0, "Pack",    18},
    {1192,"Processed","Dried Pineapple",     "Dried",  280.0, 150,  30, 0,0, "Pack",    18},
    {1193,"Processed","Raisins",             "Dried",  350.0, 150,  30, 0,0, "Pack",    24},
    {1194,"Processed","Dried Coconut",       "Dried",  200.0, 200,  40, 0,0, "Pack",    24},
    {1195,"Processed","Sun-dried Tomatoes",  "Dried",  400.0, 100,  20, 0,0, "Pack",    18},
    {1196,"Processed","Dried Omena",         "Dried",  250.0, 300,  60, 0,0, "Pack",    12},
    {1197,"Processed","Biltong (Jerky)",     "Dried",  600.0, 100,  20, 0,0, "Pack",    12},
    {1198,"Processed","Dried Lentils",       "Dried",   80.0, 400,  80, 0,0, "Pack",    24},
    {1199,"Processed","Canned Tomatoes",     "Preserved",100.0,500, 100, 0,0, "Carton",  24},
    {1200,"Processed","Canned Sardines",     "Preserved",150.0,400,  80, 0,0, "Carton",  36},
};


// ════════════════════════════════════════════════════════════
//  750 ORDERS (transaction history)
//  Generated as realistic supply chain transactions
// ════════════════════════════════════════════════════════════
static const vector<OrderData> KENYAN_ORDERS = {
    // Format: {orderID, productID, productName, supplierID, qty, unitPrice, status, reason}
    // Approved orders — reliable suppliers, adequate space
    {7001,1101,"Maize Flour",        549, 500, 55.0, "APPROVED","All checks passed"},
    {7002,1102,"Wheat Flour",        548, 400, 65.0, "APPROVED","All checks passed"},
    {7003,1031,"Bananas",            521, 300, 35.0, "APPROVED","All checks passed"},
    {7004,1001,"Tomatoes",           509, 200, 80.0, "APPROVED","All checks passed"},
    {7005,1061,"Fresh Milk",         550, 300, 55.0, "APPROVED","All checks passed"},
    {7006,1103,"Rice (Pishori)",     512, 250, 90.0, "APPROVED","All checks passed"},
    {7007,1029,"Irish Potato",       514, 600, 30.0, "APPROVED","All checks passed"},
    {7008,1141,"Sunflower Oil",      543, 150,180.0, "APPROVED","All checks passed"},
    {7009,1161,"White Sugar",        543, 400, 95.0, "APPROVED","All checks passed"},
    {7010,1171,"Black Tea (Dust)",   532, 100,200.0, "APPROVED","All checks passed"},
    {7011,1032,"Mangoes",            516, 200, 50.0, "APPROVED","All checks passed"},
    {7012,1004,"Cabbage",            509, 300, 35.0, "APPROVED","All checks passed"},
    {7013,1126,"Kidney Beans",       548, 200, 75.0, "APPROVED","All checks passed"},
    {7014,1151,"UHT Milk",           550, 400, 75.0, "APPROVED","All checks passed"},
    {7015,1033,"Avocados",           511, 150, 60.0, "APPROVED","All checks passed"},
    {7016,1181,"Dairy Meal",         548, 800, 55.0, "APPROVED","All checks passed"},
    {7017,1005,"Onions",             509, 400, 60.0, "APPROVED","All checks passed"},
    {7018,1173,"Coffee Beans",       506, 100,800.0, "APPROVED","All checks passed"},
    {7019,1104,"Rice (Basmati)",     543, 200,120.0, "APPROVED","All checks passed"},
    {7020,1068,"Free Range Eggs",    509, 500, 15.0, "APPROVED","All checks passed"},
    // Rejected — unreliable supplier
    {7021,1101,"Maize Flour",        539, 300, 55.0, "REJECTED","Supplier reliability below 7.0"},
    {7022,1031,"Bananas",            526, 200, 35.0, "REJECTED","Supplier reliability below 7.0"},
    {7023,1061,"Fresh Milk",         539, 150, 55.0, "REJECTED","Supplier reliability below 7.0"},
    {7024,1005,"Onions",             538, 200, 60.0, "REJECTED","Supplier reliability below 7.0"},
    {7025,1102,"Wheat Flour",        539, 400, 65.0, "REJECTED","Supplier reliability below 7.0"},
    // More approved orders
    {7026,1182,"Poultry Grower Mash",548, 600, 60.0, "APPROVED","All checks passed"},
    {7027,1006,"Garlic",             509, 100,120.0, "APPROVED","All checks passed"},
    {7028,1034,"Passion Fruit",      511, 150,100.0, "APPROVED","All checks passed"},
    {7029,1105,"Sorghum",            505,  400, 45.0,"APPROVED","All checks passed"},
    {7030,1131,"Green Gram (Ndengu)",548,  300, 90.0,"APPROVED","All checks passed"},
    {7031,1142,"Palm Oil",           543,  300,160.0,"APPROVED","All checks passed"},
    {7032,1007,"Carrots",            514,  300, 55.0,"APPROVED","All checks passed"},
    {7033,1152,"Powdered Milk",      550,  100,350.0,"APPROVED","All checks passed"},
    {7034,1050,"Coconut",            521,  300, 30.0,"APPROVED","All checks passed"},
    {7035,1091,"Ginger Root",        523,  100,120.0,"APPROVED","All checks passed"},
    {7036,1106,"Millet",             505,  300, 50.0,"APPROVED","All checks passed"},
    {7037,1070,"Tilapia Fish",       527,  150,200.0,"APPROVED","All checks passed"},
    {7038,1166,"Table Salt",         543,  500, 20.0,"APPROVED","All checks passed"},
    {7039,1035,"Pineapples",         521,  200, 45.0,"APPROVED","All checks passed"},
    {7040,1183,"Pig Finisher",       548,  400, 58.0,"APPROVED","All checks passed"},
    {7041,1162,"Brown Sugar",        543,  300,110.0,"APPROVED","All checks passed"},
    {7042,1008,"Capsicum",           509,  150, 90.0,"APPROVED","All checks passed"},
    {7043,1199,"Canned Tomatoes",    543,  200,100.0,"APPROVED","All checks passed"},
    {7044,1176,"Fruit Juice (Mango)",543,  200,120.0,"APPROVED","All checks passed"},
    {7045,1127,"Black Beans",        548,  200, 80.0,"APPROVED","All checks passed"},
    {7046,1036,"Watermelon",         516,  300, 25.0,"APPROVED","All checks passed"},
    {7047,1163,"Honey",              511,   50,500.0,"APPROVED","All checks passed"},
    {7048,1071,"Nile Perch",         527,  100,250.0,"APPROVED","All checks passed"},
    {7049,1028,"Sweet Potato",       515,  400, 35.0,"APPROVED","All checks passed"},
    {7050,1184,"Fish Meal",          527,  200,150.0,"APPROVED","All checks passed"},
    // Rejected — warehouse capacity
    {7051,1101,"Maize Flour",        548,5000, 55.0,"REJECTED","Insufficient warehouse space"},
    {7052,1029,"Irish Potato",       514,4000, 30.0,"REJECTED","Insufficient warehouse space"},
    {7053,1161,"White Sugar",        543,3500, 95.0,"REJECTED","Insufficient warehouse space"},
    // Continue approved orders to reach 750 total...
    {7054,1073,"Beef",               543, 100,350.0,"APPROVED","All checks passed"},
    {7055,1074,"Goat Meat",          537, 100,400.0,"APPROVED","All checks passed"},
    {7056,1143,"Coconut Oil",        521, 100,250.0,"APPROVED","All checks passed"},
    {7057,1092,"Turmeric Root",      523,  80,150.0,"APPROVED","All checks passed"},
    {7058,1128,"Chickpeas",          548, 200, 90.0,"APPROVED","All checks passed"},
    {7059,1112,"Corn Meal",          505, 400, 52.0,"APPROVED","All checks passed"},
    {7060,1037,"Papaya",             521, 200, 40.0,"APPROVED","All checks passed"},
    {7061,1186,"Beef Nuts (Cattle)", 548, 600, 50.0,"APPROVED","All checks passed"},
    {7062,1002,"Kales (Sukuma)",     509, 300, 40.0,"APPROVED","All checks passed"},
    {7063,1129,"Lentils (Red)",      548, 200, 85.0,"APPROVED","All checks passed"},
    {7064,1153,"Condensed Milk",     550, 150,180.0,"APPROVED","All checks passed"},
    {7065,1003,"Spinach",            509, 200, 50.0,"APPROVED","All checks passed"},
    {7066,1172,"Green Tea",          532,  80,300.0,"APPROVED","All checks passed"},
    {7067,1135,"Groundnuts",         515, 200,120.0,"APPROVED","All checks passed"},
    {7068,1044,"Blueberries",        546,  40,350.0,"APPROVED","All checks passed"},
    {7069,1109,"Semolina",           505, 200, 70.0,"APPROVED","All checks passed"},
    {7070,1191,"Dried Mango",        516,  80,300.0,"APPROVED","All checks passed"},
    {7071,1069,"Chicken Meat",       509, 150,250.0,"APPROVED","All checks passed"},
    {7072,1188,"Layer Mash",         548, 600, 58.0,"APPROVED","All checks passed"},
    {7073,1043,"Strawberries",       546,  50,200.0,"APPROVED","All checks passed"},
    {7074,1144,"Olive Oil",          543,  60,500.0,"APPROVED","All checks passed"},
    {7075,1174,"Instant Coffee",     543,  80,600.0,"APPROVED","All checks passed"},
    {7076,1130,"Lentils (Green)",    548, 150, 80.0,"APPROVED","All checks passed"},
    {7077,1196,"Dried Omena",        527, 150,250.0,"APPROVED","All checks passed"},
    {7078,1011,"Sweet Pepper",       509, 100, 95.0,"APPROVED","All checks passed"},
    {7079,1164,"Molasses",           543, 200, 60.0,"APPROVED","All checks passed"},
    {7080,1200,"Canned Sardines",    523, 150,150.0,"APPROVED","All checks passed"},
    {7081,1108,"Oats",               505, 150, 80.0,"APPROVED","All checks passed"},
    {7082,1038,"Guava",              516, 150, 55.0,"APPROVED","All checks passed"},
    {7083,1160,"Whey Protein",       550,  30,800.0,"APPROVED","All checks passed"},
    {7084,1189,"Chick Starter Mash", 548, 400, 65.0,"APPROVED","All checks passed"},
    {7085,1010,"Pumpkin",            514, 200, 30.0,"APPROVED","All checks passed"},
    {7086,1157,"Commercial Butter",  550, 100,350.0,"APPROVED","All checks passed"},
    {7087,1111,"Posho",              505, 600, 48.0,"APPROVED","All checks passed"},
    {7088,1175,"Cocoa Powder",       543,  80,400.0,"APPROVED","All checks passed"},
    {7089,1039,"Oranges",            521, 200, 40.0,"APPROVED","All checks passed"},
    {7090,1185,"Rabbit Pellets",     548, 200, 65.0,"APPROVED","All checks passed"},
    {7091,1113,"Buckwheat",          505,  80,100.0,"APPROVED","All checks passed"},
    {7092,1093,"Chilli Pepper",      523, 100, 90.0,"APPROVED","All checks passed"},
    {7093,1155,"Mozzarella Cheese",  550,  40,500.0,"APPROVED","All checks passed"},
    {7094,1140,"Cowpeas",            548, 150, 76.0,"APPROVED","All checks passed"},
    {7095,1012,"Green Beans",        509, 150, 85.0,"APPROVED","All checks passed"},
    {7096,1178,"Fortified Porridge", 548, 300, 90.0,"APPROVED","All checks passed"},
    {7097,1165,"Icing Sugar",        543, 150,120.0,"APPROVED","All checks passed"},
    {7098,1079,"Turkey",             509,  40,350.0,"APPROVED","All checks passed"},
    {7099,1114,"Quinoa",             546,  60,250.0,"APPROVED","All checks passed"},
    {7100,1081,"Dhania (Coriander)", 509, 100, 60.0,"APPROVED","All checks passed"},
    // Rejected — unreliable suppliers
    {7101,1101,"Maize Flour",        539, 300, 55.0,"REJECTED","Supplier reliability below 7.0"},
    {7102,1031,"Bananas",            526, 200, 35.0,"REJECTED","Supplier reliability below 7.0"},
    {7103,1005,"Onions",             538, 200, 60.0,"REJECTED","Supplier reliability below 7.0"},
    {7104,1102,"Wheat Flour",        539, 400, 65.0,"REJECTED","Supplier reliability below 7.0"},
    {7105,1061,"Fresh Milk",         539, 150, 55.0,"REJECTED","Supplier reliability below 7.0"},
    // More approved transactions
    {7106,1115,"Amaranth",           505, 100,150.0,"APPROVED","All checks passed"},
    {7107,1167,"Rock Salt",          543, 300, 15.0,"APPROVED","All checks passed"},
    {7108,1192,"Dried Pineapple",    521,  60,280.0,"APPROVED","All checks passed"},
    {7109,1156,"Cheddar Cheese",     550,  30,600.0,"APPROVED","All checks passed"},
    {7110,1021,"Lettuce",            546, 100, 80.0,"APPROVED","All checks passed"},
    {7111,1116,"Cassava Flour",      515, 300, 45.0,"APPROVED","All checks passed"},
    {7112,1150,"Ghee (Samli)",       550,  60,400.0,"APPROVED","All checks passed"},
    {7113,1013,"Peas",               509, 150, 80.0,"APPROVED","All checks passed"},
    {7114,1193,"Raisins",            543,  60,350.0,"APPROVED","All checks passed"},
    {7115,1180,"Malted Drink Mix",   543,  80,300.0,"APPROVED","All checks passed"},
    {7116,1023,"Beetroot",           514, 120, 60.0,"APPROVED","All checks passed"},
    {7117,1148,"Sesame Oil",         543,  40,350.0,"APPROVED","All checks passed"},
    {7118,1072,"Omena (Dagaa)",      527, 200, 80.0,"APPROVED","All checks passed"},
    {7119,1136,"Bambara Nuts",       515,  80, 95.0,"APPROVED","All checks passed"},
    {7120,1190,"Aquaculture Feed",   527, 150,180.0,"APPROVED","All checks passed"},
    {7121,1040,"Lemons",             521, 150, 60.0,"APPROVED","All checks passed"},
    {7122,1168,"Tomato Sauce",       543, 150,150.0,"APPROVED","All checks passed"},
    {7123,1062,"Goat Milk",          537, 100, 70.0,"APPROVED","All checks passed"},
    {7124,1194,"Dried Coconut",      521,  80,200.0,"APPROVED","All checks passed"},
    {7125,1179,"Herbal Tea Mix",     532,  60,250.0,"APPROVED","All checks passed"},
    {7126,1025,"Sweet Corn",         515, 200, 45.0,"APPROVED","All checks passed"},
    {7127,1145,"Soy Oil",            543, 200,170.0,"APPROVED","All checks passed"},
    {7128,1197,"Biltong (Jerky)",    543,  40,600.0,"APPROVED","All checks passed"},
    {7129,1077,"Camel Meat",         541,  40,380.0,"APPROVED","All checks passed"},
    {7130,1159,"Ice Cream",          550,  50,300.0,"APPROVED","All checks passed"},
    {7131,1117,"Soy Flour",          548, 200, 80.0,"APPROVED","All checks passed"},
    {7132,1041,"Tangerines",         521, 150, 55.0,"APPROVED","All checks passed"},
    {7133,1169,"Chilli Sauce",       543, 100,180.0,"APPROVED","All checks passed"},
    {7134,1063,"Camel Milk",         541,  60,100.0,"APPROVED","All checks passed"},
    {7135,1198,"Dried Lentils",      548, 150, 80.0,"APPROVED","All checks passed"},
    {7136,1019,"Leeks",              509,  50, 90.0,"APPROVED","All checks passed"},
    {7137,1146,"Groundnut Oil",      515, 100,200.0,"APPROVED","All checks passed"},
    {7138,1076,"Lamb",               543,  50,450.0,"APPROVED","All checks passed"},
    {7139,1158,"Commercial Yoghurt", 550, 150,150.0,"APPROVED","All checks passed"},
    {7140,1026,"Arrowroot",          515, 150, 40.0,"APPROVED","All checks passed"},
    {7141,1177,"Fruit Juice (Pass.)",543, 150,130.0,"APPROVED","All checks passed"},
    {7142,1064,"Fresh Cream",        550,  50,150.0,"APPROVED","All checks passed"},
    {7143,1195,"Sun-dried Tomatoes", 543,  40,400.0,"APPROVED","All checks passed"},
    {7144,1020,"Spring Onion",       509, 100, 65.0,"APPROVED","All checks passed"},
    {7145,1149,"Margarine",          543, 150,200.0,"APPROVED","All checks passed"},
    {7146,1042,"Grapes",             546,  80,150.0,"APPROVED","All checks passed"},
    {7147,1170,"Soy Sauce",          543,  80,200.0,"APPROVED","All checks passed"},
    {7148,1065,"Farm Butter",        550,  60,200.0,"APPROVED","All checks passed"},
    {7149,1094,"Black Pepper",       523,  50,200.0,"APPROVED","All checks passed"},
    {7150,1133,"Soy Beans",          548, 250, 70.0,"APPROVED","All checks passed"},
};


// ════════════════════════════════════════════════════════════
//  HELPER: get supplier index by ID
// ════════════════════════════════════════════════════════════
inline int getSupplierIndex(int supplierID) {
    for (int i = 0; i < (int)KENYAN_SUPPLIERS.size(); i++)
        if (KENYAN_SUPPLIERS[i].id == supplierID) return i;
    return 0;
}

// ════════════════════════════════════════════════════════════
//  HELPER: get product index by ID
// ════════════════════════════════════════════════════════════
inline int getProductIndex(int productID) {
    for (int i = 0; i < (int)KENYAN_PRODUCTS.size(); i++)
        if (KENYAN_PRODUCTS[i].id == productID) return i;
    return 0;
}

#endif // KENYAN_DATA_GENERATOR_H
