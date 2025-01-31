/****************************************************************************************

	File:		testcunilog.c
	Why:		Test application for cunilog.
	OS:			C99.
	Author:		Thomas
	Created:	2024-07-09

History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2024-07-09	Thomas			Created.

****************************************************************************************/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "./testcunilog.h"
#include "./../cunilog/cunilog.h"

// For testing.
#include "./../datetime/ISO__DATE__.h"
#include "./../datetime/ubf_date_and_time.h"
#include "./../dbg/ubfdebug.h"
#include "./../string/strfilesys.h"
#include "./../string/strhexdump.h"
#include "./../string/strintuint.h"
#include "./../string/strnewline.h"
#include "./../string/strwildcards.h"
#include "./../cunilog/cunilogcfgparser.h"
#include "./../string/strcustomfmt.h"
#include "./../OS/POSIX/PsxHome.h"

#ifndef _WIN32
	#include <unistd.h>
	
	#ifndef Sleep
	#define Sleep(s)									\
		sleep ((s) / 1000)
	#endif
#endif

const char	ccLogsFolder []	= STR_LOGS_FOLDER;
size_t		lnLogsFolder	= sizeof (ccLogsFolder) - 1;

int main (int argc, char *argv [])
{
	UNREFERENCED_PARAMETER (argc);
	UNREFERENCED_PARAMETER (argv);

	/*
		Tests.
	*/

	TestCunilogCfgParser ();
	test_strnewline ();
	test_strhexdump ();
	Test_ubf_times_functions ();
	Test_strintuint ();
	strwildcards_test_function ();
	ubf_test_ubf_strfilesys ();
	const char *szBld = szBuild_ISO__DATE__ ();
	ubf_assert (LEN_ISO8601DATE == strlen (szBld));
	szBld = szBuild_ISO__DATE__TIME__ ();
	ubf_assert (LEN_ISO8601DATETIMESTAMP_NO_OFFS == strlen (szBld));
	TestStrCustomFmt ();
	TestPsxHome ();

	/*
		End of tests.
	*/

	//ubf_assert (false);

	bool			b		= test_cunilog ();
	ubf_assert_true (b);

	SCUNILOGTARGET	*put;
	put = InitSCUNILOGTARGETstatic	(
				ccLogsFolder, lnLogsFolder,
				NULL, 0,
				cunilogLogPath_relativeToExecutable,
				cunilogMultiThreadedSeparateLoggingThread,
				cunilogPostfixMinute,
				NULL, 0,
				cunilogEvtTS_Default,
				cunilogNewLineDefault,
				cunilogRunProcessorsOnStartup
									);
	UNREFERENCED_PARAMETER (put);
	//b &= SetLogPrioritySCUNILOGTARGET (put, cunilogPrioIdle);
	ubf_assert_true (b);

	logTextU8_static ("");
	logTextU8_static ("----------");
	logTextU8_static (STR_HELLO_FROM_EXE);
	logTextU8_static ("A simple line to go in the logfile.");
	char *cData = "ABCDEFGasdfuiolnaoelasdfuaelhaerodclleahcoelandielacsserooot";
	logHexDumpU8sevl_static (cunilogEvtSeverityWarning, "Caption", USE_STRLEN, cData, 15);
	logHexDumpU8sevl_static (cunilogEvtSeverityNone, "Caption", USE_STRLEN, cData, 4);
	logHexDumpU8sevl_static (cunilogEvtSeverityWarning, "Caption", USE_STRLEN, cData, 4);
	logHexDumpU8sevl_static (cunilogEvtSeverityWarning, "Caption", USE_STRLEN, cData, 15);
	logHexDumpU8sevl_static (cunilogEvtSeverityWarning, "Caption", USE_STRLEN, cData, 16);
	logHexDumpU8sevl_static (cunilogEvtSeverityWarning, "Caption", USE_STRLEN, cData, 17);
	logHexDumpU8sevl_static (cunilogEvtSeverityWarning, "Caption", USE_STRLEN, cData, strlen (cData));
	uint64_t	n = 1000;
	while (n --)
	{
		logTextU8_static ("Another simple line to go in the logfile (first batch).");
	}
	PauseLogSCUNILOGTARGETstatic ();
	puts ("Sleeping (1)...");
	Sleep (2000);
	n = 1000;
	while (n --)
	{
		logTextU8_static ("Another simple line to go in the logfile (second batch).");
	}
	puts ("Sleeping (2)...");
	Sleep (2000);
	ResumeLogSCUNILOGTARGETstatic ();


	//CancelSCUNILOGTARGETstatic ();
	ShutdownSCUNILOGTARGETstatic ();
	DoneSCUNILOGTARGETstatic ();

	return b ? EXIT_SUCCESS : EXIT_FAILURE;	
}
