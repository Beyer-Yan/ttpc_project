#include "ttpdef.h"
#include "ttpconstants.h"
/** 16k medl region */ 
/** data for node 0 */
#if defined (TTP_NODE0)
const static uint8_t medl_data[16*1024] __SECTION("MEDL_SECTION") = 
{
    0x10, 0x01, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 
    0x03, 0x02, 0x32, 0x05, 0x10, 0x27, 0x00, 0x00, 
    0x10, 0x27, 0x00, 0x00, 0xc2, 0x59, 0x00, 0x00, 
    0xe8, 0x03, 0x00, 0x00, 0xf8, 0x2a, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 
    0x05, 0x00, 0x05, 0x00, 0xA8, 0x00, 0x00, 0x00, 
    0x05, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0xe8, 0x03, 0xc8, 0x64, 0x99, 0x26, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x17, 0x2c, 0x01, 
    0x00, 0x00, 0x01, 0x00, 0xe8, 0x03, 0xc8, 0x64, 
    0x99, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x01, 0x17, 0x14, 0x05, 0x00, 0x00, 0x02, 0x00, 
    0xe8, 0x03, 0xc8, 0x64, 0x99, 0x26, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x02, 0x17, 0xfc, 0x08, 
    0x00, 0x00, 0x03, 0x00, 0xe8, 0x03, 0xc8, 0x64, 
    0x99, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x03, 0x17, 0xe4, 0x0c, 0x00, 0x00, 0x04, 0x00, 
    0xe8, 0x03, 0xc8, 0x64, 0x99, 0x26, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x04, 0x1f, 0xcc, 0x10, 
    0x00, 0x00, 0x00, 0x00, 0xe8, 0x03, 0xc8, 0x64, 
    0x99, 0x26, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x17, 0x2c, 0x01, 0x00, 0x00, 0x01, 0x00, 
    0xe8, 0x03, 0xc8, 0x64, 0x99, 0x26, 0x02, 0x00, 
    0x04, 0x00, 0x00, 0x00, 0x01, 0x17, 0x14, 0x05, 
    0x00, 0x00, 0x02, 0x00, 0xe8, 0x03, 0xc8, 0x64, 
    0x99, 0x26, 0x02, 0x00, 0x08, 0x00, 0x00, 0x00, 
    0x02, 0x17, 0xfc, 0x08, 0x00, 0x00, 0x03, 0x00, 
    0xe8, 0x03, 0xc8, 0x64, 0x99, 0x26, 0x02, 0x00, 
    0x0c, 0x00, 0x00, 0x00, 0x03, 0x17, 0xe4, 0x0c, 
    0x00, 0x00, 0x04, 0x00, 0xe8, 0x03, 0xc8, 0x64, 
    0x99, 0x26, 0x02, 0x00, 0x10, 0x00, 0x00, 0x00, 
    0x04, 0x1f, 0xcc, 0x10
};
#endif

#if defined (TTP_NODE1)
const static uint8_t medl_data[16*1024] __SECTION("MEDL_SECTION") = 
{
    0x10, 0x01, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 
    0x02, 0x02, 0x0a, 0x05, 0x10, 0x27, 0x00, 0x00, 
    0x10, 0x27, 0x00, 0x00, 0xc2, 0x59, 0x00, 0x00, 
    0xd0, 0x07, 0x00, 0x00, 0xe0, 0x2e, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 
    0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 
    0x01, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 
    0x05, 0x00, 0x05, 0x00, 0xA8, 0x00, 0x00, 0x00, 
    0x05, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0xe8, 0x03, 0xc8, 0x64, 0xxx, 0xxx, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x17, 0x2c, 0x01, 
    0x00, 0x00, 0x01, 0x00, 0xe8, 0x03, 0xc8, 0x64, 
    0xxx, 0xxx, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x01, 0x17, 0x14, 0x05, 0x00, 0x00, 0x02, 0x00, 
    0xe8, 0x03, 0xc8, 0x64, 0xxx, 0xxx, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x02, 0x17, 0xfc, 0x08, 
    0x00, 0x00, 0x03, 0x00, 0xe8, 0x03, 0xc8, 0x64, 
    0xxx, 0xxx, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x03, 0x17, 0xe4, 0x0c, 0x00, 0x00, 0x04, 0x00, 
    0xe8, 0x03, 0xc8, 0x64, 0xxx, 0xxx, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x04, 0x1f, 0xcc, 0x10, 
    0x00, 0x00, 0x00, 0x00, 0xe8, 0x03, 0xc8, 0x64, 
    0xxx, 0xxx, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x17, 0x2c, 0x01, 0x00, 0x00, 0x01, 0x00, 
    0xe8, 0x03, 0xc8, 0x64, 0xxx, 0xxx, 0x02, 0x00, 
    0x04, 0x00, 0x00, 0x00, 0x01, 0x17, 0x14, 0x05, 
    0x00, 0x00, 0x02, 0x00, 0xe8, 0x03, 0xc8, 0x64, 
    0xxx, 0xxx, 0x02, 0x00, 0x08, 0x00, 0x00, 0x00, 
    0x02, 0x17, 0xfc, 0x08, 0x00, 0x00, 0x03, 0x00, 
    0xe8, 0x03, 0xc8, 0x64, 0xxx, 0xxx, 0x02, 0x00, 
    0x0c, 0x00, 0x00, 0x00, 0x03, 0x17, 0xe4, 0x0c, 
    0x00, 0x00, 0x04, 0x00, 0xe8, 0x03, 0xc8, 0x64, 
    0xxx, 0xxx, 0x02, 0x00, 0x10, 0x00, 0x00, 0x00, 
    0x04, 0x1f, 0xcc, 0x10
};
#endif

