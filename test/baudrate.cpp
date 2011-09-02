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
    unsigned short expected_value;
    unsigned short expected_index;

    calc_result(int actual, int my_value, int my_index)
        : actual_baudrate(actual)
        , expected_value(my_value)
        , expected_index(my_index)
    {
    }

    calc_result()
        : actual_baudrate(0)
        , expected_value(0)
        , expected_index(0)
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

        // Aid debugging since this test is a generic function
        BOOST_CHECK_MESSAGE(res->actual_baudrate == calc_baudrate && res->expected_value == calc_value && res->expected_index == calc_index,
                            "\n\nERROR: baudrate calculation failed for --" << baudrate.first << " baud--. Details below: ");

        BOOST_CHECK_EQUAL(res->actual_baudrate, calc_baudrate);
        BOOST_CHECK_EQUAL(res->expected_value, calc_value);
        BOOST_CHECK_EQUAL(res->expected_index, calc_index);
    }
}

BOOST_AUTO_TEST_CASE(TypeAMFixedBaudrates)
{
    ftdi->type = TYPE_AM;

    map<int, calc_result> baudrates;
    baudrates[300] = calc_result(300, 10000, 0);
    baudrates[600] = calc_result(600, 5000, 0);
    baudrates[600] = calc_result(600, 5000, 0);
    baudrates[1200] = calc_result(1200, 2500, 0);
    baudrates[2400] = calc_result(2400, 1250, 0);
    baudrates[4800] = calc_result(4800, 625, 0);
    baudrates[9600] = calc_result(9600, 16696, 0);
    baudrates[19200] = calc_result(19200, 32924, 0);
    baudrates[38400] = calc_result(38400, 49230, 0);
    baudrates[57600] = calc_result(57554, 49204, 0);
    baudrates[115200] = calc_result(115385, 26, 0);
    baudrates[230400] = calc_result(230769, 13, 0);
    baudrates[460800] = calc_result(461538, 16390, 0);
    baudrates[921600] = calc_result(923077, 32771, 0);

    test_baudrates(ftdi, baudrates);
}

BOOST_AUTO_TEST_CASE(TypeBMFixedBaudrates)
{
    ftdi->type = TYPE_BM;

    map<int, calc_result> baudrates;
    baudrates[300] = calc_result(300, 10000, 0);
    baudrates[600] = calc_result(600, 5000, 0);
    baudrates[600] = calc_result(600, 5000, 0);
    baudrates[1200] = calc_result(1200, 2500, 0);
    baudrates[2400] = calc_result(2400, 1250, 0);
    baudrates[4800] = calc_result(4800, 625, 0);
    baudrates[9600] = calc_result(9600, 16696, 0);
    baudrates[19200] = calc_result(19200, 32924, 0);
    baudrates[38400] = calc_result(38400, 49230, 0);
    baudrates[57600] = calc_result(57554, 49204, 0);
    baudrates[115200] = calc_result(115385, 26, 0);
    baudrates[230400] = calc_result(230769, 13, 0);
    baudrates[460800] = calc_result(461538, 16390, 0);
    baudrates[921600] = calc_result(923077, 32771, 0);

    test_baudrates(ftdi, baudrates);
}

BOOST_AUTO_TEST_CASE(Type2232CFixedBaudrates)
{
    ftdi->type = TYPE_2232C;

    map<int, calc_result> baudrates;
    baudrates[300] = calc_result(300, 10000, 1);
    baudrates[600] = calc_result(600, 5000, 1);
    baudrates[600] = calc_result(600, 5000, 1);
    baudrates[1200] = calc_result(1200, 2500, 1);
    baudrates[2400] = calc_result(2400, 1250, 1);
    baudrates[4800] = calc_result(4800, 625, 1);
    baudrates[9600] = calc_result(9600, 16696, 1);
    baudrates[19200] = calc_result(19200, 32924, 1);
    baudrates[38400] = calc_result(38400, 49230, 1);
    baudrates[57600] = calc_result(57554, 49204, 1);
    baudrates[115200] = calc_result(115385, 26, 1);
    baudrates[230400] = calc_result(230769, 13, 1);
    baudrates[460800] = calc_result(461538, 16390, 1);
    baudrates[921600] = calc_result(923077, 32771, 1);

    test_baudrates(ftdi, baudrates);
}

