//##########################################################################
//#                                                                        #
//#                            CLOUDCOMPARE                                #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU General Public License as published by  #
//#  the Free Software Foundation; version 2 of the License.               #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
//#  GNU General Public License for more details.                          #
//#                                                                        #
//#                    COPYRIGHT: CloudCompare project                     #
//#                                                                        #
//##########################################################################

#ifndef CC_PLATFORM_HEADER
#define CC_PLATFORM_HEADER

//Defines the following macros (depending on the compilation platform/settings)
//	- CC_WINDOWS / CC_MAC_OS / CC_LINUX
//	- CC_ENV32 / CC_ENV64
#if (defined(_WIN32) || defined(_WIN64) || defined(WIN32)) &&  !defined(ON_MXE)
	#define CC_WINDOWS
#if defined(_WIN64)
	#define CC_ENV_64
#else
	#define CC_ENV_32
#endif
#else
#if defined(__APPLE__)
	#define CC_MAC_OS
#else
	#define CC_LINUX
#endif
#if defined(__x86_64__) || defined(__ppc64__)
	#define CC_ENV_64
#else
	#define CC_ENV_32
#endif
#endif


#endif //CC_PLATFORM_HEADER
