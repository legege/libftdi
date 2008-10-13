#include "ftdi.hpp"
#include "ftdi.h"

namespace Ftdi
{

class Context::Private
{
   public:
      Private()
      :  ftdi(0), dev(0), open(false)
      {}

      bool open;
      
      struct ftdi_context* ftdi;
      struct usb_device*   dev;
      
      std::string vendor;
      std::string description;
      std::string serial;
};

/*! \brief Constructor.
 */
Context::Context()
   : d( new Private() )
{
   d->ftdi = ftdi_new();
}

/*! \brief Destructor.
 */
Context::~Context()
{
   if(d->open)
      close();
   
   ftdi_free(d->ftdi);
   delete d;
}

bool Context::is_open()
{
   return d->open;
}

int Context::open(int vendor, int product, const std::string& description, const std::string& serial)
{
   int ret = 0;
   if(description.empty() && serial.empty())
      ret = ftdi_usb_open(d->ftdi, vendor, product);
   else
      ret = ftdi_usb_open_desc(d->ftdi, vendor, product, description.c_str(), serial.c_str());

   d->dev = usb_device(d->ftdi->usb_dev);

   if((ret = ftdi_usb_open_dev(d->ftdi, d->dev)) >= 0)
   {
      d->open = true;
      get_strings();
   }

   return ret;
}

int Context::open(struct usb_device *dev)
{
   int ret = 0;
   
   if(dev != 0)
      d->dev = dev;

   if(d->dev == 0)
      return -1;
   
   if((ret = ftdi_usb_open_dev(d->ftdi, d->dev)) >= 0)
   {
      d->open = true;
      get_strings();
   }

   return ret;
}

int Context::close()
{
   d->open = false;
   return ftdi_usb_close(d->ftdi);
}

int Context::reset()
{
   return ftdi_usb_reset(d->ftdi);
}

int Context::flush(int mask)
{
   int ret = 1;
   if(mask & Input)
      ret &= ftdi_usb_purge_rx_buffer(d->ftdi);
   if(mask & Output)
      ret &= ftdi_usb_purge_tx_buffer(d->ftdi);

   return ret;
}

int Context::set_interface(enum ftdi_interface interface)
{
   return ftdi_set_interface(d->ftdi, interface);
}

void Context::set_usb_device(struct usb_dev_handle *dev)
{
   ftdi_set_usbdev(d->ftdi, dev);
   d->dev = usb_device(dev);
}

int Context::set_baud_rate(int baudrate)
{
   return ftdi_set_baudrate(d->ftdi, baudrate);
}

int Context::set_line_property(enum ftdi_bits_type bits, enum ftdi_stopbits_type sbit, enum ftdi_parity_type parity)
{
   return ftdi_set_line_property(d->ftdi, bits, sbit, parity);
}

int Context::set_line_property(enum ftdi_bits_type bits, enum ftdi_stopbits_type sbit, enum ftdi_parity_type parity, enum ftdi_break_type break_type)
{
   return ftdi_set_line_property2(d->ftdi, bits, sbit, parity, break_type);
}

int Context::read(unsigned char *buf, int size)
{
   return ftdi_read_data(d->ftdi, buf, size);
}

int Context::set_read_chunk_size(unsigned int chunksize)
{
   return ftdi_read_data_set_chunksize(d->ftdi, chunksize);
}

int Context::read_chunk_size()
{
   unsigned chunk = -1;
   if(ftdi_read_data_get_chunksize(d->ftdi, &chunk) < 0)
      return -1;
   
   return chunk;
}


int Context::write(unsigned char *buf, int size)
{
   return ftdi_write_data(d->ftdi, buf, size);
}

int Context::set_write_chunk_size(unsigned int chunksize)
{
   return ftdi_write_data_set_chunksize(d->ftdi, chunksize);
}

int Context::write_chunk_size()
{
   unsigned chunk = -1;
   if(ftdi_write_data_get_chunksize(d->ftdi, &chunk) < 0)
      return -1;

   return chunk;
}

int Context::set_flow_control(int flowctrl)
{
   return ftdi_setflowctrl(d->ftdi, flowctrl);
}

int Context::set_modem_control(int mask)
{
   int dtr = 0, rts = 0;
   
   if(mask & Dtr)
      dtr = 1;
   if(mask & Rts)
      rts = 1;

   return ftdi_setdtr_rts(d->ftdi, dtr, rts);
}

int Context::set_dtr(bool state)
{
   return ftdi_setdtr(d->ftdi, state);
}

int Context::set_rts(bool state)
{
   return ftdi_setrts(d->ftdi, state);
}

int Context::set_latency(unsigned char latency)
{
   return ftdi_set_latency_timer(d->ftdi, latency);
}

unsigned Context::latency()
{
   unsigned char latency = 0;
   ftdi_get_latency_timer(d->ftdi, &latency);
   return latency;
}

unsigned short Context::poll_modem_status()
{
   unsigned short status = 0;
   ftdi_poll_modem_status(d->ftdi, &status);
   return status;
}


int Context::set_event_char(unsigned char eventch, unsigned char enable)
{
   return ftdi_set_event_char(d->ftdi, eventch, enable);
}

int Context::set_error_char(unsigned char errorch, unsigned char enable)
{
   return ftdi_set_error_char(d->ftdi, errorch, enable);
}

int Context::bitbang_enable(unsigned char bitmask)
{
   return ftdi_enable_bitbang(d->ftdi, bitmask);
}

int Context::bitbang_disable()
{
   return ftdi_disable_bitbang(d->ftdi);
}

int Context::set_bitmode(unsigned char bitmask, unsigned char mode)
{
   return ftdi_set_bitmode(d->ftdi, bitmask, mode);
}

int Context::read_pins(unsigned char *pins)
{
   return ftdi_read_pins(d->ftdi, pins);
}

char* Context::error_string()
{
   return ftdi_get_error_string(d->ftdi);
}

int Context::get_strings()
{
   // Prepare buffers
   char vendor[512], desc[512], serial[512];
   
   int ret = ftdi_usb_get_strings(d->ftdi, d->dev, vendor, 512, desc, 512, serial, 512);

   if(ret < 0)
      return -1;
   
   d->vendor = vendor;
   d->description = desc;
   d->serial = serial;

   return 1;
}

/*! \fn vendor
 * \fn description
 * \fn serial
 * \brief Device strings properties.
 */
const std::string& Context::vendor()
{
   return d->vendor;
}

const std::string& Context::description()
{
   return d->description;
}

const std::string& Context::serial()
{
   return d->serial;
}

void Context::set_context(struct ftdi_context* context)
{
   ftdi_free(d->ftdi);
   d->ftdi = context;
}

void Context::set_usb_device(struct usb_device *dev)
{
   d->dev = dev;
}

struct ftdi_context* Context::context()
{
   return d->ftdi;
}

class Eeprom::Private
{
   public:
      Private()
      : context(0)
      {}

