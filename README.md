PANDAs
======

Arduino code for PANDAs (Portable and Affordable Nephelometric Data Acquisition)

OtterBox
--------

The first variant of PANDAs firmware, designed to run an Arduino Mini 5V. This firmware was used to generate results reported in our paper, [Field Calibrations of a Low-Cost Aerosol Sensor at a Regulatory Monitoring Site in California](http://www.atmos-meas-tech-discuss.net/7/605/2014/amtd-7-605-2014.html). The [Supplement (PDF, 14 MB)](http://www.atmos-meas-tech-discuss.net/7/605/2014/amtd-7-605-2014-supplement.pdf) to that paper shows a fully assembled OtterBox PANDA and lists the components you would need to build one.

EasyData
--------

This variant is designed to run on an [Arduino Leonardo](http://www.adafruit.com/products/849) stacked with an [Ultimate GPS Shield](http://www.adafruit.com/products/1272). It was created for a workshop called [Experiments Monitoring the Everyday](http://www.publiclab.org/wiki/experiments-monitoring-the-everyday-society-for-social-studies-of-science-workshop-san-diego). (The first picture on that page shows a table with four EasyData PANDAs.) By emulating a USB keyboard, the Leonardo can "type" data directly into a connected laptop. If the laptop is running Excel (or another spreadsheet program), data will be directly and visibly entered into the active worksheet. You can then save or graph it.

TripleScoop
-----------

The latest variant is designed to sample from three PPD42NS sensors in parallel by using [port manipulation](http://arduino.cc/en/Reference/PortManipulation). As of Spring 2014, it is running on 16 Arduino Unos (hence 48 PPD42NS sensors) distributed as part of the [BEACON network](http://beacon.berkeley.edu) in Oakland/Berkeley, CA. I will be publishing the results shortly as part of my dissertation in May 2014. Please hold your questions until then! ;-)
