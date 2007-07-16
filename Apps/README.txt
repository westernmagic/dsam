General Information
===================

This is feval version 0.2.4.  feval allows users to
design, run and evaluate basilar membrane models.  It runs a suite of
tests, each of which can be enabled or disabled the respective test
paradigm parameters can be changed.

It is backed up by the extensive collection of published and new models
supplied by the development system for auditory modelling (DSAM), formerly
known as LUTEar.

There are two executable files, which are fully operational,
however the help documentation is not yet complete.

The AMS executable will run the application version with graphics
support. The AMS_NG application is the console version.

The main parameter file for both executables is the 'AMS.par' ASCII
text file, though AMS will work without this file. The driving DSAM
simulation specification file is specified in the 'AMS.par' file,
among other parameters.

To use this application on UNIX the DSAM library must be installed.

The official ftp site is: ftp://ftp.essex.ac.uk/pub/omard/dsam

The official web site is:

	http://www.essex.ac.uk/psychology/hearinglab/dsam

A mailing list is located at: dsam@mailbase.ac.uk

To subscribe: visit the web site at:

	http://www.mailbase.ac.uk/lists/dsam/

Installation
============

See the file 'INSTALL'

How to report bugs
==================

To report a bug, send mail either to the dsam list, as mentioned
above.

In the mail include:

* The version of feval

* Information about your system. For instance:

   - What operating system and version - What version of X - For
   Linux, what version of the C library

  And anything else you think is relevant.

* How to reproduce the bug. 

* If the bug was a crash, the exact text that was printed out when
  the crash occured.

* Further information such as stack traces may be useful, but is
  not necessary.

