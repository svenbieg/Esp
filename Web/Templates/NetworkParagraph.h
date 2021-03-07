//====================
// NetworkParagraph.h
//====================

#pragma once


//=======
// Using
//=======

#include "Web/Controls/Button.h"
#include "Web/Controls/Paragraph.h"


//===========
// Namespace
//===========

namespace Web {
	namespace Templates {


//===================
// Network-Paragraph
//===================

class NetworkParagraph: public Web::Controls::Paragraph
{
private:
	// Using
	using Button=Web::Controls::Button;

public:
	// Con-/Destructors
	NetworkParagraph();

private:
	// Common
	VOID OnConnectClick(Handle<Button> Button);
	Handle<StringVariable> hStationNetwork;
	Handle<StringVariable> hStationPassword;
};

}}