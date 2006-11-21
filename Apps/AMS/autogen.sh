#!/bin/sh
##################
#
# File:		autogen.sh
# Purpose:	This script runs all of the commands necessary for configuring DSAM
#			from the CVS repository.
# Author:	L.P.O'Mard
# Created:	21 Nov 2006
# Updated:	
# Copyright:(c) 2006, Lowel P. O'Mard
#
##################

#aclocal
#autoheader
#autoconf
#automake --add-missing

autoreconf -fvis
