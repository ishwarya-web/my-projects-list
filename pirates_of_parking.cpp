#include <iostream>

#include <vector>

#include <string>

#include <iomanip>

#include <ctime>

#include <cmath>

#include <sstream>
#include <fstream>
#include <stdexcept>
#include <memory>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

using namespace std;

// Read a passcode while displaying * instead of the actual characters.
string readMaskedPassword()
{
    string password;
    char ch;
    cout.flush();
#ifdef _WIN32
    while ((ch = _getch()) != '\r')
    {
        if (ch == '\b')
        {
            if (!password.empty())
            {
                password.pop_back();
                cout << "\b \b";
            }
        }
        else if (ch != 0 && ch != -32)
        {
            password += ch;
            cout << '*';
        }
    }
#else
    termios oldSettings, newSettings;
    tcgetattr(STDIN_FILENO, &oldSettings);
    newSettings = oldSettings;
    newSettings.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);
    while (read(STDIN_FILENO, &ch, 1) == 1 && ch != '\n' && ch != '\r')
    {
        if (ch == 127 || ch == '\b')
        {
            if (!password.empty())
            {
                password.pop_back();
                cout << "\b \b";
                cout.flush();
            }
        }
        else if (ch >= 32 && ch <= 126)
        {
            password += ch;
            cout << '*';
            cout.flush();
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldSettings);
#endif
    cout << endl;
    return password;
}


// ============================================================

// SMART PARKING LOT MANAGEMENT SYSTEM

// ============================================================

//

// 1. Vehicle Entry

// 2. Vehicle Exit

// 3. Check Slot Availability

// 4. Reserve Parking Slot

// 5. Buy Monthly Parking Pass

// 6. Parking History

// 7. Pricing Information

// 8. Active Monthly Passes

//

// Features:

// - Planned parking duration

// - Advance parking payment

// - Automatic entry time

// - Automatic exit time

// - Automatic actual duration calculation

// - Minimum 1 hour charge

// - Minimum charge depends on vehicle type

// - Refund

// - Additional payment

// - Monthly parking pass

// - Monthly pass code

// - EV charging

// - EV charging payment at exit

// - Reserved slots

// - Dynamic pricing

//

// CHANGE:

// - Monthly pass can be purchased even if all slots

//   for that vehicle type are currently full.

//

// ============================================================

// ============================================================

// ENUMS

// ============================================================

// Integer constants are used instead of enums.
const int BIKE = 0;
const int CAR = 1;
const int SUV = 2;
const int EV = 3;

// ============================================================
// INHERITANCE + RUNTIME POLYMORPHISM FOR VEHICLE PRICING
// Existing rates and parking behaviour are kept unchanged.
// ============================================================
class VehiclePricing
{
public:
    virtual double hourlyRate() const = 0;
    virtual double monthlyPassRate() const = 0;
    virtual ~VehiclePricing() {}
};

class BikePricing : public VehiclePricing
{
public:
    double hourlyRate() const override { return 20.0; }
    double monthlyPassRate() const override { return 1000.0; }
};

class CarPricing : public VehiclePricing
{
public:
    double hourlyRate() const override { return 40.0; }
    double monthlyPassRate() const override { return 2000.0; }
};

class SUVPricing : public VehiclePricing
{
public:
    double hourlyRate() const override { return 60.0; }
    double monthlyPassRate() const override { return 3000.0; }
};

class EVPricing : public VehiclePricing
{
public:
    double hourlyRate() const override { return 50.0; }
    double monthlyPassRate() const override { return 2500.0; }
};

unique_ptr<VehiclePricing> createVehiclePricing(int type)
{
    switch (type)
    {
    case BIKE: return make_unique<BikePricing>();
    case CAR:  return make_unique<CarPricing>();
    case SUV:  return make_unique<SUVPricing>();
    case EV:   return make_unique<EVPricing>();
    default:   return make_unique<CarPricing>();
    }
}

// ============================================================

// PARKED VEHICLE

// ============================================================

class ParkedVehicle
{
public:
    string vehicleNumber;

    int vehicleType;

    int slotNumber;

    time_t entryTime;

    double plannedHours;

    double amountPaid;

    bool monthlyPassUsed;

    bool needsEVCharging;

    bool active;
};

// ============================================================

// PARKING HISTORY

// ============================================================

class ParkingHistory
{
public:
    string vehicleNumber;

    int slotNumber;

    int vehicleType;

    time_t entryTime;

    time_t exitTime;

    double plannedHours;

    double actualHours;

    double billableHours;

    double amountPaidAtEntry;

    double parkingCharge;

    double evChargingUnits;

    double evChargingCost;

    double finalAmount;

    double refund;

    double additionalPayment;

    bool monthlyPassUsed;

    bool active;
};

// ============================================================

// PARKING SLOT

// ============================================================

class ParkingSlot
{
public:
    int slotNumber;

    int slotType;

    bool occupied;

    bool reserved;

    string reservedVehicle;

    string currentVehicle;
};

// Paid reservation with an allowed arrival window

class Reservation
{
public:
    string vehicleNumber;

    int slotNumber;

    double plannedHours;

    double advancePaid;

    time_t arrivalStart;

    time_t arrivalEnd;

    bool active;
};

// ============================================================

// MONTHLY PASS

// ============================================================

class MonthlyPass
{
private:
    string passCode;

public:
    string vehicleNumber;
    string ownerName;
    int vehicleType;
    string startDate;
    string expiryDate;
    bool active;
    void setPassCode(const string &code) { passCode = code; }
    string getPassCode() const { return passCode; }
    bool checkPassCode(const string &code) const { return passCode == code; }
};

// ============================================================

// HELPER FUNCTIONS

// ============================================================

// Monthly pass code encryption helpers (XOR + hexadecimal encoding).
// Educational demonstration only; use a vetted crypto library for real security.
string encryptPassCode(const string &plainText)
{
    const string key = "SmartParkingKey";
    const string digits = "0123456789ABCDEF";
    string result;
    for (size_t i = 0; i < plainText.size(); ++i)
    {
        unsigned char value = static_cast<unsigned char>(plainText[i] ^ key[i % key.size()]);
        result += digits[value >> 4];
        result += digits[value & 15];
    }
    return result;
}

string decryptPassCode(const string &encryptedHex)
{
    const string key = "SmartParkingKey";
    const string digits = "0123456789ABCDEF";
    string result;
    if (encryptedHex.size() % 2 != 0)
        throw runtime_error("Invalid encrypted pass code.");
    for (size_t i = 0; i < encryptedHex.size(); i += 2)
    {
        size_t high = digits.find(encryptedHex[i]);
        size_t low = digits.find(encryptedHex[i + 1]);
        if (high == string::npos || low == string::npos)
            throw runtime_error("Invalid encrypted pass code.");
        char value = static_cast<char>((high << 4) | low);
        result += static_cast<char>(value ^ key[(i / 2) % key.size()]);
    }
    return result;
}

string formatDateTime(time_t timeValue)

{

    tm *localTime = localtime(&timeValue);

    char buffer[100];

    strftime(

        buffer,

        sizeof(buffer),

        "%d-%m-%Y %H:%M:%S",

        localTime);

    return string(buffer);
}

// ============================================================

// FORMAT DATE

// ============================================================

string formatDate(time_t timeValue)

{

    tm *localTime = localtime(&timeValue);

    char buffer[100];

    strftime(

        buffer,

        sizeof(buffer),

        "%d-%m-%Y",

        localTime);

    return string(buffer);
}

// ============================================================

