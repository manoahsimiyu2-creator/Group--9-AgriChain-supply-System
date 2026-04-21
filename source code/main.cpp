/* Project: AgriChain-Biosystems
   Milestone: 2 - Control Logic & Object Introduction
   Features: Encapsulation, Classes, and Simulation Loops
*/

#include <iostream>
#include <string>

using namespace std;

// Requirement: Introduction of Classes (The 'Blueprint' for our crops)
class Crop {
private:
    // Requirement: Encapsulation (Hiding data for safety)
    string species;
    float moistureLevel;
    int dayCount;

public:
    // Constructor: Initializes the object state
    Crop(string name, float startMoisture) {
        species = name;
        moistureLevel = startMoisture;
        dayCount = 1;
    }

    // Requirement: Decision-making logic
    void updateStatus() {
        cout << "Day " << dayCount << " Monitoring [" << species << "]" << endl;
        
        if (moistureLevel < 20.0) {
            cout << " >> ACTION: Moisture critical (" << moistureLevel << "%). Irrigation ON." << endl;
            moistureLevel += 15.0; // Adding water
        } else {
            cout << " >> STATUS: Moisture stable (" << moistureLevel << "%)." << endl;
        }
        
        moistureLevel -= 2.5; // Simulate daily natural water loss
        dayCount++;
        cout << "--------------------------------------------" << endl;
    }
};

int main() {
    // Requirement: Input Validation & User Interaction
    float initialSensorData;
    cout << "=== AGRI-CHAIN BIOSYSTEMS: MILESTONE 2 ===" << endl;
    cout << "Enter initial sensor moisture reading (0-100): ";
    cin >> initialSensorData;

    // Basic validation
    if (initialSensorData < 0 || initialSensorData > 100) {
        cout << "Sensor Error! Defaulting to 25.0%" << endl;
        initialSensorData = 25.0;
    }

    // Requirement: Object Introduction
    Crop myWheat("Wheat-Field-A1", initialSensorData);

    // Requirement: Loop-based simulation cycler (Simulating 5 days)
    for (int i = 0; i < 5; i++) {
        myWheat.updateStatus();
    }

    system("pause");
    return 0;
}
