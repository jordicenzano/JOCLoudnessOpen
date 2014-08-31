JOCLoudnessOpen
===============

Loudness computation software (R128 and A/85)

The JOCLoudness application is a windows console applications that loads a wave file (.wav) and give us the following data of the file:

- Integrated loudness
- Vector of short term loudness
- Vector of momentary loudness
- Vector and maximum true peak level 
- Loudness range value

Usage example:

> JOCLoudness.exe "c:\audiotest\totest.wav" /tin:1.1 /tout:20 /outaudiofile:"c:\out\outpartialfile.wav" /config:configcustom.ini /outlouddata:"c:\dataloud\loud.txt" /logsfile:"c:\logsloud.log"

More information on: http://jordicenzano.name/front-test/tv-loudness-metering-072013/