#if defined (TTP_NODE2)
const static uint8_t medl_data[16*1024] __SECTION("MEDL_SECTION") = 
{
    0x10, 0x01, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 
    0x02, 0x02, 0x0a, 0x05, 0x10, 0x27, 0x00, 0x00, 
    0x10, 0x27, 0x00, 0x00, 0xc2, 0x59, 0x00, 0x00, 
    0xd0, 0x07, 0x00, 0x00, 0xe0, 0x2e, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 
    0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 
    0x01, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 
    0x05, 0x00, 0x05, 0x00, 0xA8, 0x00, 0x00, 0x00, 
    0x05, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0xe8, 0x03, 0xc8, 0x64, 0xxx, 0xxx, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x17, 0x2c, 0x01, 
    0x00, 0x00, 0x01, 0x00, 0xe8, 0x03, 0xc8, 0x64, 
    0xxx, 0xxx, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x01, 0x17, 0x14, 0x05, 0x00, 0x00, 0x02, 0x00, 
    0xe8, 0x03, 0xc8, 0x64, 0xxx, 0xxx, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x02, 0x17, 0xfc, 0x08, 
    0x00, 0x00, 0x03, 0x00, 0xe8, 0x03, 0xc8, 0x64, 
    0xxx, 0xxx, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x03, 0x17, 0xe4, 0x0c, 0x00, 0x00, 0x04, 0x00, 
    0xe8, 0x03, 0xc8, 0x64, 0xxx, 0xxx, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x04, 0x1f, 0xcc, 0x10, 
    0x00, 0x00, 0x00, 0x00, 0xe8, 0x03, 0xc8, 0x64, 
    0xxx, 0xxx, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x17, 0x2c, 0x01, 0x00, 0x00, 0x01, 0x00, 
    0xe8, 0x03, 0xc8, 0x64, 0xxx, 0xxx, 0x02, 0x00, 
    0x04, 0x00, 0x00, 0x00, 0x01, 0x17, 0x14, 0x05, 
    0x00, 0x00, 0x02, 0x00, 0xe8, 0x03, 0xc8, 0x64, 
    0xxx, 0xxx, 0x02, 0x00, 0x08, 0x00, 0x00, 0x00, 
    0x02, 0x17, 0xfc, 0x08, 0x00, 0x00, 0x03, 0x00, 
    0xe8, 0x03, 0xc8, 0x64, 0xxx, 0xxx, 0x02, 0x00, 
    0x0c, 0x00, 0x00, 0x00, 0x03, 0x17, 0xe4, 0x0c, 
    0x00, 0x00, 0x04, 0x00, 0xe8, 0x03, 0xc8, 0x64, 
    0xxx, 0xxx, 0x02, 0x00, 0x10, 0x00, 0x00, 0x00, 
    0x04, 0x1f, 0xcc, 0x10
};
#endif