// ADD DAYS TO CURRENT TIME

// ============================================================

string formatExpiryDate(time_t currentTime, int days)

{

    time_t expiryTime =

        currentTime + (days * 24 * 60 * 60);

    return formatDate(expiryTime);
}

// ============================================================

// VEHICLE TYPE TO STRING

// ============================================================

string vehicleTypeToString(int type)

{

    switch (type)

    {

    case BIKE:

        return "Bike";

    case CAR:

        return "Car";

    case SUV:

        return "SUV";

    case EV:

        return "EV";
    }

    return "Unknown";
}

// ============================================================

// SLOT TYPE TO STRING

// ============================================================

string slotTypeToString(int type)

{

    switch (type)

    {

    case BIKE:

        return "Bike";

    case CAR:

        return "Car";

    case SUV:

        return "SUV";

    case EV:

        return "EV";
    }

    return "Unknown";
}

// ============================================================

// GET VEHICLE TYPE

// ============================================================

bool getintFromChoice(

    int choice,

    int &type)

{

    switch (choice)

    {

    case 1:

        type = BIKE;

        return true;

    case 2:

        type = CAR;

        return true;

    case 3:

        type = SUV;

        return true;

    case 4:

        type = EV;

        return true;

    default:

        return false;
    }
}

// ============================================================

// PARKING SYSTEM CLASS

// ============================================================

class ParkingSystem

{

private:
    vector<ParkingSlot> slots;

    vector<ParkedVehicle> parkedVehicles;

    vector<ParkingHistory> history;

    vector<MonthlyPass> monthlyPasses;

    vector<Reservation> reservations;

    // ========================================================

    // HOURLY PARKING RATES

    // ========================================================

    double bikeRate = 20.0;

    double carRate = 40.0;

    double suvRate = 60.0;

    double evRate = 50.0;

    // ========================================================

    // EV CHARGING RATE

    // ========================================================

    double evChargingRate = 15.0;

    // ========================================================

    // MONTHLY PASS RATES

    // ========================================================

    double bikeMonthlyPass = 1000.0;

    double carMonthlyPass = 2000.0;

    double suvMonthlyPass = 3000.0;

    double evMonthlyPass = 2500.0;

public:
    // ========================================================

    // RESERVATION HELPERS

    // ========================================================

    bool makeDateTime(

        int day, int month, int year,

        int hour, int minute, time_t &result)

    {

        tm value = {};

        value.tm_mday = day;

        value.tm_mon = month - 1;

        value.tm_year = year - 1900;

        value.tm_hour = hour;

        value.tm_min = minute;

        value.tm_sec = 0;

        value.tm_isdst = -1;

        result = mktime(&value);

        if (result == -1)

            return false;

        tm *check = localtime(&result);

        return check &&

               check->tm_mday == day &&

               check->tm_mon == month - 1 &&

               check->tm_year == year - 1900 &&

               check->tm_hour == hour &&

               check->tm_min == minute;
    }

    void expireReservations()

    {

        time_t now = time(nullptr);

        for (auto &reservation : reservations)

        {

            if (reservation.active && now >= reservation.arrivalEnd)

            {

                reservation.active = false;

                for (auto &slot : slots)

                {

                    if (slot.slotNumber == reservation.slotNumber &&

                        slot.reserved &&

                        slot.reservedVehicle == reservation.vehicleNumber)

                    {

                        slot.reserved = false;

                        slot.reservedVehicle = "";

                        break;
                    }
                }
            }
        }
    }

    int findReservation(string vehicleNumber)

    {

        for (int i = 0; i < static_cast<int>(reservations.size()); i++)

        {

            if (reservations[i].active &&

                reservations[i].vehicleNumber == vehicleNumber)

            {

                return i;
            }
        }

        return -1;
    }

    // ========================================================

    // CONSTRUCTOR

    // ========================================================

    // Save all records in three separate text files. Each file starts with a
    // readable report; the #DATA section at the end preserves records for reload.
    void saveMonthlyPassesToFile()
    {
        saveAllData();
    }

    void saveAllData()
    {
        // Parking history file: history + currently parked vehicles.
        {
            ofstream file("parkinghistory.txt");
            if (!file) throw runtime_error("Could not open parkinghistory.txt for writing.");
            file << "PARKING HISTORY\n---------------\n";
            if (history.empty()) file << "No parking records recorded.\n\n";
            for (const auto &r : history)
            {
                file << "Vehicle number       : " << r.vehicleNumber << '\n'
                     << "Slot number          : " << r.slotNumber << '\n'
                     << "Vehicle type         : " << vehicleTypeToString(r.vehicleType) << '\n'
                     << "Entry time           : " << timeToText(r.entryTime) << '\n'
                     << "Exit time            : " << timeToText(r.exitTime) << '\n'
                     << fixed << setprecision(2)
                     << "Planned hours        : " << r.plannedHours << '\n'
                     << "Actual hours         : " << r.actualHours << '\n'
                     << "Billable hours       : " << r.billableHours << '\n'
                     << "Paid at entry        : Rs. " << r.amountPaidAtEntry << '\n'
                     << "Parking charge       : Rs. " << r.parkingCharge << '\n'
                     << "EV charging units    : " << (r.active && r.evChargingUnits < 0 ? "Charging requested (units pending)" : to_string(r.evChargingUnits)) << '\n'
                     << "EV charging cost     : Rs. " << r.evChargingCost << '\n'
                     << "Final amount         : Rs. " << r.finalAmount << '\n'
                     << "Refund               : Rs. " << r.refund << '\n'
                     << "Additional payment   : Rs. " << r.additionalPayment << '\n'
                     << "Monthly pass used    : " << (r.monthlyPassUsed ? "Yes" : "No") << '\n'
                     << "Record status        : " << (r.active ? "Active" : "Completed") << "\n\n";
            }
            file << "CURRENTLY PARKED VEHICLES\n-------------------------\n";
            bool anyParked = false;
            for (const auto &v : parkedVehicles) if (v.active)
            {
                anyParked = true;
                file << "Vehicle number : " << v.vehicleNumber << '\n'
                     << "Vehicle type   : " << vehicleTypeToString(v.vehicleType) << '\n'
                     << "Slot number    : " << v.slotNumber << '\n'
                     << "Entry time     : " << timeToText(v.entryTime) << '\n'
                     << fixed << setprecision(2)
                     << "Planned hours  : " << v.plannedHours << '\n'
                     << "Amount paid    : Rs. " << v.amountPaid << "\n\n";
            }
            if (!anyParked) file << "No vehicles currently parked.\n";
            file << "\n#DATA\n";
            file << fixed << setprecision(2);
            for (const auto &r : history)
                file << quoted(r.vehicleNumber) << ' ' << r.slotNumber << ' '
                     << r.vehicleType << ' ' << static_cast<long long>(r.entryTime) << ' '
                     << static_cast<long long>(r.exitTime) << ' ' << r.plannedHours << ' '
                     << r.actualHours << ' ' << r.billableHours << ' '
                     << r.amountPaidAtEntry << ' ' << r.parkingCharge << ' '
                     << r.evChargingUnits << ' ' << r.evChargingCost << ' '
                     << r.finalAmount << ' ' << r.refund << ' ' << r.additionalPayment << ' '
                     << r.monthlyPassUsed << ' ' << r.active << '\n';
        }

        // Reservation file.
        {
            ofstream file("reserveslots.txt");
            if (!file) throw runtime_error("Could not open reserveslots.txt for writing.");
            file << "RESERVATIONS\n------------\n";
            if (reservations.empty()) file << "No reservations recorded.\n\n";
            for (const auto &r : reservations)
                file << "Vehicle number : " << r.vehicleNumber << '\n'
                     << "Slot number    : " << r.slotNumber << '\n'
                     << fixed << setprecision(2)
                     << "Planned hours  : " << r.plannedHours << '\n'
                     << "Advance paid   : Rs. " << r.advancePaid << '\n'
                     << "Arrival starts : " << timeToText(r.arrivalStart) << '\n'
                     << "Arrival ends   : " << timeToText(r.arrivalEnd) << '\n'
                     << "Status         : " << (r.active ? "Active" : "Inactive") << "\n\n";
            file << "#DATA\n" << fixed << setprecision(2);
            for (const auto &r : reservations)
                file << quoted(r.vehicleNumber) << ' ' << r.slotNumber << ' '
                     << r.plannedHours << ' ' << r.advancePaid << ' '
                     << static_cast<long long>(r.arrivalStart) << ' '
                     << static_cast<long long>(r.arrivalEnd) << ' ' << r.active << '\n';
        }

        // Monthly pass file.
        {
            ofstream file("monthlypass.txt");
            if (!file) throw runtime_error("Could not open monthlypass.txt for writing.");
            file << "MONTHLY PASSES\n-------------\n";
            if (monthlyPasses.empty()) file << "No monthly passes recorded.\n\n";
            for (const auto &pass : monthlyPasses)
                file << "Vehicle number : " << pass.vehicleNumber << '\n'
                     << "Owner          : " << pass.ownerName << '\n'
                     << "Vehicle type   : " << vehicleTypeToString(pass.vehicleType) << '\n'
                     << "Pass code      : **** (masked)\n"
                     << "Start date     : " << pass.startDate << '\n'
                     << "Expiry date    : " << pass.expiryDate << '\n'
                     << "Status         : " << (pass.active ? "Active" : "Inactive") << "\n\n";
            file << "#DATA\n";
            for (const auto &pass : monthlyPasses)
                file << quoted(pass.vehicleNumber) << ' ' << quoted(pass.ownerName) << ' '
                     << static_cast<int>(pass.vehicleType) << ' '
                     << quoted(encryptPassCode(pass.getPassCode())) << ' '
                     << quoted(pass.startDate) << ' ' << quoted(pass.expiryDate) << ' '
                     << pass.active << '\n';
        }
    }

