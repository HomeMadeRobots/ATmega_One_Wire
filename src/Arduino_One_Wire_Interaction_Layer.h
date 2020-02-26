#ifndef ARDUINO_ONE_WIRE_INTERACTION_LAYER_H
#define ARDUINO_ONE_WIRE_INTERACTION_LAYER_H

#if !defined(__AVR__)
#error "This low level Component_Type is dedicated to be used on project with AVR microcontollers."
#endif


/* Attributes */
#include "stdint.h"

/* Realized interfaces */
#include "i_One_Wire_Protocol.h"

/* Needed interfaces */
#include "Arduino.h"

/* Events */


class Arduino_One_Wire_Interaction_Layer : i_One_Wire_Protocol {
public :
    /*--------------------------------------------------------------------------------------------*/
    /* Constructor */
	Arduino_One_Wire_Interaction_Layer() {}
    Arduino_One_Wire_Interaction_Layer( uint8_t an_arduino_digital_pin_id );

    /*--------------------------------------------------------------------------------------------*/
    /* Component_Type_Operations */

    /*--------------------------------------------------------------------------------------------*/
    /* Event reception points accessors */

    /*--------------------------------------------------------------------------------------------*/
    /* Provided port accessors */
	i_One_Wire_Protocol* Get_Port__Communication( void );
	
	/*--------------------------------------------------------------------------------------------*/
    /* Provided operations */
	void Send_Simple_Command( 
		const T_One_Wire_Device_Address* slave_address,
		uint8_t command ) override;

    virtual void Send_Write_Command(
		const T_One_Wire_Device_Address* slave_address,
		uint8_t command,
		const uint8_t* message,
		uint8_t nb_bytes ) override;

    virtual void Send_Read_Command(
		const T_One_Wire_Device_Address* slave_address,
		uint8_t command,
		uint8_t* message,
		uint8_t nb_bytes ) override;

private :
    /*--------------------------------------------------------------------------------------------*/
    /* Private attributes */
	uint8_t Bitmask;
	volatile uint8_t* Register;

    /*--------------------------------------------------------------------------------------------*/
    /* Private methods */
	void Write_Bit( uint8_t bit_value );
	uint8_t Read_Bit( void );
	void Reset( void );
	void Write( uint8_t byte_value );
	uint8_t Read( void );
	void Write_Bytes( const uint8_t* bytes_array, uint8_t nb_bytes );
	void Read_Bytes( uint8_t* bytes_array, uint8_t nb_bytes );
	void Select( const uint8_t address[8] );
	
	
    /*--------------------------------------------------------------------------------------------*/
    /* Requirer_Ports */

    /*--------------------------------------------------------------------------------------------*/
    /* Provider ports */

    /*--------------------------------------------------------------------------------------------*/
    /* Sent events */

    /*--------------------------------------------------------------------------------------------*/
    /* Received events */
};

#endif