#pragma once
#include <EASTL/string.h>

using namespace eastl;
inline string16 GetFileName(const string16& fpath) {
	string16 name = fpath.rfind(u"/") != string16::npos ? fpath.substr(fpath.rfind(u"/") + 1) : fpath.substr(fpath.rfind(u"\\") + 1);
	name = name.substr(0, name.rfind(u"."));
	if (name.rfind(u"_") != string16::npos && name.rfind(u"p") != string16::npos) {
		string16 k = name.substr(name.rfind(u"_"), name.rfind(u"p")); k.pop_back();
		if (k == u"_backup") name = name.substr(0, name.rfind(u"_"));
	}
	return name;
}

inline string16 GetFileFormat(const string16& fpath) {
	return fpath.substr(fpath.rfind(u"."));
}
