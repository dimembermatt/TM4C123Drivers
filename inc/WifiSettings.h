// WifiSettings.h
// Dung Nguyen
// Wally Guzman

// Launchpad or TM4C123
#define LAUNCHPAD 		1
 
// Baudrate for UART connection to ESP8266
//#define BAUDRATE			74880    // The ESP first boots with 74880 baud and then switches
#define BAUDRATE 			115200

// Return values
#define NORESPONSE		(-1)
#define BADVALUE			(-1)
#define SUCCESS				1
#define FAILURE				0

enum Menu_Status {RX=0, TX, CONNECTED};

// Station or soft access point mode
// 0 means regular station, 1 means act as soft AP
#define SOFTAP        0

// Access Point Parameters
#define SSID_NAME  "YourAP"
#define PASSKEY    "YourPassword"

//#define SSID_NAME  "utexas-iot"
//#define PASSKEY    ""

/* To get the password for the "utexas-iot" network:

From your PC/laptop:

    Go to https://network.utexas.edu
    Log in with your UT EID (via UTLogin)
    Click the "Register Wireless Device" button
    Enter your device's MAC address and a description, and click the "Register" button
    Make note of the WPA2 key generated (will be unique for each device you register)

On your IoT device:

    Select utexas-iot from the available wireless networks
    Enter the WPA2 key provided to you in step 5 above

*/ 
