
# Using Cunilog

This brief introduction assumes that you either add the two combined source code files
__cunilog_combined.h__ and __cunilog_combined.c__ in folder __src/c/combined__
to your project and compile them as part of it, or that you build libcunilog, link your application to it, and only add __cunilog_combined.h__ to your project.

Another option would be that you add all files separately but this is a lot more work.

Include __cunilog_combined.h__ to access Cunilog's interface.
All necessary constants, enumerations, functions, macros, and structures should be
readily accessible from C and C++. There's currently (January 2025) no bindings
for other languages.

## Strings

Unless mentioned otherwise, many functions and macros that accept string
arguments/parameters also expect a length parameter that tells the function/macro
how long the string is. The length is the length of a string excluding a
terminating NUL character as if obtained via a call to strlen (). For example,
"ABC" has a length of 3.

This length argument can be USE_STRLEN, in which case the function/macro calls
strlen () implicitely on the string. USE_STRLEN is defined as ((size_t) -1).

If the length of a string is provided instead of USE_STRLEN, the function/macro
will not read beyond this length. This means that most functions/macros can
be called with string buffers that are not NUL-terminated.

Cunilog treats all strings as UTF-8 strings without actually understanding the
concept of UTF-8. For instance, a UTF-8 codepoint consisting of 3 octets/bytes
is worked with as a string that consists of 3 characters. It is the caller's
responsibility to ensure UTF-8 passed on to Cunilog is correct UTF-8.

## Logging

We assume that you only intend to log/write to a single log file, from a
single process, but possibly from different threads. Cunilog supports writing
to different log files or writing to the same log file from different processes
but we won't cover those cases in this brief introduction.

In Cunilog terms a log file is called a __target__. In fact, a __target__
can be anything from console output to log file or both or a database or
whatever. This is entirely configurable. Whatever the __target__ is, all
logging functions expect a target.

An application can create an arbitrary number of targets for Cunilog, or just
use the default (static) __target__. Every __target__, however, needs to be
configured first so that Cunilog knows what to do with it. This also applies to
the default (static) __target__.

Each definition, function, structure, constant, and enumeration should be
self-explanatory in the header file(s). The comments above or next to the
definition, function, structure, constant, or enumeration should contain enough
information on their purpose and usage. If the description isn't detailed enough,
then the function or macro is most likely a variation of one of the functions or
macros before/above, the comments only contain differences to the previous
function or macro, and necessary information is found there. If this is not
the case, though, this is considered a bug.

All logging functions expect as their first parameter a pointer to a __target__
structure of type __SCUNILOGTARGET__. For the default (static) __target__,
macros are provided that end in _static. These macros set the __target__ to an
internal (static) __SCUNILOGTARGET__ structure, which is shared by all these
macros/functions.

Example for using the internal static structure:
```C
// Application start

SCUNILOGTARGET *put = InitSUNILOGTARGETstatic	(
			"logs", 4,
			"MyApp", USE_STRLEN,
			cunilogLogPath_relativeToHomeDir,
			cunilogMultiThreadedSeparateLoggingThread
						);
if (NULL == put)
{
	// InitSUNILOGTARGETstatic () failed.
	exit (EXIT_FAILURE);
}

// Use the log...static () family of functions and macros for logging.
logTextU8_static ("A simple line to go in the logfile.");
logTextU8l_static ("Another line.", 13);
logTextU8l_static ("And another one.", USE_STRLEN);

// Just before the application exists shut down logging and deallocate its resources.
ShutdownSCUNILOGTARGETstatic ();		// Blocks until the queue is empty.
DoneSUNILOGTARGETstatic ();
```

Here's the signature of the InitSUNILOGTARGETstatic () function:
```C
/*
	InitSCUNILOGTARGETstatic

	Simplified version of InitSCUNILOGTARGETstaticEx ().

	szLogPath	The path where the log files are written to. This can either be an
			absolute or a relative path. If the path is relative, it is assumed to
			be relative to the executable module. If this parameter is NULL, the
			function uses the executable module's path. This string does not have to
			be NUL-terminated if lenLogPath is given correctly and not as (size_t)
			-1 (or USE_STRLEN).

	lenLogPath	The length of szLogPath. If this parameter is (size_t) -1, the function
			calls strlen () to obtain it. Conveniently USE_STRLEN is defined as
			(size_t) -1. If this parameter is 0, the function uses the executable
			module's path.

	szAppName	The name of the application. This is the part of the log file's name
			before the date/timestamp, for instance "myApp" in "myApp_2022-10-18.log".
			If this parameter is NULL, the function uses the executable module's name.
			The string does not need to be NUL-terminated if lenAppName holds the
			correct length.

	lenAppName	The length of szApplication. If this parameter is (size_t) -1, the
			function calls strlen (szApplication) to obtain it. USE_STRLEN is defined
			as (size_t) -1.
			If this parameter is 0, the function uses the executable module's name.

	relLogPath	One of the values in the enCunilogRelLogPath enumeration that specify
			the base path if szLogPath is either relative or NULL. If szLogPath is
			relative, the path is relative to
			cunilogLogPath_relativeToExecutable (the executable file),
			cunilogLogPath_relativeToCurrentDir (the current directory), or
			cunilogLogPath_relativeToHomeDir (the user's home directory).
			See cunilogstructs.h for details.
			The value of this parameter is ignored if szLogPath is an absolute
			path. If this value is cunilogLogPath_isAbsolute and szLogPath is a
			relative path or NULL, the function fails.

	type		The type of the SUNILOGTARGET. See cunilogstructs.h for more details.
			If CUNILOG_BUILD_SINGLE_THREADED_ONLY is defined, this parameter is
			ignored and implicitely set to cunilogSingleThreaded.

	The function returns a pointer to the internal SCUNILOGTARGET cunilognewlinestructure
	upon success, NULL otherwise.

	Call DoneSCUNILOGTARGETstatic () to free the structure's resources.
*/
SCUNILOGTARGET *InitSCUNILOGTARGETstatic
(
	  const char		*szLogPath		// Path to the logging information.
	, size_t		lenLogPath		// Length of szLogPath
	, const char		*szApplication		// Application name.
	, size_t		lenApplication		// Length of szApplication.
	, enCunilogRelLogPath	relLogPath		// Rel. to home, exe, or current dir.
	, enum cunilogtype	type
)
;
```
