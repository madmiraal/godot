/*************************************************************************/
/*  errors_unix.cpp                                                      */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2022 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2022 Godot Engine contributors (cf. AUTHORS.md).   */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#if defined(UNIX_ENABLED) || defined(LIBC_FILEIO_ENABLED)

#include "errors_unix.h"

#include <errno.h>

Error godot_error(int posix_errno) {
	switch (posix_errno) {
		case E2BIG: // Argument list too long.
			return ERR_PARAMETER_RANGE_ERROR; // Parameter range error
		case EACCES: // Permission denied.
			return ERR_FILE_NO_PERMISSION; // File: No permission error
		case EADDRINUSE: // Address in use.
			return ERR_ALREADY_IN_USE; // Already in use error
		case EADDRNOTAVAIL: // Address not available.
			return ERR_UNAVAILABLE; // Unavailable error
		case EAFNOSUPPORT: // Address family not supported.
			return ERR_UNAVAILABLE; // Unavailable error
		case EAGAIN: // Resource unavailable, try again (may be the same value as EWOULDBLOCK).
			return ERR_CANT_ACQUIRE_RESOURCE; // Can't acquire resource error
		case EALREADY: // Connection already in progress.
			return ERR_BUSY; // Busy error
		case EBADF: // Bad file descriptor.
			return ERR_FILE_UNRECOGNIZED; // File: Unrecognized error
		case EBADMSG: // Bad message.
			return ERR_PARSE_ERROR; // Parse error
		case EBUSY: // Device or resource busy.
			return ERR_BUSY; // Busy error
		case ECANCELED: // Operation canceled.
			return ERR_UNAVAILABLE; // Unavailable error
		case ECHILD: // No child processes.
			return ERR_CANT_FORK; // Can't fork process error
		case ECONNABORTED: // Connection aborted.
			return ERR_CONNECTION_ERROR; // Connection error
		case ECONNREFUSED: // Connection refused.
			return ERR_CANT_CONNECT; // Can't connect error
		case ECONNRESET: // Connection reset.
			return ERR_CONNECTION_ERROR; // Connection error
		case EDEADLK: // Resource deadlock would occur.
			return ERR_UNAVAILABLE; // Unavailable error
		case EDESTADDRREQ: // Destination address required.
			return ERR_FILE_BAD_PATH; // File: Bad path error
		case EDOM: // Mathematics argument out of domain of function.
			return ERR_PARAMETER_RANGE_ERROR; // Parameter range error
		case EDQUOT: // Reserved.
			return ERR_BUG; // Bug error
		case EEXIST: // File exists.
			return ERR_ALREADY_EXISTS; // Already exists error
		case EFAULT: // Bad address.
			return ERR_FILE_BAD_PATH; // File: Bad path error
		case EFBIG: // File too large.
			return ERR_OUT_OF_MEMORY; // Out of memory (OOM) error
		case EHOSTUNREACH: // Host is unreachable.
			return ERR_CANT_CONNECT; // Can't connect error
		case EIDRM: // Identifier removed.
			return ERR_UNAVAILABLE; // Unavailable error
		case EILSEQ: // Illegal byte sequence.
			return ERR_INVALID_DATA; // Invalid data error
		case EINPROGRESS: // Operation in progress.
			return ERR_ALREADY_IN_USE; // Already in use error
		case EINTR: // Interrupted function.
			return ERR_UNAVAILABLE; // Unavailable error
		case EINVAL: // Invalid argument.
			return ERR_INVALID_PARAMETER; // Invalid parameter error
		case EIO: // I/O error.
			return ERR_CONNECTION_ERROR; // Connection error
		case EISCONN: // Socket is connected.
			return ERR_ALREADY_IN_USE; // Already in use error
		case EISDIR: // Is a directory.
			return ERR_FILE_NOT_FOUND; // File: Not found error
		case ELOOP: // Too many levels of symbolic links.
			return ERR_CYCLIC_LINK; // Cycling link (import cycle) error
		case EMFILE: // File descriptor value too large.
			return ERR_PARAMETER_RANGE_ERROR; // Parameter range error
		case EMLINK: // Too many links.
			return ERR_LINK_FAILED; // Linking failed error
		case EMSGSIZE: // Message too large.
			return ERR_PARSE_ERROR; // Parse error
		case EMULTIHOP: // Reserved.
			return ERR_BUG; // Bug error
		case ENAMETOOLONG: // Filename too long.
			return ERR_FILE_BAD_PATH; // File: Bad path error
		case ENETDOWN: // Network is down.
			return ERR_CANT_CONNECT; // Can't connect error
		case ENETRESET: // Connection aborted by network.
			return ERR_CONNECTION_ERROR; // Connection error
		case ENETUNREACH: // Network unreachable.
			return ERR_CANT_CONNECT; // Can't connect error
		case ENFILE: // Too many files open in system.
			return ERR_OUT_OF_MEMORY; // Out of memory (OOM) error
		case ENOBUFS: // No buffer space available.
			return ERR_OUT_OF_MEMORY; // Out of memory (OOM) error
		case ENODATA: // OB XSR Option Start No message is available on the STREAM head read queue. Option End
			return ERR_BUSY; // Busy error
		case ENODEV: // No such device.
			return ERR_FILE_BAD_DRIVE; // File: Bad drive error
		case ENOENT: // No such file or directory.
			return ERR_FILE_NOT_FOUND; // File: Not found error
		case ENOEXEC: // Executable file format error.
			return ERR_FILE_CORRUPT; // File: Corrupt error
		case ENOLCK: // No locks available.
			return ERR_BUSY; // Busy error
		case ENOLINK: // Reserved.
			return ERR_BUG; // Bug error
		case ENOMEM: // Not enough space.
			return ERR_OUT_OF_MEMORY; // Out of memory (OOM) error
		case ENOMSG: // No message of the desired type.
			return ERR_DOES_NOT_EXIST; // Does not exist error
		case ENOPROTOOPT: // Protocol not available.
			return ERR_DOES_NOT_EXIST; // Does not exist error
		case ENOSPC: // No space left on device.
			return ERR_OUT_OF_MEMORY; // Out of memory (OOM) error
		case ENOSR: // OB XSR Option Start No STREAM resources. Option End
			return ERR_OUT_OF_MEMORY; // Out of memory (OOM) error
		case ENOSTR: // OB XSR Option Start Not a STREAM. Option End
			return ERR_CONNECTION_ERROR; // Connection error
		case ENOSYS: // Functionality not supported.
			return ERR_UNAVAILABLE; // Unavailable error
		case ENOTCONN: // The socket is not connected.
			return ERR_CONNECTION_ERROR; // Connection error
		case ENOTDIR: // Not a directory or a symbolic link to a directory.
			return ERR_FILE_BAD_PATH; // File: Bad path error
		case ENOTEMPTY: // Directory not empty.
			return ERR_FILE_ALREADY_IN_USE; // File: Already in use error
		case ENOTRECOVERABLE: // State not recoverable.
			return ERR_BUG; // Bug error
		case ENOTSOCK: // Not a socket.
			return ERR_CANT_CONNECT; // Can't connect error
		case ENOTSUP: // Not supported (may be the same value as EOPNOTSUPP).
			return ERR_UNAVAILABLE; // Unavailable error
		case ENOTTY: // Inappropriate I/O control operation.
			return ERR_METHOD_NOT_FOUND; // Method not found error
		case ENXIO: // No such device or address.
			return ERR_CANT_CONNECT; // Can't connect error
		//case EOPNOTSUPP:	// Operation not supported on socket Note: On some platforms this can be the same as ENOTSUP.
		//	return	ERR_UNAVAILABLE;	// Unavailable error
		case EOVERFLOW: // Value too large to be stored in data type.
			return ERR_PARAMETER_RANGE_ERROR; // Parameter range error
		case EOWNERDEAD: // Previous owner died.
			return ERR_UNAVAILABLE; // Unavailable error
		case EPERM: // Operation not permitted.
			return ERR_UNAUTHORIZED; // Unauthorized error
		case EPIPE: // Broken pipe.
			return ERR_CONNECTION_ERROR; // Connection error
		case EPROTO: // Protocol error.
			return ERR_PARAMETER_RANGE_ERROR; // Parameter range error
		case EPROTONOSUPPORT: // Protocol not supported.
			return ERR_UNAVAILABLE; // Unavailable error
		case EPROTOTYPE: // Protocol wrong type for socket.
			return ERR_INVALID_PARAMETER; // Invalid parameter error
		case ERANGE: // Result too large.
			return ERR_OUT_OF_MEMORY; // Out of memory (OOM) error
		case EROFS: // Read-only file system.
			return ERR_FILE_CANT_WRITE; // File: Can't write error
		case ESPIPE: // Invalid seek.
			return ERR_PARAMETER_RANGE_ERROR; // Parameter range error
		case ESRCH: // No such process.
			return ERR_UNAVAILABLE; // Unavailable error
		case ESTALE: // Reserved.
			return ERR_BUG; // Bug error
		case ETIME: // OB XSR Option Start Stream ioctl() timeout. Option End
			return ERR_TIMEOUT; // Timeout error
		case ETIMEDOUT: // Connection timed out.
			return ERR_TIMEOUT; // Timeout error
		case ETXTBSY: // Text file busy.
			return ERR_BUSY; // Busy error
		//case EWOULDBLOCK:	// Operation would block Note: On some platforms this can be the same as EAGAIN.
		//	return	ERR_CANT_ACQUIRE_RESOURCE;	// Can't acquire resource error
		case EXDEV: // Cross-device link.
			return ERR_LINK_FAILED; // Linking failed error
		default:
			return FAILED;
	}
}

#endif // UNIX ENABLED
