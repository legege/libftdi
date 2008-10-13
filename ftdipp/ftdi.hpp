#ifndef FTDICPP_H
#define FTDICPP_H

#include <list>
#include <string>
#include "ftdi.h"

namespace Ftdi
{

/* Forward declarations*/
class List;
class Eeprom;

/*! \brief FTDI device context.
 * Represents single FTDI device context.
 */
class Context
{
   /* Friends */
   friend class Eeprom;
   friend class List;
   
   public:

      /*! \brief Direction flags for flush().
       */
      enum Direction
      {
         Input,
         Output
      };

      /*! \brief Modem control flags.
       */
      enum ModemCtl
      {
         Dtr,
         Rts
      };

      /* Constructor, Destructor */
      Context();
      ~Context();
      
      /* Properties */
      Eeprom* eeprom();
      const std::string& vendor();
      const std::string& description();
      const std::string& serial();

      /* Device manipulators */
      bool is_open();
      int open(struct usb_device *dev = 0);
      int open(int vendor, int product, const std::string& description = std::string(), const std::string& serial = std::string());
      int close();
      int reset();
      int flush(int mask = Input|Output);
      int set_interface(enum ftdi_interface interface);
      void set_usb_device(struct usb_dev_handle *dev);
 
      /* Line manipulators */
      int set_baud_rate(int baudrate);
      int set_line_property(enum ftdi_bits_type bits, enum ftdi_stopbits_type sbit, enum ftdi_parity_type parity);
      int set_line_property(enum ftdi_bits_type bits, enum ftdi_stopbits_type sbit, enum ftdi_parity_type parity, enum ftdi_break_type break_type);

      /* I/O */
      int read(unsigned char *buf, int size);
      int write(unsigned char *buf, int size);
      int set_read_chunk_size(unsigned int chunksize);
      int set_write_chunk_size(unsigned int chunksize);
      int read_chunk_size();
      int write_chunk_size();

      /* Async IO
      TODO: should wrap?
      int writeAsync(unsigned char *buf, int size);
      void asyncComplete(int wait_for_more);
      */

      /* Flow control */
      int set_event_char(unsigned char eventch, unsigned char enable);
      int set_error_char(unsigned char errorch, unsigned char enable);
      int set_flow_control(int flowctrl);
      int set_modem_control(int mask = Dtr|Rts);
      int set_latency(unsigned char latency);
      int set_dtr(bool state);
      int set_rts(bool state);
      
      unsigned short poll_modem_status();
      unsigned latency();

      /* BitBang mode */
      int set_bitmode(unsigned char bitmask, unsigned char mode);
      int bitbang_enable(unsigned char bitmask);
      int bitbang_disable();
      int read_pins(unsigned char *pins);

      /* Misc */
      char* error_string();

   protected:
      int get_strings();

      /* Properties */
      struct ftdi_context* context();
      void set_context(struct ftdi_context* context);
      void set_usb_device(struct usb_device *dev);

   private:
      class Private;
      Private *d;

      /* Disable copy constructor */
      Context(const Context &) {}
      Context& operator=(const Context &) {}
};

/*! \brief Device EEPROM.
 */
class Eeprom
{
   public:
      Eeprom(Context* parent);
      ~Eeprom();

      void init_defaults();
      void set_size(int size);
      int size(unsigned char *eeprom, int maxsize);
      int chip_id(unsigned int *chipid);
      int build(unsigned char *output);
      int read(unsigned char *eeprom);
      int write(unsigned char *eeprom);
      int erase();

   private:
      class Private;
      Private *d;
};

typedef std::list<Context*> ListBase;

/*! \brief Device list.
 */
class List : public ListBase
{
   public:
      List(struct ftdi_device_list* devlist = 0);
      ~List();

      static List* find_all(int vendor, int product);

   private:
      class Private;
      Private *d;
};

}

#endif