#if defined (TTP_NODE3)
const static uint8_t medl_data[16*1024] __SECTION("MEDL_SECTION") = 
{
    0x10, 0x01, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 
    0x02, 0x02, 0x0a, 0x05, 0x10, 0x27, 0x00, 0x00, 
    0x10, 0x27, 0x00, 0x00, 0xc2, 0x59, 0x00, 0x00, 
    0xa0, 0x0f, 0x00, 0x00, 0xb0, 0x36, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 
    0x00, 0x03, 0x01, 0x00, 0x03, 0x00, 0x00, 0x00, 
    0x03, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00,
    0x05, 0x00, 0x05, 0x00, 0xA8, 0x00, 0x00, 0x00, 
    0x05, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0xe8, 0x03, 0xc8, 0x64, 0xxx, 0xxx, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x17, 0x2c, 0x01, 
    0x00, 0x00, 0x01, 0x00, 0xe8, 0x03, 0xc8, 0x64, 
    0xxx, 0xxx, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x01, 0x17, 0x14, 0x05, 0x00, 0x00, 0x02, 0x00, 
    0xe8, 0x03, 0xc8, 0x64, 0xxx, 0xxx, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x02, 0x17, 0xfc, 0x08, 
    0x00, 0x00, 0x03, 0x00, 0xe8, 0x03, 0xc8, 0x64, 
    0xxx, 0xxx, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x03, 0x17, 0xe4, 0x0c, 0x00, 0x00, 0x04, 0x00, 
    0xe8, 0x03, 0xc8, 0x64, 0xxx, 0xxx, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x04, 0x1f, 0xcc, 0x10, 
    0x00, 0x00, 0x00, 0x00, 0xe8, 0x03, 0xc8, 0x64, 
    0xxx, 0xxx, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x17, 0x2c, 0x01, 0x00, 0x00, 0x01, 0x00, 
    0xe8, 0x03, 0xc8, 0x64, 0xxx, 0xxx, 0x02, 0x00, 
    0x04, 0x00, 0x00, 0x00, 0x01, 0x17, 0x14, 0x05, 
    0x00, 0x00, 0x02, 0x00, 0xe8, 0x03, 0xc8, 0x64, 
    0xxx, 0xxx, 0x02, 0x00, 0x08, 0x00, 0x00, 0x00, 
    0x02, 0x17, 0xfc, 0x08, 0x00, 0x00, 0x03, 0x00, 
    0xe8, 0x03, 0xc8, 0x64, 0xxx, 0xxx, 0x02, 0x00, 
    0x0c, 0x00, 0x00, 0x00, 0x03, 0x17, 0xe4, 0x0c, 
    0x00, 0x00, 0x04, 0x00, 0xe8, 0x03, 0xc8, 0x64, 
    0xxx, 0xxx, 0x02, 0x00, 0x10, 0x00, 0x00, 0x00, 
    0x04, 0x1f, 0xcc, 0x10
};
#endif

#if defined (TTP_NODE4)
const static uint8_t medl_data[16*1024] __SECTION("MEDL_SECTION") = 
{
    0x10, 0x01, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 
    0x02, 0x02, 0x0a, 0x05, 0x10, 0x27, 0x00, 0x00, 
    0x10, 0x27, 0x00, 0x00, 0xc2, 0x59, 0x00, 0x00, 
    0x88, 0x13, 0x00, 0x00, 0x98, 0x3a, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 
    0x00, 0x04, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00, 
    0x04, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 
    0x05, 0x00, 0x05, 0x00, 0xA8, 0x00, 0x00, 0x00, 
    0x05, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0xe8, 0x03, 0xc8, 0x64, 0xxx, 0xxx, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x17, 0x2c, 0x01, 
    0x00, 0x00, 0x01, 0x00, 0xe8, 0x03, 0xc8, 0x64, 
    0xxx, 0xxx, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x01, 0x17, 0x14, 0x05, 0x00, 0x00, 0x02, 0x00, 
    0xe8, 0x03, 0xc8, 0x64, 0xxx, 0xxx, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x02, 0x17, 0xfc, 0x08, 
    0x00, 0x00, 0x03, 0x00, 0xe8, 0x03, 0xc8, 0x64, 
    0xxx, 0xxx, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x03, 0x17, 0xe4, 0x0c, 0x00, 0x00, 0x04, 0x00, 
    0xe8, 0x03, 0xc8, 0x64, 0xxx, 0xxx, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x04, 0x1f, 0xcc, 0x10, 
    0x00, 0x00, 0x00, 0x00, 0xe8, 0x03, 0xc8, 0x64, 
    0xxx, 0xxx, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x17, 0x2c, 0x01, 0x00, 0x00, 0x01, 0x00, 
    0xe8, 0x03, 0xc8, 0x64, 0xxx, 0xxx, 0x02, 0x00, 
    0x04, 0x00, 0x00, 0x00, 0x01, 0x17, 0x14, 0x05, 
    0x00, 0x00, 0x02, 0x00, 0xe8, 0x03, 0xc8, 0x64, 
    0xxx, 0xxx, 0x02, 0x00, 0x08, 0x00, 0x00, 0x00, 
    0x02, 0x17, 0xfc, 0x08, 0x00, 0x00, 0x03, 0x00, 
    0xe8, 0x03, 0xc8, 0x64, 0xxx, 0xxx, 0x02, 0x00, 
    0x0c, 0x00, 0x00, 0x00, 0x03, 0x17, 0xe4, 0x0c, 
    0x00, 0x00, 0x04, 0x00, 0xe8, 0x03, 0xc8, 0x64, 
    0xxx, 0xxx, 0x02, 0x00, 0x10, 0x00, 0x00, 0x00, 
    0x04, 0x1f, 0xcc, 0x10
};
#endif

