# ApolloUI
Arduino GUI interpter engine DS Apollo 1 Screen Module.
* Action String interpter
* UART interface
* UDP interface (WiFi client mode)
* UDP respond mode
* i2c controller for encoder and buttons
* i2c client for Apollo 1 Controller board
* WD2608 led controller


# HW Setup
* Apollo 1 Controller Board
  * uart? Data Over DC Module
* Apollo Screen Module
  * WT32-SC01 ESP32 based 480x320 RGB screen
  * i2c encoder
  * i2c button controller
  * i2c Arduino Nano (Arduino RDM Controller Software)
  * spi Lumenradio

[TODO: draw.io drawing]

# Action String
Idea of the action string is to have minimal effort basic scripting system running on WT32-SC01 with reasnoable draw delay and respnsive but runtime customizable UI. All efforts with Micropython based system were severly crippeled in supporting the screen module - mostly too slow but also limited colorspace options.

Idea is to store list of commands to the system in most simple form. Ie if we generate a new paramter to the lamp controller ie. fan speed, then the controller software can send some simple script strings over to customize how the Apollo Screen Module will show this paramter and how the screen is updated acoording to various user input or custom logic. 

Setup
* Create a parameter and set a value
* Draw paramter on screen
* Register event listener

Stored in the system
* Activate drawing
* Deactivate drawing
* Update parameter value

Private
* Update parameter on screen
* Broadcast string - will be sent to all outputs

String Description
* 1 letter special character
  * ? help string is printed for the function
  * / debug info, paramter values are printed
* 2 letters for function description
* 4 letter groups for numeric paramters, 1st letter is for description 999 is max value for the input
* 2 letter groups for multiple choice options, 1st letter is for description and 2nd letter for input value
* 1 char array parameter that will start after the predefined paramteres
