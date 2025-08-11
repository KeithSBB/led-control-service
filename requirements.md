# LED Control Service Requirements
## Problem statement
Wyoming-satellite is voice assistant software that runs on a raspberrypi under
 Linux OS.  The raspberrypi has one or more GPIO chips which can be used to
 control LEDs.   Wyoming-satellite has configuration options to execute
 commands when events take place.  

These are: 

--startup-command STARTUP_COMMAND
      Command run when the satellite starts

--detect-command DETECT_COMMAND
       Command to run when wake word detection starts

--detection-command DETECTION_COMMAND
       Command to run when wake word is detected

--transcript-command TRANSCRIPT_COMMAND
      Command to run when speech to text transcript is returned

--stt-start-command STT_START_COMMAND
      Command to run when the user starts speaking

--stt-stop-command STT_STOP_COMMAND
       Command to run when the user stops speaking

--synthesize-command SYNTHESIZE_COMMAND
      Command to run when text to speech text is returned

--tts-start-command TTS_START_COMMAND
      Command to run when text to speech response starts

--tts-stop-command TTS_STOP_COMMAND
      Command to run when text to speech response stops

--tts-played-command TTS_PLAYED_COMMAND
       Command to run when text-to-speech audio stopped playing

--streaming-start-command STREAMING_START_COMMAND
       Command to run when audio streaming starts

--streaming-stop-command STREAMING_STOP_COMMAND
      Command to run when audio streaming stops

--error-command ERROR_COMMAND
       Command to run when an error occurs

--connected-command CONNECTED_COMMAND
       Command to run when connected to the server

--disconnected-command DISCONNECTED_COMMAND
      Command to run when disconnected from the server

--timer-started-command TIMER_STARTED_COMMAND
      Command to run when a timer starts

--timer-updated-command TIMER_UPDATED_COMMAND
      Command to run when a timer is paused, resumed, or has time added or removed

--timer-cancelled-command TIMER_CANCELLED_COMMAND, --timer-canceled-command TIMER_CANCELLED_COMMAND
      Command to run when a timer is cancelled

--timer-finished-command TIMER_FINISHED_COMMAND
       Command to run when a timer finishes 

A service is needed that can be commanded from these wyoming-satellite event
options to control LEDs via the raspberrypi GPIO interface. Note that usually
only a subset of these options are usually used.

## Target Hardware and Operating system
A Raspberrypi 4B running  Fedora server 42 and a modifiedversion of
 Wyoming-satellite lacated here:  https://github.com/KeithSBB/wyoming-satellite-fedora42.git
## Test Hardware 
A Raspberrypi 4B running  Fedora server 42, the modified version of 
wyoming-satellite and the following gpiochip0 line 23 connected to a red LED,
 line 24 connected to a green LED and line 25 connected to a blue LED.  Cockpit
is the used as the main interface to the test hardware via a remote laptop
browser.

## Main Goal
Design and code a Fedora 42 server  service which controls LEDs on any GPIO
 chip lines for any gpiochip on a raspberrypi 4B.   The service provide an interface that
 other applications can use to control the leds.   

## Use Cases

1. turn on one or more  leds. (possible implemented with sequenc?) 
1. turn off one or more  leds. (possible implemented with sequenc?)  
1. turn on a nested sequence of  leds with a provided floating point dwell
time  in seconds  of how long to pause on each led before turning it off and 
 turning on the next.  
    1.  Each element in the sequence is either a single LED - dwell, no LEDs - Dwell,
or multiple LEDs and dwells that are started simultaneous as the next step in the
top most sequence. 
    1. The sequence also will support simulataneous LED as well as
 sequential LED operations.  
        * For example, one proposed command syntax might be:
"gpiochip1:red ON 1.2, (gpiochip0:fault ON 1.3, gpiochip1:blue ON 2),
 gpiochip1:yellow 3, OFF 4", loop forever
        - gpiochip1 red led lights for 1.2 secs, then both gpiochip0:fault
 and gpiochip1:blue turn on with gpiochip0:fault turning off after 1.3 secs
 and gpiochip1:blue turning off after 2 secs, then gpiochip1:yellow turns on
 for 3 secs, then LEDs off for 4 secs.   This sequence can be repeated until  another call terminates and starts something new
    1. A sequence can either:
        1. loop forever until terminated by another command
        1. loop for a given number of times, include just once (Loop 5)
4. Blink on and off one or more leds at a specified rate. (could be implemented with sequence)
1. The ability to interrrupt and terminate the previous command either directly
or by sending a new command.
5. Provide an /etc configuration ini file per GPIO chip with lines that assign names to each line

## Additional Requirements
* The project is named: **LED CONTROL SERVICE**
* The design is to be object oriented in C++
* The command syntax should be easy and simple to use.  It is accetable to
combined turn on/off, sequence and blink into a single command with syntax that
hanles all these use cases.
* The service is to designed as a github repository that anyone can download
 and install.  The github repository has already been created and is located
 at https://github.com/KeithSBB/led-control-service.git . The repository is
currently empty except for a MIT license file and the default README.md file
* This requirements.md file is to be included in the repository
* The README.md file is to acknowledge that Grok 4 was used to develop, write and
troubleshoot the code based on the requirements.md file.


