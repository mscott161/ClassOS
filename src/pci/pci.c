#include "pci.h"
#include "memory/heap/kheap.h"
#include "kernel.h"
#include "io/io.h"

uint32_t Read(uint16_t bus, uint16_t device, uint16_t function, uint16_t registeroffset)
{
    uint32_t id =
        0x1 << 31
        | ((bus & 0xFF) << 16)
        | ((device & 0x1F) << 11)
        | ((function & 0x07) << 8)
        | (registeroffset & 0xFC);
    outl(PCI_COMMAND_PORT, id);
    uint32_t result = insl(PCI_DATA_PORT);
    return result >> (8* (registeroffset % 4));
}

bool DeviceHasFunctions(uint16_t bus, uint16_t device)
{
    return Read(bus, device, 0, 0x0E) & (1<<7);
}

struct pci_descriptor* GetDeviceDescriptor(uint16_t bus, uint16_t device, uint16_t function)
{
    struct pci_descriptor* result = kzalloc(sizeof(struct pci_descriptor));

    result->bus = bus;
    result->device = device;
    result->function = function;
    
    result->vendor_id = Read(bus, device, function, 0x00);
    result->device_id = Read(bus, device, function, 0x02);

    result->class_id = Read(bus, device, function, 0x0b);
    result->subclass_id = Read(bus, device, function, 0x0a);
    result->interface_id = Read(bus, device, function, 0x09);

    result->revision = Read(bus, device, function, 0x08);
    result->interrupt = Read(bus, device, function, 0x3c);
    
    return result;
}

struct bar* GetBaseAddressRegister(uint16_t bus, uint16_t device, uint16_t function, uint16_t bar)
{
    struct bar* result = kzalloc(sizeof(struct bar));
    
    uint32_t headertype = Read(bus, device, function, 0x0E) & 0x7F;
    int maxBARs = 6 - (4*headertype);
    if(bar >= maxBARs)
        return result;
    
    uint32_t bar_value = Read(bus, device, function, 0x10 + 4*bar);
    result->type = (bar_value & 0x1) ? InputOutput : MemoryMapping;
    
    if(result->type == MemoryMapping)
    {
        switch((bar_value >> 1) & 0x3)
        {
            case 0: // 32 Bit Mode
            case 1: // 20 Bit Mode
            case 2: // 64 Bit Mode
                result->address = (uint8_t*)(bar_value & ~0x3);
                break;
        }
    }
    else // InputOutput
    {
        result->address = (uint8_t*)(bar_value & ~0x3);
        result->prefetchable = false;
    }

    return result;
}

void pci_init()
{
    for(int bus = 0; bus < 8; bus++)
    {
        for(int device = 0; device < 32; device++)
        {
            int numFunctions = DeviceHasFunctions(bus, device) ? 8 : 1;
            for(int function = 0; function < numFunctions; function++)
            {
                struct pci_descriptor* dev = GetDeviceDescriptor(bus, device, function);
                
                if(dev->vendor_id == 0x0000 || dev->vendor_id == 0xFFFF)
                    continue;

                printf2("PCI BUS ");
                printf2("%x", bus & 0xFF);
                
                printf2(", DEVICE ");
                printf2("%x", device & 0xFF);

                printf2(", FUNCTION ");
                printf2("%x", function & 0xFF);
                
                printf2(" = VENDOR ");
                printf2("%x", (dev->vendor_id & 0xFF00) >> 8);
                printf2("%x", dev->vendor_id & 0xFF);
                printf2(", DEVICE ");
                printf2("%x", (dev->device_id & 0xFF00) >> 8);
                printf2("%x", dev->device_id & 0xFF);
                printf2("\n");
           }
        }
    }
}

void* pci_get_lfb()
{
    for(int bus = 0; bus < 8; bus++)
    {
        for(int device = 0; device < 32; device++)
        {
            int numFunctions = DeviceHasFunctions(bus, device) ? 8 : 1;
            for(int function = 0; function < numFunctions; function++)
            {
                struct pci_descriptor* dev = GetDeviceDescriptor(bus, device, function);
                
                if(dev->vendor_id == 0x0000 || dev->vendor_id == 0xFFFF)
                    continue;

                if (dev->vendor_id == 0x1234 && dev->device_id == 0x1111)
                {
                    struct bar* bar = GetBaseAddressRegister(bus, device, function, 0);
                    if (bar)
                    {
                        return (void*)bar->address;
                    }   
                }
           }
        }
    }
    return 0;
}