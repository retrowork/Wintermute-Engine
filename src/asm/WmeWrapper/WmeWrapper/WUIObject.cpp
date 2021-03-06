// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt
// http://dead-code.org/redir.php?target=wme

#include "StdAfx.h"
#include "WUIObject.h"
#include "WUtils.h"

using namespace System;
using namespace DeadCode::WME::Core;


//////////////////////////////////////////////////////////////////////////
WUIObject::WUIObject(WGame^ Game):WObject()
{
	m_Game = Game;
	m_Native = new CUIObject(Game->Native);
	m_SelfDestroy = true;
}

//////////////////////////////////////////////////////////////////////////
WUIObject::WUIObject(CUIObject* Native):WObject(Native)
{
}

//////////////////////////////////////////////////////////////////////////
WUIObject::~WUIObject(void)
{
}