      struct ftdi_eeprom eeprom;
      struct ftdi_context* context;
};

Eeprom::Eeprom(Context* parent)
   : d ( new Private() )
{
   d->context = parent->context();
}

Eeprom::~Eeprom()
{
   delete d;
}

void Eeprom::init_defaults()
{
   return ftdi_eeprom_initdefaults(&d->eeprom);
}

void Eeprom::set_size(int size)
{
   return ftdi_eeprom_setsize(d->context, &d->eeprom, size);
}

int Eeprom::size(unsigned char *eeprom, int maxsize)
{
   return ftdi_read_eeprom_getsize(d->context, eeprom, maxsize);
}

int Eeprom::chip_id(unsigned int *chipid)
{
   return ftdi_read_chipid(d->context, chipid);
}

int Eeprom::build(unsigned char *output)
{
   return ftdi_eeprom_build(&d->eeprom, output);
}

int Eeprom::read(unsigned char *eeprom)
{
   return ftdi_read_eeprom(d->context, eeprom);
}

int Eeprom::write(unsigned char *eeprom)
{
   return ftdi_write_eeprom(d->context, eeprom);
}

int Eeprom::erase()
{
   return ftdi_erase_eeprom(d->context);
}

class List::Private
{
   public:
      Private()
      : list(0)
      {}

      struct ftdi_device_list* list;
};

List::List(struct ftdi_device_list* devlist)
   : ListBase(), d( new Private() )
{
   if(devlist != 0)
   {
      // Iterate list
      Context* c = 0;
      for(d->list = devlist; d->list != 0; d->list = d->list->next)
      {
         c = new Context();
         c->set_usb_device(d->list->dev);
         push_back(c);
      }

      // Store pointer
      d->list = devlist;
   }
}

List::~List()
{
   // Deallocate instances
   for(iterator it = begin(); it != end(); it++)
      delete *it;

   // Clear list
   clear();
   ftdi_list_free(&d->list);

   // Delete d-ptr
   delete d;
}

List* List::find_all(int vendor, int product)
{
   struct ftdi_device_list* dlist = 0;
   struct ftdi_context ftdi;
   ftdi_init(&ftdi);
   ftdi_usb_find_all(&ftdi, &dlist, vendor, product);
   ftdi_deinit(&ftdi);
   return new List(dlist);
}

}
