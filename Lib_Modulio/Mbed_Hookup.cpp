//========================================
//=== Modulio C++ Stub for MBed
//========================================
#include "mbed.h"
#include "Modulio.h"

#define I2C_HARD 0	//*** Use Hardware I2C Controller

#if I2C_HARD
I2C i2c(PB_8, PB_9);	//*** (Pin_SDA, Pin_SCL)
#else
#include "Mbed_SoftI2C.h"
SoftI2C i2c(PB_9, PB_8);
#endif

extern "C" {
	void Mbed_i2c_start(void);
	void Mbed_i2c_stop(void);
	int Mbed_i2c_write(UINT8 bData);
	UINT8 Mbed_i2c_read(UINT8 fACK);
	void Mbed_Waitms(UINT16 delay);
	void Mbed_Startup(void);
};


void Mbed_i2c_start(void)
{
	i2c.start();
}

void Mbed_i2c_stop(void)
{
	i2c.stop();
}

int Mbed_i2c_write(UINT8 bData)
{
	i2c.write(bData);
	return(MI2C_ACKslave);
}

UINT8 Mbed_i2c_read(UINT8 fACK)
{
	UINT8 dat;
	dat = i2c.read((fACK == MI2C_fACK) ? 1 : 0);
	return(dat);
}

void Mbed_Waitms(UINT16 delay)
{
	wait_ms(delay);
}

void Mbed_Startup()
{
	i2c.frequency(50000);
}
