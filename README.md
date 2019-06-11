# BLE-App-Inventor-2-and-Adafruit-nRF52832
The *.aia project for MIT App Inventor 2 and the arduino sketch for the Adafruit nRF52832 to communicate via Bluetooth Low Energy (BLE)

I started this repository to house my examples to use the MIT App Inventor 2 platform to send integer data to an Adafruit nRF52832 controller via Bluetooth Low Energy (BLE).

When I started, there was not a clear cut example of how to get the App Inv 2 to send BLE data to the nRF52.  So I created this base test code.  

# Critical
You MUST follow the instructions at https://learn.adafruit.com/bluefruit-nrf52-feather-learning-guide/arduino-bsp-setup on how to setup the Arduino IDE to program the Adafruit nRF52.  This is highly important.

In the App Inventor, you MUST install the BluetoothLE extension from http://appinventor.mit.edu/extensions/

# Things to note.
As of this version, this is one way only.  From the Android phone app to the nRF52.  Currently there is no data coming from the nRF52 to the phone.

As of this version, this is integer only.  Depending on the mode selected (array mode or single value mode), the data sent from the phone is an unsigned integer (either four sets of 0-255 or one number 4,294,967,295).

If Array Mode is selected, this example only sends data in array spot 0 and 1.  Array spots 2 and 3 will always be 0.
