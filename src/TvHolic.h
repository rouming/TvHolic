#ifndef __TVHOLIC_H__
#define __TVHOLIC_H__

#include "brisadevice.h"

#include "ConnectionManagerService.h"
#include "ContentDirectoryService.h"

class TvHolic : public Brisa::BrisaDevice
{
	Q_OBJECT
public:
	TvHolic();
	~TvHolic();

private:
	ContentDirectoryService* m_contDirService;
	ConnectionManagerService* m_connMngService;
};

#endif //__TVHOLIC_H__
