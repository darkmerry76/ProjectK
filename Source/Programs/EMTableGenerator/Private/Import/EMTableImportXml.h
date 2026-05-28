#pragma once

#include "EMTableGenerator.h"
#include "EMTableImport.h"

#include "pugixml.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMTableImportXML
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EMTableImportXML : public EMTableImport
{
public:
	virtual ~EMTableImportXML()
	{
		Clear();
	}

	pugi::xml_document* GetXMLDocument() { return XMLDocument; };

	virtual void Clear() override;

	virtual bool DoImport(const FString& Path) override;

protected:
	pugi::xml_document*	XMLDocument;
};