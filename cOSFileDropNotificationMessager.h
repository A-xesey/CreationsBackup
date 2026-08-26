#pragma once
#include "Spore/BasicIncludes.h"

class cOSFileDropNotificationMessager
{
public:
	virtual ~cOSFileDropNotificationMessager() { delete this; };
	int AddRef();
	int Release();

public:
	/* 04h */	vector<eastl::string16> mFileStrs;
	/* 18h */	int mnRefCount;
	/* 1Ch */	void* mpOSData;
};
ASSERT_SIZE(cOSFileDropNotificationMessager, 0x20);

//cOSFileDropNotificationMessager::~cOSFileDropNotificationMessager() {
//	delete this;
//}

inline int cOSFileDropNotificationMessager::AddRef() {
	return mnRefCount++;
}

#include <ole2.h>
inline int cOSFileDropNotificationMessager::Release() {
	if (mnRefCount > 1) return mnRefCount--;
	if (this->mpOSData) ReleaseStgMedium((LPSTGMEDIUM)this->mpOSData);
	delete this;
}

