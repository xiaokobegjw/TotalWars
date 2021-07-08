#include "string_utils.h"
#include "log.h"
#include "hex.h"
#include "macros.h"

#include <algorithm>
#include <array>
#include <sstream>
#include <iomanip>
#include <map>

#ifndef _WIN32
	#include <iconv.h>
#else
	#define _WIN32_WINNT 0x0501
	#include <windows.h>
#endif

#if defined(_ICONV_H_) && (defined(__FreeBSD__) || defined(__NetBSD__) || \
	defined(__OpenBSD__) || defined(__DragonFly__))
	#define BSD_ICONV_USED
#endif

#ifndef _WIN32

static bool convert(const char *to, const char *from, char *outbuf,
		size_t *outbuf_size, char *inbuf, size_t inbuf_size)
{
	iconv_t cd = iconv_open(to, from);

#ifdef BSD_ICONV_USED
	const char *inbuf_ptr = inbuf;
#else
	char *inbuf_ptr = inbuf;
#endif
	char *outbuf_ptr = outbuf;

	size_t *inbuf_left_ptr = &inbuf_size;

	const size_t old_outbuf_size = *outbuf_size;
	size_t old_size = inbuf_size;
	while (inbuf_size > 0) {
		iconv(cd, &inbuf_ptr, inbuf_left_ptr, &outbuf_ptr, outbuf_size);
		if (inbuf_size == old_size) {
			iconv_close(cd);
			return false;
		}
		old_size = inbuf_size;
	}

	iconv_close(cd);
	*outbuf_size = old_outbuf_size - *outbuf_size;
	return true;
}

#ifdef __ANDROID__
// On Android iconv disagrees how big a wchar_t is for whatever reason
const char *DEFAULT_ENCODING = "UTF-32LE";
#else
const char *DEFAULT_ENCODING = "WCHAR_T";
#endif

std::wstring utf8_to_wide(const std::string &input)
{
	const size_t inbuf_size = input.length();
	// maximum possible size, every character is sizeof(wchar_t) bytes
	size_t outbuf_size = input.length() * sizeof(wchar_t);

	char *inbuf = new char[inbuf_size]; // intentionally NOT null-terminated
	memcpy(inbuf, input.c_str(), inbuf_size);
	std::wstring out;
	out.resize(outbuf_size / sizeof(wchar_t));

#ifdef __ANDROID__
	SANITY_CHECK(sizeof(wchar_t) == 4);
#endif

	char *outbuf = reinterpret_cast<char*>(&out[0]);
	if (!convert(DEFAULT_ENCODING, "UTF-8", outbuf, &outbuf_size, inbuf, inbuf_size)) {
		infostream << "Couldn't convert UTF-8 string 0x" << hex_encode(input)
			<< " into wstring" << std::endl;
		delete[] inbuf;
		return L"<invalid UTF-8 string>";
	}
	delete[] inbuf;

	out.resize(outbuf_size / sizeof(wchar_t));
	return out;
}

std::string wide_to_utf8(const std::wstring &input)
{
	const size_t inbuf_size = input.length() * sizeof(wchar_t);
	// maximum possible size: utf-8 encodes codepoints using 1 up to 4 bytes
	size_t outbuf_size = input.length() * 4;

	char *inbuf = new char[inbuf_size]; // intentionally NOT null-terminated
	memcpy(inbuf, input.c_str(), inbuf_size);
	std::string out;
	out.resize(outbuf_size);

	if (!convert("UTF-8", DEFAULT_ENCODING, &out[0], &outbuf_size, inbuf, inbuf_size)) {
		infostream << "Couldn't convert wstring 0x" << hex_encode(inbuf, inbuf_size)
			<< " into UTF-8 string" << std::endl;
		delete[] inbuf;
		return "<invalid wide string>";
	}
	delete[] inbuf;

	out.resize(outbuf_size);
	return out;
}

#else // _WIN32

std::wstring utf8_to_wide(const std::string &input)
{
	size_t outbuf_size = input.size() + 1;
	wchar_t *outbuf = new wchar_t[outbuf_size];
	memset(outbuf, 0, outbuf_size * sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, input.c_str(), input.size(),
		outbuf, outbuf_size);
	std::wstring out(outbuf);
	delete[] outbuf;
	return out;
}

std::string wide_to_utf8(const std::wstring &input)
{
	size_t outbuf_size = (input.size() + 1) * 6;
	char *outbuf = new char[outbuf_size];
	memset(outbuf, 0, outbuf_size);
	WideCharToMultiByte(CP_UTF8, 0, input.c_str(), input.size(),
		outbuf, outbuf_size, NULL, NULL);
	std::string out(outbuf);
	delete[] outbuf;
	return out;
}

#endif // _WIN32