#if defined (TTP_NODE5)
const static uint8_t medl_data[16*1024] __SECTION("MEDL_SECTION") = 
{
    0x10, 0x01, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 
    0x02, 0x02, 0x0a, 0x05, 0x10, 0x27, 0x00, 0x00, 
    0x10, 0x27, 0x00, 0x00, 0xc2, 0x59, 0x00, 0x00, 
    0x88, 0x13, 0x00, 0x00, 0x98, 0x3a, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 
    0x01, 0x04, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00, 
    0x04, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 
    0x05, 0x00, 0x05, 0x00, 0xA8, 0x00, 0x00, 0x00, 
    0x05, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0xe8, 0x03, 0xc8, 0x64, 0xxx, 0xxx, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x17, 0x2c, 0x01, 
    0x00, 0x00, 0x01, 0x00, 0xe8, 0x03, 0xc8, 0x64, 
    0xxx, 0xxx, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x01, 0x17, 0x14, 0x05, 0x00, 0x00, 0x02, 0x00, 
    0xe8, 0x03, 0xc8, 0x64, 0xxx, 0xxx, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x02, 0x17, 0xfc, 0x08, 
    0x00, 0x00, 0x03, 0x00, 0xe8, 0x03, 0xc8, 0x64, 
    0xxx, 0xxx, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x03, 0x17, 0xe4, 0x0c, 0x00, 0x00, 0x04, 0x00, 
    0xe8, 0x03, 0xc8, 0x64, 0xxx, 0xxx, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x04, 0x1f, 0xcc, 0x10, 
    0x00, 0x00, 0x00, 0x00, 0xe8, 0x03, 0xc8, 0x64, 
    0xxx, 0xxx, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x17, 0x2c, 0x01, 0x00, 0x00, 0x01, 0x00, 
    0xe8, 0x03, 0xc8, 0x64, 0xxx, 0xxx, 0x02, 0x00, 
    0x04, 0x00, 0x00, 0x00, 0x01, 0x17, 0x14, 0x05, 
    0x00, 0x00, 0x02, 0x00, 0xe8, 0x03, 0xc8, 0x64, 
    0xxx, 0xxx, 0x02, 0x00, 0x08, 0x00, 0x00, 0x00, 
    0x02, 0x17, 0xfc, 0x08, 0x00, 0x00, 0x03, 0x00, 
    0xe8, 0x03, 0xc8, 0x64, 0xxx, 0xxx, 0x02, 0x00, 
    0x0c, 0x00, 0x00, 0x00, 0x03, 0x17, 0xe4, 0x0c, 
    0x00, 0x00, 0x04, 0x00, 0xe8, 0x03, 0xc8, 0x64, 
    0xxx, 0xxx, 0x02, 0x00, 0x10, 0x00, 0x00, 0x00, 
    0x04, 0x1f, 0xcc, 0x10
};
#endif

const uint8_t* medl_get_base_addr(void)
{
    return medl_data;
}
