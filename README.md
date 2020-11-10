# HomeSpan
*HomeKit for the Arduino-ESP32!*

Welcome to HomeSpan - a robust and extremely easy-to-use Arduino library for creating your own [ESP32-based](https://www.espressif.com/en/products/modules/esp32) HomeKit devices entirely within the [Arduino IDE](http://www.arduino.cc).

HomeSpan provides a microcontroller-focused implementation of [Apple's HomeKit Accessory Protocol Specification (HAP) Release R2](https://developer.apple.com/support/homekit-accessory-protocol/) designed specifically for the Espressif ESP32 microcontroller running within the Arduino IDE.  HomeSpan pairs directly to HomeKit via your home WiFi network without the need for any external bridges or components.  With HomeSpan you can use the full power of the ESP32's I/O functionality to create custom control software and/or hardware to automatically operate external devices from the Home App on your iPhone, iPad, or Mac, or with Siri.

### HomeSpan Highlights

* Provides a natural, intuitive, and **very** easy-to-use framework
* Utilizes a unique *Service-Centric* approach to creating HomeKit devices
* Takes full advantage of the widely-popular Arduino IDE
* 100% HAP R2 compliance
* 38 integrated [HomeKit Services](docs/ServiceList.md)
* Operates in either Accessory or Bridge mode

### For the HomeSpan Developer

* Extensive use of the Arduino Serial Monitor
  * Real-time, easy-to-understand diagnostics
  * Complete transparency to every underlying HomeKit action, data request, and data response
  * Command-line interface with a variety of info, debugging, and configuration commands
* Built-in database validation to ensure your configuration meets all HAP requirements
* Integrated PWM functionality supporting pulse-wave-modulation on any ESP32 pin
* Integrated Push Button functionality supporting single, double, and long presses 
* Integrated access to the ESP32's on-chip Remote Control peripheral for easy generation of IR and RF signals
* 16 detailed tutorial-examples with extensive comments, instructions, and HomeKit tips and tricks

### For the HomeSpan End-User

* Embedded WiFi Access Point and Web Interface to allow end-users (non-developers) to:
  * Setup Homespan with their own home WiFi Credentials
  * Create their own HomeKit Pairing Setup Code
* Status LED and Control Button to allow end-users to:
  * Unpair the device from HomeKit
  * Perform a Factory Reset
  * Launch the WiFi Access Point
* A 10-page [End-User Guide](docs/UserGuide.md)

# Latest Updates

#### 11/3/2020

> HomeSpan 1.0.0 is the initial public release. It is 100% feature-complete and includes 16 very detailed [tutorial examples](docs/Tutorials.md) providing step-by-step instructions on how to use HomeSpan to create your own HomeKit devices. A 10-page [User Guide](docs/UserGuide.md) provides addtional details on how to configure your finished HomeKit device for use on your WiFi network, and how to pair it to HomeKit.

# Getting Started

#### Step 1: Install the Arduino IDE

Since HomeSpan is a library designed for Arduino's Integrated Development Environment, the first step is to download and install the latest version of this IDE from the [Arduino Software page](https://www.arduino.cc/en/software).  If you have experience with programming Arduino sketches, you'll find Homespan's use of the standard Arduino setup() and loop() functions, as well as the ever-present *\*.ino* file, very familiar.  If you've never programmed an Arduino, you may want to begin by first reviewing [Arduino's Guide to the IDE](https://www.arduino.cc/en/Guide/Environment) (and maybe try out a few Arduino examples) before proceeding to program with HomeSpan.

#### Step 2: Install the Arduino-ESP32 Core

The Arduino IDE comes with built-in compilers and support for a variety of Arduino boards, such as the Arduino Uno and Arduino Nano.  However, the IDE does not natively support programming for the ESP32.  Fortunately, the IDE allows you to download and install *cores* for other micro-controllers, including the ESP32, so  they can be programming from within the Arduino environment.  See the [Espressif Arduino-ESP32 GitHub page](https://github.com/espressif/arduino-esp32#arduino-core-for-esp32-wifi-chip) for complete instructions on how to easily install the Arduino-ESP32 core into the Arduino IDE using the [Arduino IDE Board Manager](https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md).

#### Step 3: Install HomeSpan

HomeSpan is packaged as a standard Arduino IDE.  To install, first download the [latest version](https://github.com/HomeSpan/HomeSpan/releases/latest) of *Source Code (zip)* to your desktop. It does not matter if your operating system keeps the package in zip form, or automatically unzips it when saving to you desktop.  Next, from the top menu bar within the Arduino IDE, select *Sketch → Include Library → Add .ZIP Library...* and navigtate to the Desktop folder where you should see the HomeSpan package you just downloaded (either as a zip file, or a folder).  Select the package (don't open it) and click `Choose`.  This directs the Arduino IDE to copy the HomeSpan package into its library sub-folder.  HomeSpan is now ready for use as a standard Arduino library (you may delete the HomeSpan package from your desktop as it is no longer needed).

#### Step 4: Explore the HomeSpan Tutorials

The HomeSpan library includes 16 detailed and heavily annotated tutorial examples you can explore even if you don't yet have an ESP32 board.  From the top menu bar of the Arduino IDE, select *File → Examples → HomeSpan* and choose the first tutorial to get started. The examples start simple and grow in complexity, taking you through all the functions and features of HomeSpan.  Along the way you'll also learn a lot about HomeKit itself.  See [HomeSpan Tutorials](docs/Tutorials.md) for a summary of all the included examples.  Note that you can also compile the examples without having an ESP32 provided you select one of the ESP32 boards listed under *Tools → Board → ESP32 Arduino* (the compiler will throw an error message if you try to compile under a non-ESP32 board).

#### Step 5: Acquire an ESP32 Board

ESP32 development boards are widely available in different configurations from most electronics hobby sites, such as [Adafruit](https://www.adafruit.com) and [Sparkfun](https://www.sparkfun.com).  HomeSpan was developed using a dual-core ESP32-WROOM-32 (the [Huzzah32 from Adafruit](https://www.adafruit.com/product/3619)) but should work equally well on dual-core ESP32-WROVER chips.  HomeSpan has *not* been tested on any single-core ESP32 chips, such as the ESP32-S2.

#### Step 6: Install any required USB Drivers

Programming an ESP32 board is generally done serially via USB.  Most ESP32 development boards contain a specialized chip that converts the serial UART signals used by the ESP32 to USB signals that can be transmitted to and and from your computer, usually over a standard USB cable.  Depending on the specific USB-UART chip, your operating system may or may not need you to install a separate driver.  The manufacturer of your ESP32 board typically provides instructions on how to download and install whatever drivers may be required for the USB-UART chip they used on the board.

#### Step 7: Create your first Homespan Device

If you've completed all the steps above, you are ready to start programming your ESP32 with the HomeSpan library.

* Plug your ESP32 board into your computer
* Launch the Arduino IDE
* Load one of the HomeSpan example tutorial sketches (for example, *File → Examples → HomeSpan → 01-SimpleLightBulb*)
* Set the Board to match your ESP32 board (for example, *Tools → Board → ESP32 Arduino → Adafruit ESP32 Feather*)
* Compile and upload the HomeSpan sketch (*Sketch → Upload*)

Congratulations!  You've just created your first HomeSpan device.

# The HomeSpan Command-Line Interface (CLI)

HomeSpan devices are of course meant to run on a standalone basis, connected to some real-world appliance like a fan or window shade.  HomeSpan devices are meant to only be connected to a computer for initial programming using the Arduino IDE.  However, HomeSpan also implements a light-weight Command-Line Interface over its serial port that can be accessed via the Arduino Serial Monitor whenever the 




# Resources and Documentation

*in progress*

[End-User Guide](docs/UserGuide.md)

