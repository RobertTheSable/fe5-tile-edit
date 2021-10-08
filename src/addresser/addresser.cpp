#include "addresser.h"
#include <cctype>
#include <sstream>  

int PCToLoROM(int addr, bool header)
{
    if (header)
    {
        addr-=512;
    }
    if (addr<0 || addr>=0x400000)
    {
        return -1;
    }
    addr=((addr<<1)&0x7F0000)|(addr&0x7FFF)|0x8000;
    addr|=0x800000;
    return addr;
}
int LoROMToPC(int addr, bool header)
{
    if (addr<0 || addr>0xFFFFFF ||//not 24bit
                (addr&0xFE0000)==0x7E0000 ||//wram
                (addr&0x408000)==0x000000)//hardware regs
            return -1;
    addr=((addr&0x7F0000)>>1|(addr&0x7FFF));
    if (header) addr+=512;
    return addr;
}
int HiROMToPC(int addr, bool header)
{
    if (addr<0 || addr>0xFFFFFF ||//not 24bit
                (addr&0xFE0000)==0x7E0000) //wram
            return -1;
    addr=(addr& 0x3fffff);
    if (header) addr+=512;
    return addr;
}


RomMap::RomMap()
{
    
}

RomMap::RomMap(const std::string& fileName, const std::string& basePath)
{
    myState = rom_ok;
    std::ifstream input_rom(fileName, std::ios::binary | std::ios::ate);
    romSize = input_rom.tellg() - (input_rom.tellg() % 1024);
    if(input_rom)
    {
        
        if((input_rom.tellg() % 1024) == 512)
        {
            rom_headered = true;
        }
        else if((input_rom.tellg() % 1024) == 0)
        {
            rom_headered = false;
        }
        else
        {
            myState |= bad_header;
        }
        //std::cout << (headered ? "True" : "False") << std::endl;
        if(myState == rom_ok)
        {
            //int rev;
            input_rom.seekg(LoROMToPC(0x80FFC0, rom_headered));
            input_rom.read((char*)&internalHeader, 0x20);
            
            for(int i = 0; i < 20; i++)
            {
                //std::cout << internalHeader.name[i];
                if(!isprint (internalHeader.name[i]))
                {
                    myState |= malformed_name;
                }
            }
            if((myState & malformed_name) != 0)
            {
                myState ^= malformed_name;
                //std::cout << HiROMToPC(0x80FFC0, headered) << std::endl;
                input_rom.seekg(HiROMToPC(0x80FFC0, rom_headered));
                input_rom.read((char*)&internalHeader, 0x20);
            
                for(int i = 0; i < 20; i++)
                {
                    //std::cout << internalHeader.name[i];
                    if(!isprint (internalHeader.name[i]))
                    {
                        myState |= malformed_name;
                    }
                }
                
            }
            if(myState == rom_ok)
            {
                std::string ini_name;
                if (basePath != "") {
                    ini_name = basePath + std::string(internalHeader.name, 21);
                } else {
                    ini_name = std::string(INI_DIR) + std::string(internalHeader.name, 21);
                }
                std::stringstream ini_stream(ini_name.erase(ini_name.find_last_not_of(" ")+1), std::ios_base::ate|std::ios_base::out);
                ini_stream << " 1." << (int)internalHeader.version  << ".ini";
                ini_name = ini_stream.str();
                
                std::ifstream input_ini(ini_name);
                if(input_ini)
                {
                    std::string label;
                    unsigned int address = 0;
                    while(input_ini >> label)
                    {
                        input_ini >> std::hex >> address;
                        m_addressMap[label] = address;
                    }
                }
                else
                {
                    myState |= not_supported;
                }
                int startoffset  = ((rom_headered) ? 512 : 0);
                input_rom.seekg(startoffset, input_rom.beg);
                romData = new uint8_t[romSize];
                input_rom.read((char*)romData, romSize );
            }
        }
    }
    else
    {
        myState |= no_file;
    }
    input_rom.close();
    //std::cout << myState << std::endl;
    
}
int RomMap::getAddress(std::string label) const {
    if(m_addressMap.count(label) != 0)
    {
        return m_addressMap.at(label);
    }
    else
    {
        return -1;
    }
}
int RomMap::getPCAddress(std::string label, bool headered) const {
    int retvalue;
    if(m_addressMap.count(label) != 0)
    {
        retvalue = getPCAddress(m_addressMap.at(label), headered);
    }
    else
    {
        retvalue= -1;
    }
    return retvalue;
}
int RomMap::getPCAddress(int address, bool headered) const {
    int retvalue;
    if( (internalHeader.romType & HIROM) == 0)
    {
        retvalue = LoROMToPC(address, headered);
    }
    else
    {
        retvalue = HiROMToPC(address, headered);
    }
    return retvalue;
}
void RomMap::read(uint8_t* buf, int offset, int size) const{
    for(int i = 0; i < size; i++)
    {
        buf[i] = romData[getPCAddress(offset + i)];
    }
}

int RomMap::readAddress(int offset) const
{
    int *ret_offset = (int*)&romData[getPCAddress(offset)];
    return (*ret_offset)&0xFFFFFF;
}

uint8_t& RomMap::operator[](int address)
{
    return romData[getPCAddress(address)];
}
uint8_t& RomMap::operator[](std::string label)
{
    return romData[getPCAddress(label)];
}

RomMap::romState RomMap::getMyState() const
{
    return myState;
}
bool RomMap::isHeadered() const
{
    return rom_headered;
}
int RomMap::getSize() const
{
    return romSize;
}
    