    void loadAllData()
    {
        // ParkingHistory.txt stores completed records and currently parked records.
        {
            ifstream file("parkinghistory.txt");
            string line;
            while (file && line != "#DATA") getline(file, line);
            while (file)
            {
                ParkingHistory r;
                long long entry, exit;
                if (!(file >> quoted(r.vehicleNumber) >> r.slotNumber >> r.vehicleType
                          >> entry >> exit >> r.plannedHours >> r.actualHours
                          >> r.billableHours >> r.amountPaidAtEntry >> r.parkingCharge
                          >> r.evChargingUnits >> r.evChargingCost >> r.finalAmount
                          >> r.refund >> r.additionalPayment >> r.monthlyPassUsed >> r.active))
                    break;
                r.entryTime = static_cast<time_t>(entry);
                r.exitTime = static_cast<time_t>(exit);
                history.push_back(r);
                if (r.active)
                {
                    ParkedVehicle v;
                    v.vehicleNumber = r.vehicleNumber;
                    v.vehicleType = r.vehicleType;
                    v.slotNumber = r.slotNumber;
                    v.entryTime = r.entryTime;
                    v.plannedHours = r.plannedHours;
                    v.amountPaid = r.amountPaidAtEntry;
                    v.monthlyPassUsed = r.monthlyPassUsed;
                    v.needsEVCharging = (r.evChargingUnits < 0);
                    v.active = true;
                    parkedVehicles.push_back(v);
                }
            }
        }
        {
            ifstream file("reserveslots.txt");
            string line;
            while (file && line != "#DATA") getline(file, line);
            while (file)
            {
                Reservation r;
                long long start, finish;
                if (!(file >> quoted(r.vehicleNumber) >> r.slotNumber >> r.plannedHours
                          >> r.advancePaid >> start >> finish >> r.active))
                    break;
                r.arrivalStart = static_cast<time_t>(start);
                r.arrivalEnd = static_cast<time_t>(finish);
                reservations.push_back(r);
            }
        }
        {
            ifstream file("monthlypass.txt");
            string line;
            while (file && line != "#DATA") getline(file, line);
            while (file)
            {
                MonthlyPass pass;
                int type;
                string encryptedCode;
                if (!(file >> quoted(pass.vehicleNumber) >> quoted(pass.ownerName) >> type
                          >> quoted(encryptedCode) >> quoted(pass.startDate)
                          >> quoted(pass.expiryDate) >> pass.active))
                    break;
                if (type < 0 || type > 3) continue;
                try
                {
                    pass.vehicleType = type;
                    pass.setPassCode(decryptPassCode(encryptedCode));
                    monthlyPasses.push_back(pass);
                }
                catch (const exception &e)
                {
                    cerr << "Skipping invalid monthly pass record: " << e.what() << endl;
                }
            }
        }
        for (auto &slot : slots)
        {
            slot.occupied = false;
            slot.reserved = false;
            slot.currentVehicle = "";
            slot.reservedVehicle = "";
        }
        for (const auto &v : parkedVehicles)
        {
            if (!v.active || v.slotNumber < 1 || v.slotNumber > static_cast<int>(slots.size())) continue;
            ParkingSlot &slot = slots[v.slotNumber - 1];
            slot.occupied = true;
            slot.currentVehicle = v.vehicleNumber;
        }
        for (const auto &r : reservations)
        {
            if (!r.active || r.slotNumber < 1 || r.slotNumber > static_cast<int>(slots.size())) continue;
            ParkingSlot &slot = slots[r.slotNumber - 1];
            if (!slot.occupied)
            {
                slot.reserved = true;
                slot.reservedVehicle = r.vehicleNumber;
            }
        }
        expireReservations();
    }

