/*
 * Original work Copyright 2009 - 2010 Kevin Ackley (kackley@gwi.net)
 * Modified work Copyright 2018 - 2020 Andy Maloney <asmaloney@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "E57Exception.h"
#include "E57Version.h"

namespace e57
{
   /*!
   @class E57Exception
   @brief   Object thrown by E57 API functions to communicate the conditions of an
   error.
   @details
   The E57Exception object communicates information about errors occurring in calls
   to the E57 API functions. The error information is communicated from the
   location in the E57 implementation where the error was detected to the @c catch
   statement in the code of the API user. The state of E57Exception object has one
   mandatory field, the errorCode, and several optional fields that can be set
   depending on the debug level of the E57 implementation. There are three optional
   fields that encode the location in the source code of the E57 implementation
   where the error was detected: @c sourceFileName, @c sourceFunctionName, and @c
   sourceLineNumber. Another optional field is the @c context string that is human
   (or at least programmer) readable, which can capture some variable values that
   might be useful in debugging. The E57Exception class is derived from
   std::exception. So applications that only catch std::exceptions will detect
   E57Exceptions (but with no information about the origin of the error).

   Many other APIs use error codes (defined integer constants) returned from the
   API functions to communicate success or failure of the requested command. In
   contrast, the E57 API uses the C++ exception mechanism to communicate failure
   (success is communicated by the return of the function without exception).
   E57Exception(E57_SUCCESS) is never thrown. The API ErrorCode is packaged inside
   the E57Exception. The documentation for each function in the API declares which
   ErrorCode values (inside an E57Exception) can possibly be thrown by the
   function. Some API functions do not throw any E57Exceptions, and this is
   documented by the designation "No E57Exceptions." in the "Exceptions:" section
   of the function documentation page.

   If an API function does throw an E57Exception, the API user will rightfully be
   concerned about the state of all of the API objects. There are four categories
   of guarantee about the state of all objects that the API specifies.

   1) <b>All objects unchanged</b> - all API objects are left in their original
   state before the API function was called. This is the default guarantee, so if
   there is no notation next to the ErrorCode in the "Exceptions:" section of the
   function documentation page, the this category is implied.

   2) <b>XXX object modified, but consistent</b> - The given object (or objects)
   have been modified, but are left in a consistent state.

   3) <b>XXX object in undocumented state</b> - The given object (or objects) may
   have been left in an inconsistent state, and the only safe thing to do with them
   is to call their destructor.

   4) <b>All objects in undocumented state</b> - A very serious consistency error
   has been detected, and the state of all API objects is suspect.  The only safe
   thing to do is to call their destructors.

   Almost all of the API functions can throw the following two ErrorCodes:
   E57_ERROR_IMAGEFILE_NOT_OPEN and E57_ERROR_INTERNAL. In some E57
   implementations, the tree information may be stored on disk rather than in
   memory. If the disk file is closed, even the most basic information may not be
   available about nodes in the tree. So if the ImageFile is closed (by calling
   ImageFile::close), the API user must be ready for many of the API functions to
   throw E57Exception(E57_ERROR_IMAGEFILE_NOT_OPEN). Secondly, regarding the
   E57_ERROR_INTERNAL error, there is a lot of consistency checking in the
   Reference Implementation, and there may be much more added. Even if some API
   routines do not now throw E57_ERROR_INTERNAL, they could some time in the
   future, or in different implementations. So the right to throw
   E57_ERROR_INTERNAL is reserved for every API function (except those that by
   design can't throw E57Exceptions).

   It is strongly recommended that catch statements in user code that call API
   functions catch E57Exception by reference (i.e. <tt>catch (E57Exception&
   ex)</tt> and, if necessary, rethrow using the syntax that throws the currently
   active exception (i.e. <tt>throw;</tt>).

   Exceptions other that E57Exception may be thrown by calls to API functions (e.g.
   std::bad_alloc). Production code will likely have catch handlers for these
   exceptions as well.

   @see     HelloWorld.cpp example
   */

   //! @cond documentNonPublic   The following isn't part of the API, and isn't
   //! documented.
   E57Exception::E57Exception( ErrorCode ecode, const std::string &context, const std::string &srcFileName,
                               int srcLineNumber, const char *srcFunctionName ) :
      errorCode_( ecode ),
      context_( context ), sourceFunctionName_( srcFunctionName ), sourceLineNumber_( srcLineNumber )
   {
      sourceFileName_ = srcFileName.substr( srcFileName.find_last_of( "/\\" ) + 1 );
   }
   //! @endcond

   /*!
   @brief   Print error information on a given output stream.
   @param   [in] reportingFileName     Name of file where catch statement caught
   the exception. NULL if unknown.
   @param   [in] reportingLineNumber   Number of source code line where catch
   statement caught the exception.  0 if unknown.
   @param   [in] reportingFunctionName String name of function containing catch
   statement that caught the exception.  NULL if unknown.
   @param   [in] os Output string to print a summary of exception information and
   location of catch statement.
   @details
   The amount of information printed to output stream may depend on whether the E57
   implementation was built with debugging enabled.
   @post    No visible state is modified.
   @throw   No E57Exceptions.
   @see     E57ExceptionFunctions.cpp example, ErrorCode, HelloWorld.cpp example
   */
   void E57Exception::report( const char *reportingFileName, int reportingLineNumber, const char *reportingFunctionName,
                              std::ostream &os ) const
   {
      os << "**** Got an e57 exception: " << e57::Utilities::errorCodeToString( errorCode() ) << std::endl;

#ifdef E57_DEBUG
      os << "  Debug info: " << std::endl;
      os << "    context: " << context_ << std::endl;
      os << "    sourceFunctionName: " << sourceFunctionName_ << std::endl;
      if ( reportingFunctionName )
         os << "    reportingFunctionName: " << reportingFunctionName << std::endl;

      /*** Add a line in error message that a smart editor (gnu emacs) can
       * interpret as a link to the source code: */
      os << sourceFileName_ << "(" << sourceLineNumber_ << ") : error C" << errorCode_ << ":  <--- occurred on"
         << std::endl;
      if ( reportingFileName )
         os << reportingFileName << "(" << reportingLineNumber << ") : error C0:  <--- reported on" << std::endl;
#endif
   }

   /*!
   @brief   Get numeric ::ErrorCode associated with the exception.
   @post    No visible state is modified.
   @return  The numeric ::ErrorCode associated with the exception.
   @throw   No E57Exceptions.
   @see     E57ExceptionFunctions.cpp example, E57Utilities::errorCodeToString,
   ErrorCode
   */
   ErrorCode E57Exception::errorCode() const
   {
      return errorCode_;
   }

   /*!
   @brief   Get human-readable string that describes the context of the error.
   @details
   The context string may include values in object state, or function arguments.
   The format of the context string is not standardized.
   However, in the Reference Implementation, many strings contain a sequence of "
   VARNAME=VARVALUE" fields.
   @post    No visible state is modified.
   @return  The human-readable string that describes the context of the error.
   @throw   No E57Exceptions.
   @see     E57ExceptionsFunctions.cpp example
   */
   std::string E57Exception::context() const
   {
      return context_;
   }

   /*!
   @brief   Get string description of exception category.
   @details
   Returns "E57 Exception" for all E57Exceptions, no matter what the errorCode.
   @post    No visible state is modified.
   @return  The string description of exception category.
   @throw   No E57Exceptions.
   @see     E57ExceptionsFunctions.cpp example
   */
   const char *E57Exception::what() const noexcept
   {
      return "E57 exception";
   }

   /*!
   @brief   Get name of source file where exception occurred, for debugging.
   @details
   May return the value of the macro variable __FILE__ at the location where the
   E57Exception was constructed. May return the empty string ("") in some E57
   implementations.
   @post    No visible state is modified.
   @return  The name of source file where exception occurred, for debugging.
   @throw   No E57Exceptions.
   @see     E57ExceptionsFunctions.cpp example
   */
   const char *E57Exception::sourceFileName() const
   {
      return sourceFileName_.c_str();
   }

   /*!
   @brief   Get name of function in source code where the error occurred , for
   debugging.
   @details
   May return the value of the macro variable __FUNCTION__ at the location where
   the E57Exception was constructed. May return the empty string ("") in some E57
   implementations.
   @post    No visible state is modified.
   @return  The name of source code function where the error occurred , for
   debugging.
   @throw   No E57Exceptions.
   */
   const char *E57Exception::sourceFunctionName() const
   {
      return sourceFunctionName_;
   }

   /*!
   @brief   Get line number in source code file where exception occurred, for
   debugging.
   @details
   May return the value of the macro variable __LINE__ at the location where the
   E57Exception was constructed. May return the empty string ("") in some E57
   implementations.
   @post    No visible state is modified.
   @return  The line number in source code file where exception occurred, for
   debugging.
   @throw   No E57Exceptions.
   */
   int E57Exception::sourceLineNumber() const
   {
      return sourceLineNumber_;
   }

   //=====================================================================================

   /*!
   @brief   Get the version of ASTM E57 standard that the API implementation
   supports, and library id string.
   @param   [out] astmMajor    The major version number of the ASTM E57 standard
   supported.
   @param   [out] astmMinor    The minor version number of the ASTM E57 standard
   supported.
   @param   [out] libraryId    A string identifying the implementation of the API.
   @details
   Since the E57 implementation may be dynamically linked underneath the API, the
   version string for the implementation and the ASTM version that it supports
   can't be determined at compile-time. This function returns these identifiers
   from the underlying implementation.
   @throw   No E57Exceptions.
   */
   void Utilities::getVersions( int &astmMajor, int &astmMinor, std::string &libraryId )
   {
      /// REVISION_ID should be passed from compiler command line

#ifndef REVISION_ID
#error "Need to specify REVISION_ID on command line"
#endif

      astmMajor = E57_FORMAT_MAJOR;
      astmMinor = E57_FORMAT_MINOR;
      libraryId = REVISION_ID;
   }

   /*!
   @brief   Get short string description of an E57 ErrorCode.
   @param   [in] ecode     The numeric errorCode from an E57Exception.
   @details
   The errorCode is translated into a one-line English string.
   @return  English std::string describing error.
   @throw   No E57Exceptions.
   @see     E57Exception::errorCode
   */
   std::string Utilities::errorCodeToString( ErrorCode ecode )
   {
      switch ( ecode )
      {
         // N.B.  *** When changing error strings here, remember to update the
         // Doxygen strings in E57Exception.h ****
         case E57_SUCCESS:
            return "operation was successful (E57_SUCCESS)";
         case E57_ERROR_BAD_CV_HEADER:
            return "a CompressedVector binary header was bad "
                   "(E57_ERROR_BAD_CV_HEADER)";
         case E57_ERROR_BAD_CV_PACKET:
            return "a CompressedVector binary packet was bad "
                   "(E57_ERROR_BAD_CV_PACKET)";
         case E57_ERROR_CHILD_INDEX_OUT_OF_BOUNDS:
            return "a numerical index identifying a child was out of bounds "
                   "(E57_ERROR_CHILD_INDEX_OUT_OF_BOUNDS)";
         case E57_ERROR_SET_TWICE:
            return "attempted to set an existing child element to a new value "
                   "(E57_ERROR_SET_TWICE)";
         case E57_ERROR_HOMOGENEOUS_VIOLATION:
            return "attempted to add an E57 Element that would have made the "
                   "children of a "
                   "homogeneous Vector have different types "
                   "(E57_ERROR_HOMOGENEOUS_VIOLATION)";
         case E57_ERROR_VALUE_NOT_REPRESENTABLE:
            return "a value could not be represented in the requested type "
                   "(E57_ERROR_VALUE_NOT_REPRESENTABLE)";
         case E57_ERROR_SCALED_VALUE_NOT_REPRESENTABLE:
            return "after scaling the result could not be represented in the "
                   "requested type "
                   "(E57_ERROR_SCALED_VALUE_NOT_REPRESENTABLE)";
         case E57_ERROR_REAL64_TOO_LARGE:
            return "a 64 bit IEEE float was too large to store in a 32 bit IEEE "
                   "float "
                   "(E57_ERROR_REAL64_TOO_LARGE)";
         case E57_ERROR_EXPECTING_NUMERIC:
            return "Expecting numeric representation in user's buffer, found "
                   "ustring "
                   "(E57_ERROR_EXPECTING_NUMERIC)";
         case E57_ERROR_EXPECTING_USTRING:
            return "Expecting string representation in user's buffer, found "
                   "numeric "
                   "(E57_ERROR_EXPECTING_USTRING)";
         case E57_ERROR_INTERNAL:
            return "An unrecoverable inconsistent internal state was detected "
                   "(E57_ERROR_INTERNAL)";
         case E57_ERROR_BAD_XML_FORMAT:
            return "E57 primitive not encoded in XML correctly "
                   "(E57_ERROR_BAD_XML_FORMAT)";
         case E57_ERROR_XML_PARSER:
            return "XML not well formed (E57_ERROR_XML_PARSER)";
         case E57_ERROR_BAD_API_ARGUMENT:
            return "bad API function argument provided by user "
                   "(E57_ERROR_BAD_API_ARGUMENT)";
         case E57_ERROR_FILE_IS_READ_ONLY:
            return "can't modify read only file (E57_ERROR_FILE_IS_READ_ONLY)";
         case E57_ERROR_BAD_CHECKSUM:
            return "checksum mismatch, file is corrupted (E57_ERROR_BAD_CHECKSUM)";
         case E57_ERROR_OPEN_FAILED:
            return "open() failed (E57_ERROR_OPEN_FAILED)";
         case E57_ERROR_CLOSE_FAILED:
            return "close() failed (E57_ERROR_CLOSE_FAILED)";
         case E57_ERROR_READ_FAILED:
            return "read() failed (E57_ERROR_READ_FAILED)";
         case E57_ERROR_WRITE_FAILED:
            return "write() failed (E57_ERROR_WRITE_FAILED)";
         case E57_ERROR_LSEEK_FAILED:
            return "lseek() failed (E57_ERROR_LSEEK_FAILED)";
         case E57_ERROR_PATH_UNDEFINED:
            return "E57 element path well formed but not defined "
                   "(E57_ERROR_PATH_UNDEFINED)";
         case E57_ERROR_BAD_BUFFER:
            return "bad SourceDestBuffer (E57_ERROR_BAD_BUFFER)";
         case E57_ERROR_NO_BUFFER_FOR_ELEMENT:
            return "no buffer specified for an element in CompressedVectorNode "
                   "during write "
                   "(E57_ERROR_NO_BUFFER_FOR_ELEMENT)";
         case E57_ERROR_BUFFER_SIZE_MISMATCH:
            return "SourceDestBuffers not all same size "
                   "(E57_ERROR_BUFFER_SIZE_MISMATCH)";
         case E57_ERROR_BUFFER_DUPLICATE_PATHNAME:
            return "duplicate pathname in CompressedVectorNode read/write "
                   "(E57_ERROR_BUFFER_DUPLICATE_PATHNAME)";
         case E57_ERROR_BAD_FILE_SIGNATURE:
            return "file signature not "
                   "ASTM-E57"
                   " (E57_ERROR_BAD_FILE_SIGNATURE)";
         case E57_ERROR_UNKNOWN_FILE_VERSION:
            return "incompatible file version (E57_ERROR_UNKNOWN_FILE_VERSION)";
         case E57_ERROR_BAD_FILE_LENGTH:
            return "size in file header not same as actual "
                   "(E57_ERROR_BAD_FILE_LENGTH)";
         case E57_ERROR_XML_PARSER_INIT:
            return "XML parser failed to initialize (E57_ERROR_XML_PARSER_INIT)";
         case E57_ERROR_DUPLICATE_NAMESPACE_PREFIX:
            return "namespace prefix already defined "
                   "(E57_ERROR_DUPLICATE_NAMESPACE_PREFIX)";
         case E57_ERROR_DUPLICATE_NAMESPACE_URI:
            return "namespace URI already defined "
                   "(E57_ERROR_DUPLICATE_NAMESPACE_URI)";
         case E57_ERROR_BAD_PROTOTYPE:
            return "bad prototype in CompressedVectorNode "
                   "(E57_ERROR_BAD_PROTOTYPE)";
         case E57_ERROR_BAD_CODECS:
            return "bad codecs in CompressedVectorNode (E57_ERROR_BAD_CODECS)";
         case E57_ERROR_VALUE_OUT_OF_BOUNDS:
            return "element value out of min/max bounds "
                   "(E57_ERROR_VALUE_OUT_OF_BOUNDS)";
         case E57_ERROR_CONVERSION_REQUIRED:
            return "conversion required to assign element value, but not "
                   "requested "
                   "(E57_ERROR_CONVERSION_REQUIRED)";
         case E57_ERROR_BAD_PATH_NAME:
            return "E57 path name is not well formed (E57_ERROR_BAD_PATH_NAME)";
         case E57_ERROR_NOT_IMPLEMENTED:
            return "functionality not implemented (E57_ERROR_NOT_IMPLEMENTED)";
         case E57_ERROR_BAD_NODE_DOWNCAST:
            return "bad downcast from Node to specific node type "
                   "(E57_ERROR_BAD_NODE_DOWNCAST)";
         case E57_ERROR_WRITER_NOT_OPEN:
            return "CompressedVectorWriter is no longer open "
                   "(E57_ERROR_WRITER_NOT_OPEN)";
         case E57_ERROR_READER_NOT_OPEN:
            return "CompressedVectorReader is no longer open "
                   "(E57_ERROR_READER_NOT_OPEN)";
         case E57_ERROR_NODE_UNATTACHED:
            return "node is not yet attached to tree of ImageFile "
                   "(E57_ERROR_NODE_UNATTACHED)";
         case E57_ERROR_ALREADY_HAS_PARENT:
            return "node already has a parent (E57_ERROR_ALREADY_HAS_PARENT)";
         case E57_ERROR_DIFFERENT_DEST_IMAGEFILE:
            return "nodes were constructed with different destImageFiles "
                   "(E57_ERROR_DIFFERENT_DEST_IMAGEFILE)";
         case E57_ERROR_IMAGEFILE_NOT_OPEN:
            return "destImageFile is no longer open "
                   "(E57_ERROR_IMAGEFILE_NOT_OPEN)";
         case E57_ERROR_BUFFERS_NOT_COMPATIBLE:
            return "SourceDestBuffers not compatible with previously given ones "
                   "(E57_ERROR_BUFFERS_NOT_COMPATIBLE)";
         case E57_ERROR_TOO_MANY_WRITERS:
            return "too many open CompressedVectorWriters of an ImageFile "
                   "(E57_ERROR_TOO_MANY_WRITERS)";
         case E57_ERROR_TOO_MANY_READERS:
            return "too many open CompressedVectorReaders of an ImageFile "
                   "(E57_ERROR_TOO_MANY_READERS)";
         case E57_ERROR_BAD_CONFIGURATION:
            return "bad configuration string (E57_ERROR_BAD_CONFIGURATION)";
         case E57_ERROR_INVARIANCE_VIOLATION:
            return "class invariance constraint violation in debug mode "
                   "(E57_ERROR_INVARIANCE_VIOLATION)";
         default:
            return "unknown error (" + std::to_string( ecode ) + ")";
      }
   }

}
