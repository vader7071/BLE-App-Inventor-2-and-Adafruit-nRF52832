/*********************************************************************
 based on the Adafruit bleuart.ino example

 Used in conjunction with the MIT App Inventor 2 application.

 This is the testing code.  The MIT App will send a number, and this
 will display that number on the serial monitor.

 Please note:  The MIT App sends 4 sets of data See comments below.
 This is because the MIT App is sending data in 32 bits (uint32_t)
 and I have this sketch coded to 8 bit (uint8_t).  You can change
 the following line:
    uint8_t ch[4] = {0,0,0,0};
 to:
    uint32_t ch;
 and now you will get all data in just one variable.

 This sketch and MIT app work on integers only.  It is possible to send
 characters and other data across the BLE pipeline, however for my purposes
 all I needed was a simple "numeric option", so I chose to use integers
 as my data being transmitted and received. 

 There may be some extra code in this sketch that is not needed, however
 I got the project functioning as this sketch is, and I do not want to 
 tempt fate.
*********************************************************************/
#include <bluefruit.h>

#define Array_Mode "YES"  //sets if you are using array mode or single variable mode.  YES or NO

// BLE Service
BLEDfu  bledfu;  // Over The Air Device Firmware Update service
BLEDis  bledis;  // Device Information Service
BLEUart bleuart; // uart over ble
BLEBas  blebas;  // BAttery Service

uint8_t ble_array_data[4] = {0,0,0,0};  // creates 4 storage locations for the data from the MIT App
// The MIT App sends 4 blocks of integers.  This separates the 4 blocks into usable sections.
// Each block gets it's own spot in the array so the sketch can access the data

uint32_t ble_single_data = 0; //creates one variable location to accept only one data storage location

void setup()
{
  Serial.begin(115200);
  while ( !Serial ) delay(10);   // for nrf52840 with native usb
  
  Serial.println("Bluefruit52 BLEUART Example");
  Serial.println("---------------------------\n");

  // Setup the BLE LED to be enabled on CONNECT
  // Note: This is actually the default behaviour, but provided
  // here in case you want to control this LED manually via PIN 19
  Bluefruit.autoConnLed(true);

  // Config the peripheral connection with maximum bandwidth 
  // more SRAM required by SoftDevice
  // Note: All config***() function must be called before begin()
  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);

  Bluefruit.begin();
  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values
  Bluefruit.setName("Bluefruit_nRF52");
 
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

  // To be consistent OTA DFU should be added first if it exists
  bledfu.begin();

  // Configure and Start Device Information Service
  bledis.setManufacturer("Adafruit Industries");
  bledis.setModel("Bluefruit Feather52");
  bledis.begin();

  // Configure and Start BLE Uart Service
  bleuart.begin();

  // Start BLE Battery Service
  blebas.begin();
  blebas.write(100);

  // Set up and start advertising
  startAdv();

  Serial.println("nRF52 is ready.  Please activate MIT App and connect");
  Serial.println("Data from MIT App will display here");
}

void startAdv(void)
{
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // Include bleuart 128-bit uuid
  Bluefruit.Advertising.addService(bleuart);

  // Secondary Scan Response packet (optional)
  // Since there is no room for 'Name' in Advertising packet
  Bluefruit.ScanResponse.addName();
  
  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html   
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds  
}

void loop()
{
  if (Array_Mode == "YES"){
    /* 
     *  Whatever number is sent from the MIT App will display here in this format:
     *  Value - Array Position
     *  {value} - 0
     *  {value} - 1
     *  {value} - 2
     *  {value} - 3
     *  end of line
     *  
     */
    while ( bleuart.available() )
    {
      Serial.println("Value - Array Position");
      for (int x = 0; x < 4; x++) {
        ble_array_data[x] = (uint8_t) bleuart.read();
        Serial.print(ble_array_data[x]);
        Serial.print(" - ");
        Serial.println(x);
        Serial.println("end of line");
        Serial.println(" ");
      }
    }
  /* 
   *  In the MIT app, to access the different array locations, you need to adjust the integer values
   *  on the bit level.  You will need to know how to count in binary, or convert binary to decimal, 
   *  to fully use all of the array locations.  If you are not familiar with binary counting and how 
   *  to work with bianry numbers, I *HIGHLY* suggest changing the line near the top from:
   *        uint8_t ch[4] = {0,0,0,0};
   *  to:
   *        uint32_t ch;
   *  and only use 1 variable location and just base your code on whatever number you send there.
   *  
   *  If you are familiar with binary, here is how the array works:
   *  
   *  The MIT app sends:
   *  [aaaaaaaa bbbbbbbb cccccccc dddddddd] (1 block of 32 bit binary)
   *  
   *  The nRF52 sees : (the arrays work in this manner)
   *  [aaaaaaaa] [bbbbbbbb] [cccccccc] [dddddddd]  (4 blocks of 8 bit binary)
   *   ch[3]       ch[2]     ch[1]       ch[0]
   *   
   *   The MIT app has the above conversion built in, at least for array spot 0 & 1.  Array spots 2 & 3 are not
   *   used in this example.
   *   
   *   To add array spot 2, copy the array spot 1 block, initialize array_data_spot_2, and change the 256 to 65536 (make sure to add all 3 values when sending)
   *   To add array spot 3, same as above, and change 256 to 16777216.
   */
  }

  if (Array_Mode == "NO"){
    /* 
     * The below code is setup to use only the one 32 bit integer
     * Comment out the ab 
     */
    while ( bleuart.available() )
    {
    Serial.println("Value - Array Position");
      ble_single_data = (uint32_t) bleuart.read();
      Serial.print(ble_single_data);
      Serial.println("end of line");
      Serial.println(" ");
    }
  }
}
  

// callback invoked when central connects
void connect_callback(uint16_t conn_handle)
{
  // Get the reference to current connection
  BLEConnection* connection = Bluefruit.Connection(conn_handle);

  char central_name[32] = { 0 };
  connection->getPeerName(central_name, sizeof(central_name));

  Serial.print("Connected to ");
  Serial.println(central_name);
}

/**
 * Callback invoked when a connection is dropped
 * @param conn_handle connection where this event happens
 * @param reason is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h
 */
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;

  Serial.println();
  Serial.println("Disconnected");
}
