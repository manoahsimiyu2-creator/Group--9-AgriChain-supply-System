/* Project: Agricultural Supply Chain System
   Milestone: 1 - Computational Foundations
   Description: Mapping real-world biosystems to computational models.
*/

#include <iostream>
#include <string>

using namespace std;

int main() {
    // 1. System State Variables (Mapping Requirement)
    string cropType = "Maize";      // Biological identity
    float soilMoisture = 18.2f;     // Physical state (% moisture)
    int growthDay = 1;              // Temporal state
    bool irrigationActive = false;  // System state

    cout << "--- AGRI-CHAIN SYSTEM INITIALIZED ---" << endl;
    cout << "Monitoring: " << cropType << " | Day: " << growthDay << endl;

    // 2. Basic Arithmetic & Control Flow
    // If moisture is below 20%, we need to calculate irrigation needs
    if (soilMoisture < 20.0) {
        cout << "[ALERT] Low moisture detected: " << soilMoisture << "%" << endl;
        
        // Arithmetic: Simple simulation of adding 10% moisture
        soilMoisture += 10.0;
        irrigationActive = true;
        
        cout << "[ACTION] Irrigation activated. New level: " << soilMoisture << "%" << endl;
    }

    system("pause");
    return 0;
}
