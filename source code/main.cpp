#include <iostream>
#include <string>
#include <iomanip> // For nice decimal formatting

using namespace std;

class CropNode {
private:
    string batchID;
    float moisture;
    int approvedCycles;
    int totalCycles;

public:
    CropNode(string id, float startMoisture) {
        batchID = id;
        moisture = startMoisture;
        approvedCycles = 0;
        totalCycles = 0;
    }

    void runQualityCheck() {
        totalCycles++;
        cout << "Running Check for Batch: " << batchID << "..." << endl;
        
        if (moisture >= 20.0 && moisture <= 60.0) {
            cout << " >> [PASS] Quality standard met." << endl;
            approvedCycles++;
        } else {
            cout << " >> [FAIL] Quality out of bounds. Irrigation/Drainage required." << endl;
            // Adjustment logic
            if (moisture < 20.0) moisture += 15.0;
            else moisture -= 10.0;
        }
        moisture -= 2.0; // Environmental depletion
    }

    // This mimics your Dashboard screenshot!
    void displayDashboard() {
        float rate = (float)approvedCycles / totalCycles * 100;
        
        cout << "\n===========================================" << endl;
        cout << "      AGRI-CHAIN QUALITY DASHBOARD         " << endl;
        cout << "===========================================" << endl;
        cout << " Batch ID:    " << batchID << endl;
        cout << " Total Checks: " << totalCycles << endl;
        cout << " Approved:     " << approvedCycles << endl;
        cout << " Rejected:     " << (totalCycles - approvedCycles) << endl;
        cout << " Approve Rate: " << fixed << setprecision(1) << rate << "%" << endl;
        cout << "===========================================\n" << endl;
    }
};

int main() {
    float sensorInput;
    cout << "Initialize System - Enter Sensor Reading: ";
    cin >> sensorInput;

    CropNode batch01("JUJA-WHEAT-001", sensorInput);

    // Simulate 6 batches/cycles to match your screenshot
    for(int i = 0; i < 6; i++) {
        batch01.runQualityCheck();
    }

    batch01.displayDashboard();

    system("pause");
    return 0;
}
