/**@file
@brief Test baudrate calculator code

@author Thomas Jarosch
*/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License           *
 *   version 2.1 as published by the Free Software Foundation;             *
 *                                                                         *
 ***************************************************************************/

#include <ftdi.h>

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/foreach.hpp>
#include <vector>
#include <map>

using namespace std;

extern "C" int convert_baudrate_UT_export(int baudrate, struct ftdi_context *ftdi,
                                 unsigned short *value, unsigned short *index);

/// Basic initialization of libftdi for every test
class BaseFTDIFixture
{
protected:
    ftdi_context *ftdi;

public:
    BaseFTDIFixture()
        : ftdi(NULL)
    {
        ftdi = ftdi_new();
    }

    ~BaseFTDIFixture()
    {
        delete ftdi;
        ftdi = NULL;
    }
};

BOOST_FIXTURE_TEST_SUITE(Baudrate, BaseFTDIFixture)

/// Helper class to store the convert_baudrate_UT_export result
struct calc_result
{
    int actual_baudrate;
    unsigned short divisor;
    unsigned short fractional_bits;
    unsigned short clock;

    calc_result(int actual, unsigned short my_divisor, unsigned short my_fractional_bits, unsigned short my_clock)
        : actual_baudrate(actual)
        , divisor(my_divisor)
        , fractional_bits(my_fractional_bits)
        , clock(my_clock)
    {
    }

    calc_result()
        : actual_baudrate(0)
        , divisor(0)
        , fractional_bits(0)
        , clock(0)
    {
    }
};

/**
 * @brief Test convert_baudrate code against a list of baud rates
 *
 * @param baudrates Baudrates to check
 **/
static void test_baudrates(ftdi_context *ftdi, const map<int, calc_result> &baudrates)
{
    typedef std::pair<int, calc_result> baudrate_type;
    BOOST_FOREACH(const baudrate_type &baudrate, baudrates)
    {
        unsigned short calc_value = 0, calc_index = 0;
        int calc_baudrate = convert_baudrate_UT_export(baudrate.first, ftdi, &calc_value, &calc_index);

        const calc_result *res = &baudrate.second;

        unsigned short divisor = calc_value & 0x3fff;
        unsigned short fractional_bits = (calc_value >> 14);
        unsigned short clock = (calc_index & 0x200) ? 120 : 48;

        switch (ftdi->type)
        {
        case TYPE_232H:
        case TYPE_2232H:
        case TYPE_4232H:
            fractional_bits |= (calc_index & 0x100) ? 4 : 0;
            break;
        case TYPE_R:
        case TYPE_2232C:
        case TYPE_BM:
            fractional_bits |= (calc_index & 0x001) ? 4 : 0;
            break;
        default:;
        }
        // Aid debugging since this test is a generic function
        BOOST_CHECK_MESSAGE(res->actual_baudrate == calc_baudrate && res->divisor == divisor && res->fractional_bits == fractional_bits
                            && res->clock == clock,
                            "\n\nERROR: baudrate calculation failed for --" << baudrate.first << " baud--. Details below: ");

        BOOST_CHECK_EQUAL(res->actual_baudrate, calc_baudrate);
        BOOST_CHECK_EQUAL(res->divisor, divisor);
        BOOST_CHECK_EQUAL(res->fractional_bits, fractional_bits);
        BOOST_CHECK_EQUAL(res->clock, clock);
    }
}

BOOST_AUTO_TEST_CASE(TypeAMFixedBaudrates)
{
    ftdi->type = TYPE_AM;

    map<int, calc_result> baudrates;
    baudrates[300] = calc_result(300, 784, 2, 48);
    baudrates[600] = calc_result(600, 904, 1, 48);
    baudrates[1200] = calc_result(1200, 452, 0, 48);
    baudrates[2400] = calc_result(2400, 226, 0, 48);
    baudrates[4800] = calc_result(4800, 625, 0, 48);
    baudrates[9600] = calc_result(9600, 312, 4, 48);
    baudrates[19200] = calc_result(19200, 156, 8, 48);
    baudrates[38400] = calc_result(38400, 78, 12, 48);
    baudrates[57600] = calc_result(57554, 52, 12, 48);
    baudrates[115200] = calc_result(115385, 26, 0, 48);
    baudrates[230400] = calc_result(230769, 13, 0, 48);
    baudrates[460800] = calc_result(461538, 6, 4, 48);
    baudrates[921600] = calc_result(923077, 3, 8, 48);

    test_baudrates(ftdi, baudrates);
}

BOOST_AUTO_TEST_CASE(TypeBMFixedBaudrates)
{
    // Unify testing of chips behaving the same
    std::vector<enum ftdi_chip_type> test_types;
    test_types.push_back(TYPE_BM);
    test_types.push_back(TYPE_2232C);
    test_types.push_back(TYPE_R);

    map<int, calc_result> baudrates;
    baudrates[300] = calc_result(300, 784, 2, 48);
    baudrates[600] = calc_result(600, 904, 1, 48);
    baudrates[1200] = calc_result(1200, 452, 0, 48);
    baudrates[2400] = calc_result(2400, 226, 0, 48);
    baudrates[4800] = calc_result(4800, 625, 0, 48);
    baudrates[9600] = calc_result(9600, 312, 4, 48);
    baudrates[19200] = calc_result(19200, 156, 8, 48);
    baudrates[38400] = calc_result(38400, 78, 12, 48);
    baudrates[57600] = calc_result(57553, 52, 12, 48);
    baudrates[115200] = calc_result(115384, 26, 0, 48);
    baudrates[230400] = calc_result(230769, 13, 0, 48);
    baudrates[460800] = calc_result(461538, 6, 4, 48);
    baudrates[921600] = calc_result(923076, 3, 8, 48);

    BOOST_FOREACH(const enum ftdi_chip_type &test_chip_type, test_types)
    {
        ftdi->type = test_chip_type;
        test_baudrates(ftdi, baudrates);
    }
}

BOOST_AUTO_TEST_CASE(TypeHFixedBaudrates)
{
    // Unify testing of chips behaving the same
    std::vector<enum ftdi_chip_type> test_types;
    test_types.push_back(TYPE_2232H);
    test_types.push_back(TYPE_4232H);
    test_types.push_back(TYPE_232H);

    map<int, calc_result> baudrates;
    baudrates[300] = calc_result(300, 784, 2, 48);
    baudrates[600] = calc_result(600, 904, 1, 48);
    baudrates[1200] = calc_result(1200, 784, 2, 48);
    baudrates[2400] = calc_result(2400, 904, 1, 48);
    baudrates[4800] = calc_result(4800, 452, 0, 48);
    baudrates[9600] = calc_result(9600, 226, 0, 48);
    baudrates[19200] = calc_result(19200, 625, 0, 48);
    baudrates[38400] = calc_result(38400, 312, 4, 48);
    baudrates[57600] = calc_result(57588, 208, 4, 120);
    baudrates[115200] = calc_result(115246, 104, 12, 48);
    baudrates[230400] = calc_result(230215, 52, 12, 48);
    baudrates[460800] = calc_result(461538, 26, 0, 48);
    baudrates[921600] = calc_result(923076, 13, 0, 48);

    BOOST_FOREACH(const enum ftdi_chip_type &test_chip_type, test_types)
    {
        ftdi->type = test_chip_type;
        test_baudrates(ftdi, baudrates);
    }
}

BOOST_AUTO_TEST_SUITE_END()
