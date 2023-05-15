#include "stdafx.h"

void SetExceptionSender(IPythonExceptionSender * pkExceptionSender)
{
	g_pkExceptionSender = pkExceptionSender;
}
