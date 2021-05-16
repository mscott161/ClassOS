#ifndef PCI_H
#define PCI_H

#include <stdint.h>
#include <stdbool.h>

#define PCI_COMMAND_PORT 0xCF8
#define PCI_DATA_PORT 0xCFC

enum 
{
    MemoryMapping = 0,
    InputOutput = 1
};

typedef unsigned int BaseAddressRegisterType;

struct bar
{
    bool prefetchable;
    uint8_t* address;
    uint32_t size;
    BaseAddressRegisterType type;
};

struct pci_descriptor
{
    uint32_t portBase;
    uint32_t interrupt;

    uint16_t bus;
    uint16_t device;
    uint16_t function;

    uint16_t vendor_id;
    uint16_t device_id;

    uint8_t class_id;
    uint8_t subclass_id;
    uint8_t interface_id;

    uint8_t revision;
} __attribute__((packed));

void pci_init();
void* pci_get_lfb();

#endif
