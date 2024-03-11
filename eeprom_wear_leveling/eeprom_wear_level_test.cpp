
#include <cstdio>
#include <iostream>


using std::cout;
using std::endl;


namespace wear_leveling {


static const size_t hardware_eeprom_size = 512;


struct Record {
    uint32_t last_prime;
};


/**
Sorta-kinda mock-out access to EEPROM.

Real EEPROM looks like this:

    EEPROM Registers

    uint8_t EEDR (EEPROM Data Register)
    uint8_t EECR (EEPROM Control Register)
        * Bits 7 to 4: Reserved Bits
        * Bit 3: EERIE (EEPROM Ready Interrupt Enable)
        * Bit 2: EEMWE: EEPROM Master Write Enable
        * Bit 1: EEWE: EEPROM Write Enable
        * Bit 0: EERE: EEPROM Read Enable
    uint16_t EEAR (EEPROM Address Register)
        * Bits 15 to 10: Reserved Bits
        * Bits 9 to 0: EEPROM Address

*/
class FakeEEPROM {
    private:
        FILE* fp;

    public:
        FakeEEPROM();
        ~FakeEEPROM();

    void write(uint16_t address, uint8_t value);
    uint8_t read(uint16_t address);
};


FakeEEPROM::FakeEEPROM() {
    fp = fopen("eeprom.bin", "w+");
    uint8_t buffer[hardware_eeprom_size];
    for(size_t i=0; i < hardware_eeprom_size; ++i) {
        // Erase like real EEPROM
        buffer[i] = 0xff;
    }
    fwrite(buffer, 1, hardware_eeprom_size, fp);
}


FakeEEPROM::~FakeEEPROM() {
    if ( fp != NULL) {
        fclose(fp);
    }
}

/**
EEPROM Write

1. Wait till previous write operation is completed(i.e. wait till EEWE becomes zero).
2. Load the EEPROM address into EEAR at which the data has to be stored.
3. Load the data into EEDR which has to be stored in EEPROM.
4. Set the EEMWE (EEPROM Master Write Enable).
5. Within four clock cycles after 4th step, set EEWE(Eeprom Write Enable)
  to trigger the EEPROM Write operation.

For example::

    while(EECR & (1<<EEWE));
    EEAR = address;
    EEDR = value;
    EECR |= (1<<EEMWE);
    EECR |= (1<<EEWE);
    EEAR = 0;

*/
void FakeEEPROM::write(uint16_t address, uint8_t value) {
    fseek(fp, address, SEEK_SET);
    fputc(value, fp);
}


/**
Read Operation

1. WAit for completion of previous Write operation.
2. EEWE will be cleared once EEPROM write is completed.
3. Load the EEPROM address into EEAR from where the data needs to be read.
4. Trigger the EEPROM read operation by setting EERE (EEPROM Read Enable).
5. Wait for some time (about 1ms) and collect the read data from EEDR.

For example:

    while(EECR & (1<<EEWE));
    EEAR = address;
    EECR |= (1<<EERE);
    EEAR = 0;
    return EEDR;

*/
uint8_t FakeEEPROM::read(uint16_t address) {
    fseek(fp, address, SEEK_SET);
    return fgetc(fp);
}


/**
Use entire EEPROM to store a single Record.

Many copies are written across the whole address

Taking care to extend lifetime using wear leveling.
*/
class RecordStore {
    private:

        const Record& record;
        FakeEEPROM& eeprom;
        uint8_t current_index;
        uint8_t num_records;
        uint8_t record_size;


        void update_status();
        void update_record();

    public:
        RecordStore(const Record& record, FakeEEPROM& eeprom);
        uint8_t get_record_size() { return record_size; }
        uint8_t get_num_records() { return num_records; }
        void save();
};


RecordStore::RecordStore(const Record& record, FakeEEPROM& eeprom) :
        record(record), eeprom(eeprom) {
    current_index = 0;
    record_size = sizeof(record);
    num_records = (hardware_eeprom_size - 1) / (record_size + 1);
};


void RecordStore::save() {
    update_status();
    update_record();
}


/**
Update the status buffer for the current position.
*/
void RecordStore::update_status() {
    uint16_t start = 1;
    uint16_t end = (num_records + 1);

    // Save test bit pattern
    uint8_t test_value = 1;
    for(uint16_t address = start; address < end; ++address) {
        eeprom.write(address, test_value);
        ++test_value;
    }
}


/**
Save the current value of our record into the current position.
*/
void RecordStore::update_record() {
    uint16_t start = (num_records + 1);
    uint16_t end = start + (record_size * num_records);

    // Save test bit pattern
    uint8_t i;
    uint8_t test_value = 1;

    uint16_t address = start;
    while (address < end) {
        // Write each byte of record
        for(i = 0; i < record_size; ++i) {
            eeprom.write(address, test_value);
            ++address;
        }
        ++test_value;
    }
}


} // namespace wear_leveling


int main(int argc, char** argv) {
    wear_leveling::Record record = {
        .last_prime=999983
    };
    auto eeprom = wear_leveling::FakeEEPROM();
    auto store = wear_leveling::RecordStore(record, eeprom);

    // Cast to to 'int' to avoid problem printing 'uint8_t' as 'char'
    int structure_size = static_cast<int>(store.get_record_size());
    int leveling_ratio = static_cast<int>(store.get_num_records());

    cout << endl;
    cout << "We have 512 bytes of WearLeveling storage, ";
    cout << "but lore tells us to avoid address 0x00." << endl;
    cout << "This leaves us 511 bytes to hold our configuration." << endl;
    cout << endl;
    cout << "The size of our config struct is " << structure_size;
    cout << " bytes. We will need another byte per record " << endl;
    cout << "for bookkeeping: " << (structure_size + 1) << " bytes." << endl;
    cout << endl;
    cout << "The greatest integer function of 511 / " << (structure_size + 1);
    cout << " gives a wear leveling ratio of " << leveling_ratio << endl;

    store.save();
    record.last_prime = 97;
    store.save();

    return 0;
}
