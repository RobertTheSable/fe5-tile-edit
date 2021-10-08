#pragma once
#include <map>
#include <string>
#include <cstdint>
#include <iostream>
#include <fstream>
#define HIROM 1
#ifndef INI_DIR
    #ifdef win32
        #define INI_DIR "ini\\"
    #else
        #define INI_DIR "ini/"
    #endif
#endif

int PCToLoROM(int addr, bool header = false);
int LoROMToPC(int addr, bool header = false);

struct RomHeader{
    char name[21];
    unsigned char mapMode;
    unsigned char romType;
    unsigned char romSize;
    unsigned char sramSize;
    unsigned char licenseID;
    unsigned char fixed;
    unsigned char version;
    unsigned short checksumComplement;
    unsigned short checksum;
};

class RomMap{
public:
    typedef int romState;
    static const romState rom_ok = 0;
    static const romState no_file = 1;
    static const romState bad_header = 2;
    static const romState not_supported = 4;
    static const romState malformed_name = 8;
    static const romState misc_fail = 16;
    
    static const unsigned int REVISION =  0x80FFDB; 
    RomMap(const std::string& fileName, const std::string& iniPath = "");
    RomMap();
    int getAddress(std::string label) const;
    int getPCAddress(std::string label, bool headered = false) const;
    int getPCAddress(int address, bool headered = false) const;
    void read(uint8_t* buf, int offset, int size) const;
    int readAddress(int offset) const;
    bool isHeadered() const;
    int getSize() const;
    
    uint8_t& operator[](int address);
    uint8_t& operator[](std::string label);
    //explicit operator bool() const;
    romState getMyState() const;

private:
    std::map<std::string, unsigned int> m_addressMap;
    uint8_t *romData;
    int romSize;
    bool rom_headered;
    RomHeader internalHeader;
    std::string rom_name;
    romState myState;
};