BOOST_AUTO_TEST_CASE(TypeRFixedBaudrates)
{
    ftdi->type = TYPE_R;

    map<int, calc_result> baudrates;
    baudrates[300] = calc_result(300, 10000, 0);
    baudrates[600] = calc_result(600, 5000, 0);
    baudrates[600] = calc_result(600, 5000, 0);
    baudrates[1200] = calc_result(1200, 2500, 0);
    baudrates[2400] = calc_result(2400, 1250, 0);
    baudrates[4800] = calc_result(4800, 625, 0);
    baudrates[9600] = calc_result(9600, 16696, 0);
    baudrates[19200] = calc_result(19200, 32924, 0);
    baudrates[38400] = calc_result(38400, 49230, 0);
    baudrates[57600] = calc_result(57554, 49204, 0);
    baudrates[115200] = calc_result(115385, 26, 0);
    baudrates[230400] = calc_result(230769, 13, 0);
    baudrates[460800] = calc_result(461538, 16390, 0);
    baudrates[921600] = calc_result(923077, 32771, 0);

    test_baudrates(ftdi, baudrates);
}

BOOST_AUTO_TEST_CASE(Type2232HFixedBaudrates)
{
    ftdi->type = TYPE_2232H;

    map<int, calc_result> baudrates;
    baudrates[300] = calc_result(300, 10000, 1);
    baudrates[600] = calc_result(600, 5000, 1);
    baudrates[600] = calc_result(600, 5000, 1);
    baudrates[1200] = calc_result(1200, 2500, 1);
    baudrates[2400] = calc_result(2400, 1250, 1);
    baudrates[4800] = calc_result(4800, 625, 1);
    baudrates[9600] = calc_result(9600, 16696, 1);
    baudrates[19200] = calc_result(19200, 32924, 1);
    baudrates[38400] = calc_result(38400, 49230, 1);
    baudrates[57600] = calc_result(57554, 49204, 1);
    baudrates[115200] = calc_result(115385, 26, 1);
    baudrates[230400] = calc_result(230769, 13, 1);
    baudrates[460800] = calc_result(461538, 16390, 1);
    baudrates[921600] = calc_result(923077, 32771, 1);

    test_baudrates(ftdi, baudrates);
}

BOOST_AUTO_TEST_CASE(Type4232HFixedBaudrates)
{
    ftdi->type = TYPE_4232H;

    map<int, calc_result> baudrates;
    baudrates[300] = calc_result(300, 10000, 1);
    baudrates[600] = calc_result(600, 5000, 1);
    baudrates[600] = calc_result(600, 5000, 1);
    baudrates[1200] = calc_result(1200, 2500, 1);
    baudrates[2400] = calc_result(2400, 1250, 1);
    baudrates[4800] = calc_result(4800, 625, 1);
    baudrates[9600] = calc_result(9600, 16696, 1);
    baudrates[19200] = calc_result(19200, 32924, 1);
    baudrates[38400] = calc_result(38400, 49230, 1);
    baudrates[57600] = calc_result(57554, 49204, 1);
    baudrates[115200] = calc_result(115385, 26, 1);
    baudrates[230400] = calc_result(230769, 13, 1);
    baudrates[460800] = calc_result(461538, 16390, 1);
    baudrates[921600] = calc_result(923077, 32771, 1);

    test_baudrates(ftdi, baudrates);
}

BOOST_AUTO_TEST_CASE(Type232HFixedBaudrates)
{
    ftdi->type = TYPE_232H;

    map<int, calc_result> baudrates;
    baudrates[300] = calc_result(300, 10000, 1);
    baudrates[600] = calc_result(600, 5000, 1);
    baudrates[600] = calc_result(600, 5000, 1);
    baudrates[1200] = calc_result(1200, 2500, 1);
    baudrates[2400] = calc_result(2400, 1250, 1);
    baudrates[4800] = calc_result(4800, 625, 1);
    baudrates[9600] = calc_result(9600, 16696, 1);
    baudrates[19200] = calc_result(19200, 32924, 1);
    baudrates[38400] = calc_result(38400, 49230, 1);
    baudrates[57600] = calc_result(57554, 49204, 1);
    baudrates[115200] = calc_result(115385, 26, 1);
    baudrates[230400] = calc_result(230769, 13, 1);
    baudrates[460800] = calc_result(461538, 16390, 1);
    baudrates[921600] = calc_result(923077, 32771, 1);

    test_baudrates(ftdi, baudrates);
}

BOOST_AUTO_TEST_SUITE_END()
