
	GXEV( Off, 0 )				///< Led state: turn off.
	GXEV( OG, 0x02 )			///< The led type is bi-color: green/orange. Set to this value is not permitted.
	GXEV( ORG, 0x03 )			///< The led type is tri-color: red/green/orange. Set to this value is not permitted.
	GXEV( System, 0x10 )		///< Led type. The led is controlled by the system only. Set to this value is not permitted.
	GXEV( Green, 0x81 )			///< State/type. The color of the led is green.
	GXEV( Red, 0x82 )			///< State/type. The color of the led is red.
	GXEV( Orange, 0x83 )		///< State/type. The color of the led is orange.
	GXEV( Blue, 0x84 )			///< State/type. The color of the led is blue.
	GXEV( Auto, 0xac )			///< Led state: turn on automatic led control.
	GXEV( On, 0xff )			///< Led state: turn on any of the available lights.
	GXEV( Buzzer, 0xc0 )		///< State/type. Buzzer.