    string timeToText(time_t value) const
    {
        if (value == 0)
            return "N/A";
        tm *timeInfo = localtime(&value);
        if (!timeInfo)
            return "N/A";
        char buffer[32];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeInfo);
        return buffer;
    }



    ParkingSystem()
    {
        createSlots();
        loadAllData();
    }

    // ========================================================

    // CREATE PARKING SLOTS

    // ========================================================

    void createSlots()

    {

        // ----------------------------------------------------

        // BIKE SLOTS 1 - 5

        // ----------------------------------------------------

        for (int i = 1; i <= 5; i++)

        {

            ParkingSlot slot;

            slot.slotNumber = i;

            slot.slotType = BIKE;

            slot.occupied = false;

            slot.reserved = false;

            slot.reservedVehicle = "";

            slot.currentVehicle = "";

            slots.push_back(slot);
        }

        // ----------------------------------------------------

        // CAR SLOTS 6 - 15

        // ----------------------------------------------------

        for (int i = 6; i <= 15; i++)

        {

            ParkingSlot slot;

            slot.slotNumber = i;

            slot.slotType = CAR;

            slot.occupied = false;

            slot.reserved = false;

            slot.reservedVehicle = "";

            slot.currentVehicle = "";

            slots.push_back(slot);
        }

        // ----------------------------------------------------

        // SUV SLOTS 16 - 20

        // ----------------------------------------------------

        for (int i = 16; i <= 20; i++)

        {

            ParkingSlot slot;

            slot.slotNumber = i;

            slot.slotType = SUV;

            slot.occupied = false;

            slot.reserved = false;

            slot.reservedVehicle = "";

            slot.currentVehicle = "";

            slots.push_back(slot);
        }

        // ----------------------------------------------------

        // EV SLOTS 21 - 25

        // ----------------------------------------------------

        for (int i = 21; i <= 25; i++)

        {

            ParkingSlot slot;

            slot.slotNumber = i;

            slot.slotType = EV;

            slot.occupied = false;

            slot.reserved = false;

            slot.reservedVehicle = "";

            slot.currentVehicle = "";

            slots.push_back(slot);
        }
    }

    // ========================================================

    // GET REQUIRED SLOT TYPE

    // ========================================================

    int getRequiredSlot(int type)

    {

        switch (type)

        {

        case BIKE:

            return BIKE;

        case CAR:

            return CAR;

        case SUV:

            return SUV;

        case EV:

            return EV;
        }

        return CAR;
    }

    // ========================================================

    // GET HOURLY RATE

    // ========================================================

    double getBaseRate(int type)

    {
        // Virtual function call: the selected derived class supplies the rate.
        unique_ptr<VehiclePricing> vehicle = createVehiclePricing(type);
        return vehicle->hourlyRate();
    }

    // ========================================================

    // DYNAMIC PRICING

    // ========================================================

    double getDynamicMultiplier()

    {

        int occupied = 0;

        for (const auto &slot : slots)

        {

            if (slot.occupied)

            {

                occupied++;
            }
        }

        double occupancy =

            static_cast<double>(occupied) / slots.size();

        if (occupancy < 0.50)

        {

            return 1.00;
        }

        if (occupancy < 0.80)

        {

            return 1.25;
        }

        return 1.50;
    }

    // ========================================================

    // GET MINIMUM CHARGE

    // ========================================================

    double getMinimumCharge(int type)

    {

        double rate =

            getBaseRate(type);

        double multiplier =

            getDynamicMultiplier();

        return rate * multiplier;
    }

    // ========================================================

    // CHECK ACTIVE MONTHLY PASS

    // ========================================================

    bool hasMonthlyPass(string vehicleNumber)

    {

        for (const auto &pass : monthlyPasses)

        {

            if (

                pass.vehicleNumber == vehicleNumber &&

                pass.active)

            {

                return true;
            }
        }

        return false;
    }

    // ========================================================

    // VALIDATE MONTHLY PASS CODE

    // ========================================================

    bool validateMonthlyPass(

        string vehicleNumber,

        string passCode)

    {

        for (const auto &pass : monthlyPasses)

        {

            if (

                pass.vehicleNumber == vehicleNumber &&

                pass.checkPassCode(passCode) &&

                pass.active)

            {

                return true;
            }
        }

        return false;
    }

    // ========================================================

    // FIND MONTHLY PASS

    // ========================================================

    int findMonthlyPass(string vehicleNumber)

    {

        for (

            int i = 0;

            i < static_cast<int>(monthlyPasses.size());

            i++)

        {

            if (

                monthlyPasses[i].vehicleNumber == vehicleNumber &&

                monthlyPasses[i].active)

            {

                return i;
            }
        }

        return -1;
    }

    // ========================================================

    // FIND PARKED VEHICLE

    // ========================================================

    int findParkedVehicle(string vehicleNumber)

    {

        for (

            int i = 0;

            i < static_cast<int>(parkedVehicles.size());

            i++)

        {

            if (

                parkedVehicles[i].vehicleNumber == vehicleNumber &&

                parkedVehicles[i].active)

            {

                return i;
            }
        }

        return -1;
    }

    // ========================================================

    // FIND AVAILABLE SLOT

    // ========================================================

    int findAvailableSlot(

        int vehicleType,

        string vehicleNumber)

    {

        int requiredType =

            getRequiredSlot(vehicleType);

        for (auto &slot : slots)

        {

            if (

                slot.slotType == requiredType &&

                !slot.occupied)

            {

                if (

                    slot.reserved &&

                    slot.reservedVehicle != vehicleNumber)

                {

                    continue;
                }

                return slot.slotNumber;
            }
        }

        return -1;
    }

    // ========================================================

    // VEHICLE ENTRY

    // ========================================================

    void vehicleEntry()

    {

        cout << "\n";

        cout << "============================================\n";

        cout << "              VEHICLE ENTRY\n";

        cout << "============================================\n";

        string vehicleNumber;

        cout << "Enter vehicle number: ";

        cin >> vehicleNumber;

        expireReservations();

        if (findParkedVehicle(vehicleNumber) != -1)

        {

            cout << "\nVehicle is already inside.\n";

            return;
        }

        int reservationIndex = findReservation(vehicleNumber);

        int slotNumber = -1;

        double plannedHours = 0.0;

        double reservationAdvance = 0.0;

        int vehicleType;

        time_t now = time(nullptr);

        if (reservationIndex != -1)

        {

            Reservation &booking = reservations[reservationIndex];

            if (now < booking.arrivalStart)

            {

                cout << "\nYour reservation arrival window has not started yet.\n";

                cout << "You may enter from: "

                     << formatDateTime(booking.arrivalStart) << endl;

                return;
            }

            if (now > booking.arrivalEnd)

            {

                expireReservations();

                cout << "\nYour reservation has expired. The reserved slot is now available.\n";

                return;
            }

            slotNumber = booking.slotNumber;

            plannedHours = booking.plannedHours;

            reservationAdvance = booking.advancePaid;

            if (slotNumber < 1 || slotNumber > static_cast<int>(slots.size()))

            {

                cout << "\nReservation has an invalid slot number.\n";

                return;
            }

            ParkingSlot &reservedSlot = slots[slotNumber - 1];

            if (!reservedSlot.reserved ||

                reservedSlot.reservedVehicle != vehicleNumber ||

                reservedSlot.occupied)

            {

                cout << "\nThe reserved slot is not available.\n";

                return;
            }

            switch (reservedSlot.slotType)

            {

            case BIKE:

                vehicleType = BIKE;

                break;

            case CAR:

                vehicleType = CAR;

                break;

            case SUV:

                vehicleType = SUV;

                break;

            case EV:

                vehicleType = EV;

                break;
            }

            cout << "\nReservation found!\n";

            cout << "Reserved Slot: " << slotNumber << endl;

            cout << "Planned Hours: " << plannedHours << endl;
        }

        else
        {
            int typeChoice;

            cout << "\nVehicle Type:\n";
            cout << "1. Bike\n";
            cout << "2. Car\n";
            cout << "3. SUV\n";
            cout << "4. EV\n";

            cout << "\nEnter choice: ";
            cin >> typeChoice;

            if (!getintFromChoice(typeChoice, vehicleType))
            {
                cout << "\nInvalid vehicle type.\n";
                return;
            }

            // Check for a suitable slot BEFORE asking for parking duration.
            slotNumber = findAvailableSlot(vehicleType, vehicleNumber);

            if (slotNumber == -1)
            {
                cout << "\nNo suitable parking slot available for this vehicle type.\n";
                return;
            }

            cout << "\nEnter planned parking duration (hours): ";
            cin >> plannedHours;

            if (plannedHours <= 0)
            {
                cout << "\nInvalid duration.\n";
                return;
            }

            if (plannedHours < 1)
                plannedHours = 1;
        }

        char passChoice;

        cout << "\nDo you have a monthly parking pass? (y/n): ";

        cin >> passChoice;

        bool monthlyPassUsed = false;

        if (passChoice == 'y' || passChoice == 'Y')

        {

            cout << "Enter monthly pass code (characters hidden): ";
            string passCode = readMaskedPassword();

            if (validateMonthlyPass(vehicleNumber, passCode))

            {

                monthlyPassUsed = true;

                cout << "\nMonthly pass verified successfully.\n";
            }

            else

            {

                cout << "\nInvalid monthly pass code.\n";

                cout << "Vehicle entry cancelled.\n";

                return;
            }
        }

        bool needsEVCharging = false;
        if (vehicleType == EV)
        {
            char chargingChoice;
            cout << "Does this EV require charging? (y/n): ";
            cin >> chargingChoice;
            needsEVCharging = (chargingChoice == 'y' || chargingChoice == 'Y');
        }

        time_t entryTime = time(nullptr);

        double multiplier = getDynamicMultiplier();

        double rate = getBaseRate(vehicleType);

        double estimatedParkingCharge = 0;

        if (!monthlyPassUsed)

        {

            estimatedParkingCharge =

                plannedHours * rate * multiplier;

            double minimumCharge = getMinimumCharge(vehicleType);

            if (estimatedParkingCharge < minimumCharge)

                estimatedParkingCharge = minimumCharge;
        }

        cout << fixed << setprecision(2);

        cout << "\n============================================\n";

        cout << "             PARKING DETAILS\n";

        cout << "============================================\n";

        cout << "Vehicle Number      : " << vehicleNumber << endl;

        cout << "Vehicle Type        : " << vehicleTypeToString(vehicleType) << endl;

        cout << "Slot Number         : " << slotNumber << endl;

        cout << "Entry Time          : " << formatDateTime(entryTime) << endl;

        cout << "Planned Duration    : " << plannedHours << " hour(s)" << endl;

        if (monthlyPassUsed)

        {

            cout << "Monthly Pass        : ACTIVE" << endl;

            cout << "Parking Charge      : Rs. 0.00" << endl;

            cout << "Payment Required    : NO" << endl;
        }

        else

        {

            cout << "Hourly Rate         : Rs. " << rate << endl;

            cout << "Dynamic Multiplier  : " << multiplier << "x" << endl;

            cout << "Minimum Charge      : Rs. " << getMinimumCharge(vehicleType) << endl;

            cout << "Estimated Charge    : Rs. " << estimatedParkingCharge << endl;

            cout << "Payment Required    : YES" << endl;
        }

        cout << "============================================\n";

        double payment = 0;

        double entryRefund = 0;

        if (!monthlyPassUsed)

        {

            double remainingAmount = estimatedParkingCharge - reservationAdvance;

            if (remainingAmount < 0)

                remainingAmount = 0;

            if (reservationAdvance > 0)

            {

                cout << "\nReservation advance already paid: Rs. "

                     << reservationAdvance << endl;

                cout << "Remaining amount to pay: Rs. "

                     << remainingAmount << endl;
            }

            if (remainingAmount > 0)

            {

                cout << "Enter payment amount: Rs. ";

                cin >> payment;

                if (payment < remainingAmount)

                {

                    cout << "\nInsufficient payment.\n";

                    cout << "Required amount: Rs. " << remainingAmount << endl;

                    return;
                }
            }

            double totalPaid = reservationAdvance + payment;

            entryRefund = totalPaid - estimatedParkingCharge;

            if (entryRefund > 0)

                cout << "Change/Refund: Rs. " << entryRefund << endl;

            payment = totalPaid;
        }

        for (auto &slot : slots)

        {

            if (slot.slotNumber == slotNumber)

            {

                slot.occupied = true;

                slot.currentVehicle = vehicleNumber;

                if (reservationIndex != -1)

                {

                    slot.reserved = false;

                    slot.reservedVehicle = "";

                    reservations[reservationIndex].active = false;
                }

                break;
            }
        }

        ParkedVehicle vehicle;

        vehicle.vehicleNumber = vehicleNumber;

        vehicle.vehicleType = vehicleType;

        vehicle.slotNumber = slotNumber;

        vehicle.entryTime = entryTime;

        vehicle.plannedHours = plannedHours;

        vehicle.amountPaid = payment;

        vehicle.monthlyPassUsed = monthlyPassUsed;

        vehicle.needsEVCharging = needsEVCharging;

        vehicle.active = true;

        parkedVehicles.push_back(vehicle);

        // ADD CURRENT VEHICLE TO PARKING HISTORY

        ParkingHistory record;

        record.vehicleNumber = vehicleNumber;

        record.slotNumber = slotNumber;

        record.vehicleType = vehicleType;

        record.entryTime = entryTime;

        record.exitTime = 0;

        record.plannedHours = plannedHours;

        record.actualHours = 0;

        record.billableHours = 0;

        record.amountPaidAtEntry = payment;

        record.parkingCharge = estimatedParkingCharge;

        record.evChargingUnits = (vehicleType == EV && needsEVCharging) ? -1 : 0;

        record.evChargingCost = 0;

        record.finalAmount = 0;

        record.refund = entryRefund;

        record.additionalPayment = 0;

        record.monthlyPassUsed = monthlyPassUsed;

        record.active = true;

        history.push_back(record);

        cout << "\n============================================\n";

        cout << "             PARKING TICKET\n";

        cout << "============================================\n";

        cout << "Vehicle Number : " << vehicleNumber << endl;

        cout << "Vehicle Type   : " << vehicleTypeToString(vehicleType) << endl;

        cout << "Slot Number    : " << slotNumber << endl;

        cout << "Entry Time     : " << formatDateTime(entryTime) << endl;

        cout << "Planned Time   : " << plannedHours << " hour(s)" << endl;

        cout << "Amount Paid    : Rs. " << payment << endl;

        if (reservationAdvance > 0)

            cout << "Advance Paid   : Rs. " << reservationAdvance << endl;

        cout << "============================================\n";

        cout << "\nVehicle successfully parked.\n";
    }

    // ========================================================

    // VEHICLE EXIT

    // ========================================================

    void vehicleExit()

    {

        cout << "\n";

        cout << "============================================\n";

        cout << "               VEHICLE EXIT\n";

        cout << "============================================\n";

        string vehicleNumber;

        cout << "Enter vehicle number: ";

        cin >> vehicleNumber;

        int index =

            findParkedVehicle(vehicleNumber);

        if (index == -1)

        {

            cout << "\nVehicle is not currently parked.\n";

            return;
        }

        ParkedVehicle &vehicle =

            parkedVehicles[index];

        // ----------------------------------------------------

        // AUTOMATIC EXIT TIME

        // ----------------------------------------------------

        time_t exitTime =

            time(nullptr);

        // ----------------------------------------------------

        // ACTUAL DURATION

        // ----------------------------------------------------

        double seconds =

            difftime(

                exitTime,

                vehicle.entryTime);

        double actualHours =

            seconds / 3600.0;

        // ----------------------------------------------------

        // MINIMUM 1 HOUR

        // ----------------------------------------------------

        if (actualHours < 1.0)

        {

            actualHours = 1.0;
        }

        // ----------------------------------------------------

        // BILLABLE HOURS

        // ----------------------------------------------------

        double billableHours =

            ceil(actualHours);

        // ----------------------------------------------------

        // PARKING CHARGE

        // ----------------------------------------------------

        double parkingCharge = 0;

        if (!vehicle.monthlyPassUsed)

        {

            double rate =

                getBaseRate(

                    vehicle.vehicleType);

            double multiplier =

                getDynamicMultiplier();

            parkingCharge =

                billableHours * rate * multiplier;

            double minimumCharge =

                getMinimumCharge(

                    vehicle.vehicleType);

            if (

                parkingCharge < minimumCharge)

            {

                parkingCharge =

                    minimumCharge;
            }
        }

        else

        {

            parkingCharge = 0;
        }

        // ----------------------------------------------------

        // EV CHARGING: the choice was made at vehicle entry.
        double evChargingUnits = 0;
        double evChargingCost = 0;

        if (vehicle.vehicleType == EV && vehicle.needsEVCharging)
        {
            cout << "Enter charging units (kWh): ";
            cin >> evChargingUnits;
            if (evChargingUnits < 0)
                evChargingUnits = 0;
            evChargingCost = evChargingUnits * evChargingRate;
        }

        // ----------------------------------------------------

        // TOTAL FINAL BILL

        // ----------------------------------------------------

        double finalAmount =

            parkingCharge + evChargingCost;

        double refund = 0;

        double additionalPayment = 0;

        // ----------------------------------------------------

        // ADDITIONAL PAYMENT

        // ----------------------------------------------------

        if (

            vehicle.amountPaid < finalAmount)

        {

            additionalPayment =

                finalAmount - vehicle.amountPaid;
        }

        // ----------------------------------------------------

        // REFUND

        // ----------------------------------------------------

        if (

            vehicle.amountPaid > finalAmount)

        {

            refund =

                vehicle.amountPaid - finalAmount;
        }

        // ----------------------------------------------------

        // DISPLAY FINAL BILL

        // ----------------------------------------------------

        cout << fixed

             << setprecision(2);

        cout << "\n============================================\n";

        cout << "              FINAL PARKING BILL\n";

        cout << "============================================\n";

        cout << "Vehicle Number       : "

             << vehicle.vehicleNumber

             << endl;

        cout << "Vehicle Type         : "

             << vehicleTypeToString(

                    vehicle.vehicleType)

             << endl;

        cout << "Slot Number          : "

             << vehicle.slotNumber

             << endl;

        cout << "\nEntry Time           : "

             << formatDateTime(

                    vehicle.entryTime)

             << endl;

        cout << "Exit Time            : "

             << formatDateTime(exitTime)

             << endl;

        cout << "\nPlanned Duration     : "

             << vehicle.plannedHours

             << " hour(s)"

             << endl;

        cout << "Actual Duration      : "

             << actualHours

             << " hour(s)"

             << endl;

        cout << "Billable Duration    : "

             << billableHours

             << " hour(s)"

             << endl;

        if (vehicle.monthlyPassUsed)

        {

            cout << "\nMonthly Pass         : ACTIVE"

                 << endl;

            cout << "Parking Charge       : Rs. 0.00"

                 << endl;
        }

        else

        {

            cout << "\nParking Charge       : Rs. "

                 << parkingCharge

                 << endl;
        }

        // ----------------------------------------------------

        // EV CHARGING

        // ----------------------------------------------------

        if (

            vehicle.vehicleType == EV)

        {

            cout << "\nEV Charging Units    : "

                 << evChargingUnits

                 << " kWh"

                 << endl;

            cout << "EV Charging Rate     : Rs. "

                 << evChargingRate

                 << " / kWh"

                 << endl;

            cout << "EV Charging Cost     : Rs. "

                 << evChargingCost

                 << endl;
        }

        // ----------------------------------------------------

        // FINAL TOTAL

        // ----------------------------------------------------

        cout << "\n--------------------------------------------\n";

        cout << "TOTAL FINAL BILL     : Rs. "

             << finalAmount

             << endl;

        cout << "Amount Paid at Entry : Rs. "

             << vehicle.amountPaid

             << endl;

        // ----------------------------------------------------

        // ADDITIONAL PAYMENT

        // ----------------------------------------------------

        if (additionalPayment > 0)

        {

            cout << "\nADDITIONAL PAYMENT REQUIRED : Rs. "

                 << additionalPayment

                 << endl;

            double extraPayment;

            cout << "Enter additional payment: Rs. ";

            cin >> extraPayment;

            if (

                extraPayment < additionalPayment)

            {

                cout << "\nInsufficient additional payment.\n";

                cout << "Vehicle cannot exit yet.\n";

                return;
            }

            double extraRefund =

                extraPayment - additionalPayment;

            if (extraRefund > 0)

            {

                cout << "Additional Refund    : Rs. "

                     << extraRefund

                     << endl;
            }
        }

        // ----------------------------------------------------

        // REFUND

        // ----------------------------------------------------

        if (refund > 0)

        {

            cout << "\nREFUND               : Rs. "

                 << refund

                 << endl;
        }

        cout << "--------------------------------------------\n";

        // ----------------------------------------------------

        // UPDATE EXISTING HISTORY RECORD

        // ----------------------------------------------------

        for (auto &record : history)

        {

            if (record.vehicleNumber == vehicle.vehicleNumber &&

                record.active)

            {

                record.exitTime = exitTime;

                record.actualHours = actualHours;

                record.billableHours = billableHours;

                record.parkingCharge = parkingCharge;

                record.evChargingUnits = evChargingUnits;

                record.evChargingCost = evChargingCost;

                record.finalAmount = finalAmount;

                record.refund = refund;

                record.additionalPayment = additionalPayment;

                record.active = false;

                break;
            }
        }

        // ----------------------------------------------------

        // FREE PARKING SLOT

        // ----------------------------------------------------

        for (auto &slot : slots)

        {

            if (

                slot.slotNumber == vehicle.slotNumber)

            {

                slot.occupied = false;

                slot.currentVehicle = "";

                break;
            }
        }

        // ----------------------------------------------------

        // MARK VEHICLE INACTIVE

        // ----------------------------------------------------

        vehicle.active = false;

        cout << "\n============================================\n";

        cout << "          VEHICLE EXIT COMPLETED\n";

        cout << "============================================\n";

        cout << "Slot "

             << vehicle.slotNumber

             << " is now available.\n";

        cout << "Thank you for using Smart Parking.\n";

        cout << "============================================\n";
    }

    // ========================================================

    // RESERVE PARKING SLOT

    // ========================================================

    void reserveSlot()

    {

        expireReservations();

        cout << "\n============================================\n";

        cout << "              RESERVE SLOT\n";

        cout << "============================================\n";

        string vehicleNumber;

        int typeChoice;

        int slotNumber;

        double plannedHours;

        int day, month, year, startHour, startMinute, endHour, endMinute;

        int type;

        cout << "Enter vehicle number: ";

        cin >> vehicleNumber;

        if (findParkedVehicle(vehicleNumber) != -1)

        {

            cout << "Vehicle is already parked.\n";

            return;
        }

        if (findReservation(vehicleNumber) != -1)

        {

            cout << "This vehicle already has an active reservation.\n";

            return;
        }

        cout << "\nSelect vehicle type:\n";

        cout << "1. Bike\n";

        cout << "2. Car\n";

        cout << "3. SUV\n";

        cout << "4. EV\n";

        cout << "Enter vehicle type choice: ";

        cin >> typeChoice;

        if (!getintFromChoice(typeChoice, type))

        {

            cout << "Invalid vehicle type choice.\n";

            return;
        }

        slotNumber = findAvailableSlot(type, vehicleNumber);

        if (slotNumber == -1)

        {

            cout << "Sorry, no available slot for this vehicle type.\n";

            return;
        }

        cout << "Enter planned parking duration in hours: ";

        cin >> plannedHours;

        if (plannedHours <= 0)

        {

            cout << "Duration must be greater than zero.\n";

            return;
        }

        if (plannedHours < 1)

            plannedHours = 1;

        cout << "Enter reservation date (DD MM YYYY): ";

        cin >> day >> month >> year;

        cout << "Enter arrival window START time (HH MM, 24-hour): ";

        cin >> startHour >> startMinute;

        cout << "Enter arrival window END time (HH MM, 24-hour): ";

        cin >> endHour >> endMinute;

        time_t arrivalStart, arrivalEnd;

        if (startHour < 0 || startHour > 23 ||

            endHour < 0 || endHour > 23 ||

            startMinute < 0 || startMinute > 59 ||

            endMinute < 0 || endMinute > 59 ||

            !makeDateTime(day, month, year, startHour, startMinute, arrivalStart) ||

            !makeDateTime(day, month, year, endHour, endMinute, arrivalEnd) ||

            arrivalStart <= time(nullptr) ||

            arrivalEnd <= arrivalStart)

        {

            cout << "Invalid date/time window. Enter a future date and an end time after start time.\n";

            return;
        }

        double estimatedAmount =

            plannedHours * getBaseRate(type) * getDynamicMultiplier();

        if (estimatedAmount < getMinimumCharge(type))

            estimatedAmount = getMinimumCharge(type);

        double advanceRequired = estimatedAmount * 0.50;

        cout << fixed << setprecision(2);

        cout << "\nVehicle type               : " << vehicleTypeToString(type) << endl;

        cout << "Automatically assigned slot: " << slotNumber << endl;

        cout << "Estimated parking charge : Rs. " << estimatedAmount << endl;

        cout << "50% advance to pay       : Rs. " << advanceRequired << endl;

        cout << "Arrival allowed from     : " << formatDateTime(arrivalStart) << endl;

        cout << "Arrival allowed until    : " << formatDateTime(arrivalEnd) << endl;

        double payment;

        cout << "Pay advance amount (Rs.): ";

        cin >> payment;

        if (payment < advanceRequired)

        {

            cout << "Insufficient advance. Reservation was NOT created.\n";

            return;
        }

        Reservation booking;

        booking.vehicleNumber = vehicleNumber;

        booking.slotNumber = slotNumber;

        booking.plannedHours = plannedHours;

        booking.advancePaid = advanceRequired;

        booking.arrivalStart = arrivalStart;

        booking.arrivalEnd = arrivalEnd;

        booking.active = true;

        reservations.push_back(booking);

        ParkingSlot &slot = slots[slotNumber - 1];

        slot.reserved = true;

        slot.reservedVehicle = vehicleNumber;

        cout << "\nReservation confirmed! Slot " << slotNumber << " is booked.\n";

        cout << "Advance received: Rs. " << advanceRequired << endl;

        if (payment > advanceRequired)

            cout << "Extra amount to return: Rs. "

                 << (payment - advanceRequired) << endl;
    }

    // ========================================================

    // DISPLAY SLOT AVAILABILITY

    // ========================================================

    void displaySlots()

    {

        expireReservations();

        cout << "\n";

        cout << "============================================\n";

        cout << "             SLOT AVAILABILITY\n";

        cout << "============================================\n";

        cout << left

             << setw(10)

             << "Slot"

             << setw(12)

             << "Type"

             << setw(15)

             << "Status"

             << setw(20)

             << "Vehicle"

             << endl;

        cout << "--------------------------------------------\n";

        for (const auto &slot : slots)

        {

            string status;

            if (slot.occupied)

            {

                status = "Occupied";
            }

            else if (slot.reserved)

            {

                status = "Reserved";
            }

            else

            {

                status = "Available";
            }

            string vehicle = "-";

            if (slot.occupied)

            {

                vehicle =

                    slot.currentVehicle;
            }

            else if (slot.reserved)

            {

                vehicle =

                    slot.reservedVehicle;
            }

            cout << left

                 << setw(10)

                 << slot.slotNumber

                 << setw(12)

                 << slotTypeToString(

                        slot.slotType)

                 << setw(15)

                 << status

                 << setw(20)

                 << vehicle

                 << endl;
        }
    }

    // ========================================================

    // BUY MONTHLY PARKING PASS

    // ========================================================

    void buyMonthlyPass()

    {

        cout << "\n";

        cout << "============================================\n";

        cout << "           MONTHLY PARKING PASS\n";

        cout << "============================================\n";

        string vehicleNumber;

        cout << "Enter vehicle number: ";

        cin >> vehicleNumber;

        if (hasMonthlyPass(vehicleNumber))

        {

            cout << "\nVehicle already has an active pass.\n";

            return;
        }

        // ----------------------------------------------------

        // IMPORTANT:

        // THERE IS NO SLOT AVAILABILITY CHECK HERE.

        //

        // A MONTHLY PASS CAN BE PURCHASED EVEN IF ALL

        // SLOTS FOR THAT VEHICLE TYPE ARE FULL.

        // ----------------------------------------------------

        string ownerName;

        cout << "Enter owner name: ";

        cin.ignore();

        getline(

            cin,

            ownerName);

        int typeChoice;

        cout << "\nVehicle Type:\n";

        cout << "1. Bike - Rs. 1000/month\n";

        cout << "2. Car  - Rs. 2000/month\n";

        cout << "3. SUV  - Rs. 3000/month\n";

        cout << "4. EV   - Rs. 2500/month\n";

        cout << "\nEnter choice: ";

        cin >> typeChoice;

        int vehicleType;

        if (

            !getintFromChoice(

                typeChoice,

                vehicleType))

        {

            cout << "\nInvalid vehicle type.\n";

            return;
        }

        // Runtime polymorphism selects the existing monthly-pass price.
        unique_ptr<VehiclePricing> vehiclePricing = createVehiclePricing(vehicleType);
        double price = vehiclePricing->monthlyPassRate();

        cout << fixed

             << setprecision(2);

        cout << "\nMonthly Pass Price: Rs. "

             << price

             << endl;

        double payment;

        cout << "Enter payment amount: Rs. ";

        cin >> payment;

        if (payment < price)

        {

            cout << "\nInsufficient payment.\n";

            return;
        }

        MonthlyPass pass;

        pass.vehicleNumber =

            vehicleNumber;

        pass.ownerName =

            ownerName;

        pass.vehicleType =

            vehicleType;

        pass.setPassCode("PASS" + vehicleNumber);

        time_t currentTime =

            time(nullptr);

        pass.startDate =

            formatDate(currentTime);

        pass.expiryDate =

            formatExpiryDate(

                currentTime,

                30);

        pass.active = true;

        monthlyPasses.push_back(pass);
        saveMonthlyPassesToFile();

        cout << "\n============================================\n";

        cout << "       MONTHLY PASS PURCHASED\n";

        cout << "============================================\n";

        cout << "Vehicle       : "

             << vehicleNumber

             << endl;

        cout << "Owner         : "

             << ownerName

             << endl;

        cout << "Vehicle Type  : "

             << vehicleTypeToString(

                    vehicleType)

             << endl;

        cout << "Pass Code     : "

             << pass.getPassCode()

             << endl;

        cout << "Amount Paid   : Rs. "

             << price

             << endl;

        cout << "Refund        : Rs. "

             << payment - price

             << endl;

        cout << "Start Date    : "

             << pass.startDate

             << endl;

        cout << "Expiry Date   : "

             << pass.expiryDate

             << endl;

        cout << "Status        : ACTIVE"

             << endl;

        cout << "============================================\n";
    }

    // ========================================================

    // PARKING HISTORY

    // ========================================================

    void showParkingHistory()

    {

        cout << "\n";

        cout << "============================================\n";

        cout << "              PARKING HISTORY\n";

        cout << "============================================\n";

        if (history.empty())

        {

            cout << "No parking history available.\n";

            return;
        }

        cout << fixed << setprecision(2);

        for (int i = 0; i < static_cast<int>(history.size()); i++)

        {

            const auto &record = history[i];

            cout << "\n";

            cout << "--------------------------------------------\n";

            cout << "Record " << i + 1 << endl;

            cout << "--------------------------------------------\n";

            cout << "Vehicle Number       : " << record.vehicleNumber << endl;

            if (record.active)

                cout << "Status               : CURRENTLY PARKED" << endl;

            else

                cout << "Status               : EXITED" << endl;

            cout << "Vehicle Type         : "

                 << vehicleTypeToString(record.vehicleType) << endl;

            cout << "Slot Number          : " << record.slotNumber << endl;

            cout << "Entry Time           : " << formatDateTime(record.entryTime) << endl;

            if (record.active)

                cout << "Exit Time            : NOT EXITED YET" << endl;

            else

                cout << "Exit Time            : " << formatDateTime(record.exitTime) << endl;

            cout << "Planned Duration     : " << record.plannedHours << " hour(s)" << endl;

            cout << "Actual Duration      : " << record.actualHours << " hour(s)" << endl;

            cout << "Billable Duration    : " << record.billableHours << " hour(s)" << endl;

            cout << "Monthly Pass Used    : "

                 << (record.monthlyPassUsed ? "YES" : "NO") << endl;

            cout << "Amount Paid at Entry : Rs. " << record.amountPaidAtEntry << endl;

            cout << "Parking Charge       : Rs. " << record.parkingCharge << endl;

            cout << "EV Charging Units    : " << record.evChargingUnits << " kWh" << endl;

            cout << "EV Charging Cost     : Rs. " << record.evChargingCost << endl;

            cout << "Final Amount         : Rs. " << record.finalAmount << endl;

            cout << "Refund               : Rs. " << record.refund << endl;

            cout << "Additional Payment   : Rs. " << record.additionalPayment << endl;
        }
    }

    // ========================================================

    // PRICING INFORMATION

    // ========================================================

    void showPricing()

    {

        cout << "\n";

        cout << "============================================\n";

        cout << "              PRICING INFORMATION\n";

        cout << "============================================\n";

        cout << fixed

             << setprecision(2);

        cout << "\nHourly Charges:\n";

        cout << "Bike : Rs. "

             << bikeRate

             << " / hour\n";

        cout << "Car  : Rs. "

             << carRate

             << " / hour\n";

        cout << "SUV  : Rs. "

             << suvRate

             << " / hour\n";

        cout << "EV   : Rs. "

             << evRate

             << " / hour\n";

        cout << "\nMinimum Parking Charge:\n";

        cout << "Bike : 1 hour rate * dynamic multiplier\n";

        cout << "Car  : 1 hour rate * dynamic multiplier\n";

        cout << "SUV  : 1 hour rate * dynamic multiplier\n";

        cout << "EV   : 1 hour rate * dynamic multiplier\n";

        cout << "\nCurrent Dynamic Multiplier: "

             << getDynamicMultiplier()

             << "x\n";

        cout << "\nCurrent Minimum Charges:\n";

        cout << "Bike : Rs. "

             << getMinimumCharge(BIKE)

             << endl;

        cout << "Car  : Rs. "

             << getMinimumCharge(CAR)

             << endl;

        cout << "SUV  : Rs. "

             << getMinimumCharge(SUV)

             << endl;

        cout << "EV   : Rs. "

             << getMinimumCharge(EV)

             << endl;

        cout << "\nDynamic Pricing:\n";

        cout << "Occupancy < 50% : 1.00x\n";

        cout << "Occupancy 50-80%: 1.25x\n";

        cout << "Occupancy > 80% : 1.50x\n";

        cout << "\nEV Charging:\n";

        cout << "Rs. "

             << evChargingRate

             << " / kWh\n";

        cout << "\nMonthly Passes:\n";

        cout << "Bike : Rs. "

             << bikeMonthlyPass

             << " / month\n";

        cout << "Car  : Rs. "

             << carMonthlyPass

             << " / month\n";

        cout << "SUV  : Rs. "

             << suvMonthlyPass

             << " / month\n";

        cout << "EV   : Rs. "

             << evMonthlyPass

             << " / month\n";
    }

    // ========================================================

    // ACTIVE MONTHLY PASSES

    // ========================================================

    void showMonthlyPasses()

    {

        cout << "\n";

        cout << "============================================\n";

        cout << "            ACTIVE MONTHLY PASSES\n";

        cout << "============================================\n";

        bool found = false;

        cout << fixed

             << setprecision(2);

        for (const auto &pass : monthlyPasses)

        {

            if (!pass.active)

            {

                continue;
            }

            found = true;

            cout << "\n";

            cout << "--------------------------------------------\n";

            cout << "Vehicle Number : "

                 << pass.vehicleNumber

                 << endl;

            cout << "Owner Name     : "

                 << pass.ownerName

                 << endl;

            cout << "Vehicle Type   : "

                 << vehicleTypeToString(

                        pass.vehicleType)

                 << endl;

            cout << "Pass Code      : "

                 << pass.getPassCode()

                 << endl;

            cout << "Start Date     : "

                 << pass.startDate

                 << endl;

            cout << "Expiry Date    : "

                 << pass.expiryDate

                 << endl;

            cout << "Status         : ACTIVE\n";
        }

        if (!found)

        {

            cout << "\nNo active monthly passes found.\n";
        }
    }

    // ========================================================

    // MAIN MENU

    // ========================================================

    void menu()

    {

        int choice;

        do

        {

            cout << "\n\n";

            cout << "============================================\n";

            cout << "       SMART PARKING LOT MANAGEMENT\n";

            cout << "============================================\n";

            cout << "1. Vehicle Entry\n";

            cout << "2. Vehicle Exit\n";

            cout << "3. Check Slot Availability\n";

            cout << "4. Reserve Parking Slot\n";

            cout << "5. Buy Monthly Parking Pass\n";

            cout << "6. Parking History\n";

            cout << "7. Pricing Information\n";

            cout << "8. Active Monthly Passes\n";

            cout << "9. Exit Program\n";

            cout << "============================================\n";

            cout << "Enter your choice: ";

            cin >> choice;

            try

            {

                switch (choice)

                {

                case 1:

                    vehicleEntry();

                    break;

                case 2:

                    vehicleExit();

                    break;

                case 3:

                    displaySlots();

                    break;

                case 4:

                    reserveSlot();

                    break;

                case 5:

                    buyMonthlyPass();

                    break;

                case 6:

                    showParkingHistory();

                    break;

                case 7:

                    showPricing();

                    break;

                case 8:

                    showMonthlyPasses();

                    break;

                case 9:

                    cout << "\n";

                    cout << "Thank you for using "

                            "Smart Parking System.\n";

                    break;

                default:

                    cout << "\nInvalid choice.\n";
                }

                saveAllData();
            }
            catch (const exception &e)
            {
                cerr << "\nOperation failed: " << e.what() << endl;
            }

        } while (choice != 9);
    }
};


// ============================================================

// MAIN FUNCTION

// ============================================================

int main()

{

    ParkingSystem parkingSystem;

    parkingSystem.menu();

    return 0;
}