#include "Arduino_One_Wire_Interaction_Layer.h"


/**************************************************************************************************/
/* Constructor */
Arduino_One_Wire_Interaction_Layer::Arduino_One_Wire_Interaction_Layer( 
	uint8_t an_arduino_digital_pin_id )
{
	pinMode( an_arduino_digital_pin_id, INPUT );
	this->Bitmask = digitalPinToBitMask(an_arduino_digital_pin_id);
	this->Register = portInputRegister(digitalPinToPort(an_arduino_digital_pin_id));

}
/**************************************************************************************************/


/**************************************************************************************************/
/* Component_Type_Operations */
/**************************************************************************************************/


/**************************************************************************************************/
/* Event reception points accessors */
/**************************************************************************************************/


/**************************************************************************************************/
/* Prodided port accessors */
/**************************************************************************************************/
i_One_Wire_Protocol* Arduino_One_Wire_Interaction_Layer::Get_Port__Communication( void )
{
	return (i_One_Wire_Protocol*)(this);
}
/**************************************************************************************************/


/**************************************************************************************************/
/* Private methods */
/**************************************************************************************************/
#define DIRECT_READ(base, mask)         (((*(base)) & (mask)) ? 1 : 0)
#define DIRECT_MODE_INPUT(base, mask)   ((*((base)+1)) &= ~(mask))
#define DIRECT_MODE_OUTPUT(base, mask)  ((*((base)+1)) |= (mask))
#define DIRECT_WRITE_LOW(base, mask)    ((*((base)+2)) &= ~(mask))
#define DIRECT_WRITE_HIGH(base, mask)   ((*((base)+2)) |= (mask))
/*------------------------------------------------------------------------------------------------*/
void Arduino_One_Wire_Interaction_Layer::Write_Bit( uint8_t bit_value )
{
	uint8_t mask = this->Bitmask;
	volatile uint8_t* reg = this->Register;

	if(bit_value & 1)
	{
		noInterrupts();
		DIRECT_WRITE_LOW(reg, mask);
		DIRECT_MODE_OUTPUT(reg, mask);
		delayMicroseconds(10);
		DIRECT_WRITE_HIGH(reg, mask);
		interrupts();
		delayMicroseconds(50);
	}
	else
	{
		noInterrupts();
		DIRECT_WRITE_LOW(reg, mask);
		DIRECT_MODE_OUTPUT(reg, mask);
		delayMicroseconds(55);
		DIRECT_WRITE_HIGH(reg, mask);
		interrupts();
		delayMicroseconds(5);
	}	
}
/*------------------------------------------------------------------------------------------------*/
uint8_t Arduino_One_Wire_Interaction_Layer::Read_Bit( void )
{
	uint8_t mask = this->Bitmask;
	volatile uint8_t* reg = this->Register;
	uint8_t bit_value;

	noInterrupts();
	DIRECT_MODE_OUTPUT(reg, mask);
	DIRECT_WRITE_LOW(reg, mask);
	delayMicroseconds(3);
	DIRECT_MODE_INPUT(reg, mask);
	delayMicroseconds(10);
	bit_value = DIRECT_READ(reg, mask);
	interrupts();
	delayMicroseconds(47);
	return bit_value;	
}
/*------------------------------------------------------------------------------------------------*/
void Arduino_One_Wire_Interaction_Layer::Reset( void )
{
	uint8_t mask = this->Bitmask;
	volatile uint8_t* reg = this->Register;
	uint8_t retries = 0;

	noInterrupts();
	DIRECT_MODE_INPUT(reg, mask);
	interrupts();

	/* Wait for the bus is HIGH */
	do
	{
		retries++;
		delayMicroseconds(2);
	} while ( !DIRECT_READ(reg, mask) && retries < 100 );

	if( retries < 100 )
	{
		noInterrupts();
		DIRECT_WRITE_LOW(reg, mask);
		DIRECT_MODE_OUTPUT(reg, mask);
		interrupts();
		delayMicroseconds(480);
		noInterrupts();
		DIRECT_MODE_INPUT(reg, mask);
		delayMicroseconds(60);
		interrupts();
		delayMicroseconds(240);
	}
}
/*------------------------------------------------------------------------------------------------*/
void Arduino_One_Wire_Interaction_Layer::Write( uint8_t byte_value )
{
    uint8_t bitmask;

    for (bitmask = 0x01; bitmask; bitmask <<= 1)
	{
		Write_Bit( (bitmask & byte_value) ? 1 : 0 );
    }
	
}
/*------------------------------------------------------------------------------------------------*/
uint8_t Arduino_One_Wire_Interaction_Layer::Read( void )
{
    uint8_t bitmask;
    uint8_t byte_value = 0;

    for (bitmask = 0x01; bitmask; bitmask <<= 1)
	{
		if ( Read_Bit() )
		{
			byte_value |= bitmask;
		}
    }
    return byte_value;	
}
/*------------------------------------------------------------------------------------------------*/
void Arduino_One_Wire_Interaction_Layer::Write_Bytes( const uint8_t* bytes_array, uint8_t nb_bytes )
{
	for ( uint8_t i = 0 ; i < nb_bytes ; i++ )
	{
		Write( bytes_array[i] );
	}
}
/*------------------------------------------------------------------------------------------------*/
void Arduino_One_Wire_Interaction_Layer::Read_Bytes( uint8_t* bytes_array, uint8_t nb_bytes )
{
	for( uint8_t i = 0 ; i < nb_bytes ; i++ )
	{
		bytes_array[i] = Read();
	}
}
/*------------------------------------------------------------------------------------------------*/
void Arduino_One_Wire_Interaction_Layer::Select( const uint8_t address[8] )
{
    uint8_t i;

    Write( 0x55 );

    for( i = 0; i < 8; i++ )
	{
		Write( address[i] );
	}
}
/**************************************************************************************************/


/**************************************************************************************************/
/* Provided operations */
/**************************************************************************************************/
void Arduino_One_Wire_Interaction_Layer::Send_Simple_Command( 
		T_One_Wire_Device_Address slave_address,
		uint8_t command )
{
	this->Reset();
	this->Select(slave_address);
	this->Write(command);
}
/*------------------------------------------------------------------------------------------------*/
void Arduino_One_Wire_Interaction_Layer::Send_Write_Command(
		T_One_Wire_Device_Address slave_address,
		uint8_t command,
		uint8_t* message,
		uint8_t nb_bytes )
{
	this->Reset();
	this->Select( slave_address );
	this->Write( command );
	this->Write_Bytes( message, nb_bytes );
}
/*------------------------------------------------------------------------------------------------*/
void Arduino_One_Wire_Interaction_Layer::Send_Read_Command(
	T_One_Wire_Device_Address slave_address,
	uint8_t command,
	uint8_t* message,
	uint8_t nb_bytes )
{
	this->Reset();
	this->Select( slave_address );
	this->Write( command );
	this->Read_Bytes( message, nb_bytes );
}
/**************************************************************************************************/


/**************************************************************************************************/
/* Received events */
/**************************************************************************************************/