wchar_t *utf8_to_wide_c(const char *str)
{
	std::wstring ret = utf8_to_wide(std::string(str));
	size_t len = ret.length();
	wchar_t *ret_c = new wchar_t[len + 1];
	memcpy(ret_c, ret.c_str(), (len + 1) * sizeof(wchar_t));
	return ret_c;
}


std::string urlencode(const std::string &str)
{
	// Encodes non-unreserved URI characters by a percent sign
	// followed by two hex digits. See RFC 3986, section 2.3.
	static const char url_hex_chars[] = "0123456789ABCDEF";
	std::ostringstream oss(std::ios::binary);
	for (unsigned char c : str) {
		if (isalnum(c) || c == '-' || c == '.' || c == '_' || c == '~') {
			oss << c;
		} else {
			oss << "%"
				<< url_hex_chars[(c & 0xf0) >> 4]
				<< url_hex_chars[c & 0x0f];
		}
	}
	return oss.str();
}

std::string urldecode(const std::string &str)
{
	// Inverse of urlencode
	std::ostringstream oss(std::ios::binary);
	for (uint32_t i = 0; i < str.size(); i++) {
		unsigned char highvalue, lowvalue;
		if (str[i] == '%' &&
				hex_digit_decode(str[i+1], highvalue) &&
				hex_digit_decode(str[i+2], lowvalue)) {
			oss << (char) ((highvalue << 4) | lowvalue);
			i += 2;
		} else {
			oss << str[i];
		}
	}
	return oss.str();
}

uint32_t readFlagString(std::string str, const FlagDesc *flagdesc, uint32_t *flagmask)
{
	uint32_t result = 0;
	uint32_t mask = 0;
	char *s = &str[0];
	char *flagstr;
	char *strpos = nullptr;

	while ((flagstr = strtok_r(s, ",", &strpos))) {
		s = nullptr;

		while (*flagstr == ' ' || *flagstr == '\t')
			flagstr++;

		bool flagset = true;
		if (!strncasecmp(flagstr, "no", 2)) {
			flagset = false;
			flagstr += 2;
		}

		for (int i = 0; flagdesc[i].name; i++) {
			if (!strcasecmp(flagstr, flagdesc[i].name)) {
				mask |= flagdesc[i].flag;
				if (flagset)
					result |= flagdesc[i].flag;
				break;
			}
		}
	}

	if (flagmask)
		*flagmask = mask;

	return result;
}

std::string writeFlagString(uint32_t flags, const FlagDesc *flagdesc, uint32_t flagmask)
{
	std::string result;

	for (int i = 0; flagdesc[i].name; i++) {
		if (flagmask & flagdesc[i].flag) {
			if (!(flags & flagdesc[i].flag))
				result += "no";

			result += flagdesc[i].name;
			result += ", ";
		}
	}

	size_t len = result.length();
	if (len >= 2)
		result.erase(len - 2, 2);

	return result;
}

size_t mystrlcpy(char *dst, const char *src, size_t size)
{
	size_t srclen  = strlen(src) + 1;
	size_t copylen = MYMIN(srclen, size);

	if (copylen > 0) {
		memcpy(dst, src, copylen);
		dst[copylen - 1] = '\0';
	}

	return srclen;
}

char *mystrtok_r(char *s, const char *sep, char **lasts)
{
	char *t;

	if (!s)
		s = *lasts;

	while (*s && strchr(sep, *s))
		s++;

	if (!*s)
		return nullptr;

	t = s;
	while (*t) {
		if (strchr(sep, *t)) {
			*t++ = '\0';
			break;
		}
		t++;
	}

	*lasts = t;
	return s;
}

void str_replace(std::string &str, char from, char to)
{
	std::replace(str.begin(), str.end(), from, to);
}

int mt_snprintf(char *buf, const size_t buf_size, const char *fmt, ...)
{
	// https://msdn.microsoft.com/en-us/library/bt7tawza.aspx
	//  Many of the MSVC / Windows printf-style functions do not support positional
	//  arguments (eg. "%1$s"). We just forward the call to vsnprintf for sane
	//  platforms, but defer to _vsprintf_p on MSVC / Windows.
	// https://github.com/FFmpeg/FFmpeg/blob/5ae9fa13f5ac640bec113120d540f70971aa635d/compat/msvcrt/snprintf.c#L46
	//  _vsprintf_p has to be shimmed with _vscprintf_p on -1 (for an example see
	//  above FFmpeg link).
	va_list args;
	va_start(args, fmt);
#ifndef _MSC_VER
	int c = vsnprintf(buf, buf_size, fmt, args);
#else  // _MSC_VER
	int c = _vsprintf_p(buf, buf_size, fmt, args);
	if (c == -1)
		c = _vscprintf_p(fmt, args);
#endif // _MSC_VER
	va_end(args);
	return c;
}