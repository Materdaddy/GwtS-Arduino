# Arduino library to control Disney Glow With The Show ears

* Uses pin 2
* Easy for beginners to use
* Simply hook up an IR LED (940nm-950nm) to pin 2 with a current limiting resistor, and write code!

Please refer to:

* [Original GWTS Arduino MEGA Code](https://github.com/rjchu/RenardToGWTSBridge)
* [Forked GWTS Arduino Uno Code](https://github.com/Materdaddy/RenardToGWTSBridge)
* [DoItYourselfChristmas Forum](http://doityourselfchristmas.com/forums/showthread.php?25142-Glow-with-*OUR*-Show-and-MSP430G2553-discussion)
* [LA Times article on the ears](http://articles.latimes.com/2012/jun/18/business/la-fi-mo-disney-ears-20120618)

This library disables interrupts while writing out to the ears to maintain proper timing.  The protocol
is spitting out 2400 baud modulated over at 38kHz.
