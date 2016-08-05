#pragma once

#include <vector>


#include "../EDMath/Float3.h"
#include "../EDMath/aabb.h"
#include "../EDMath/sphere.h"

class TiXmlElement;
class GDAttributeBase
{
protected:
	int GetItemCount(TiXmlElement* itemListElementPtr);
	virtual void clear(void) = 0;
	virtual void resize(unsigned int size) = 0;
	virtual size_t size(void)const = 0;

public:
	virtual ~GDAttributeBase(void){}
	
	//virtual void Read(std::ifstream &fin) = 0;
	//virtual void Read(const char* pMeshFileName) = 0;
	void Read(const char* fileName);
	void Read(std::ifstream* fileStreamPtr);
	void ReadXML(const char* meshXMLFileName);
	void ReadXML(TiXmlElement* elementPtr);
	virtual void ReadXMLItem(TiXmlElement* itemElementPtr, unsigned int itemIndex) = 0;
	virtual void ReadItem(std::ifstream* fileStreamPtr, unsigned int itemIndex) = 0;
	virtual const char* GetAttributeTypeName(void) = 0;
	virtual const char* GetAttributeName(void) = 0;
	//virtual void SetAttributeName(const char*_name) = 0;

	static void ReadTransformXML(TiXmlElement* elementPtr, float* transformPtr);
	static void ReadVector3XML(TiXmlElement* elementPtr, float* vectorPtr);
	static void ReadVector4XML(TiXmlElement* elementPtr, float* vectorPtr);
	static void ReadAabbXML(TiXmlElement* elementPtr, EDMath::Aabb* aabbPtr);
	static void ReadSphereXML(TiXmlElement* elementPtr, EDMath::Sphere* spherePtr);
	static void ReadColor4XML(TiXmlElement* elementPtr, float* vectorPtr);
};



template<typename Type>
class GDAttribute : public GDAttributeBase
{
	std::string attributeName;
	std::vector< Type > values;

public:
	virtual ~GDAttribute(void){}

	void resize(unsigned int size){ values.resize(size); }
	void clear(void){ values.clear(); }
	Type& operator[](size_t index){ return values[index]; }
	const Type& operator[](size_t index)const { return values[index]; }
	size_t size(void)const { return values.size(); }

	void ReadXMLItem(TiXmlElement* itemElementPtr, unsigned int itemIndex){}
	void ReadItem(std::ifstream* fileStreamPtr, unsigned int itemIndex){}
	//void Read(std::ifstream &fin){}
	//void Read(const char* pMeshFileName){}

	static const char* _getAttributeTypeName(void){ return 0; }
	const char* GetAttributeTypeName(void){ return _getAttributeTypeName(); }

	const char* GetAttributeName(void){ return (attributeName.length() == 0 ? 0 : attributeName.c_str()); }
	void SetAttributeName(const char*_name){ attributeName = _name; }

	static GDAttribute<Type>* LoadXML(TiXmlElement* elementPtr, const char* indentifier = 0)
	{
		GDAttribute<Type>* newAttributePtr = new GDAttribute<Type>;

		if( indentifier != 0 )
			newAttributePtr->attributeName = indentifier;

		newAttributePtr->ReadXML(elementPtr);

		if( !newAttributePtr->values.empty() )
			return newAttributePtr;

		delete newAttributePtr;
		return 0;
	}

	static GDAttribute<Type>* LoadXML(const char* xmlFileNamePtr, const char* indentifier = 0)
	{
		GDAttribute<Type>* newAttributePtr = new GDAttribute<Type>;

		if( indentifier != 0 )
			newAttributePtr->attributeName = indentifier;

		newAttributePtr->ReadXML(xmlFileNamePtr);

		if( !newAttributePtr->values.empty() )
			return newAttributePtr;

		delete newAttributePtr;
		return 0;
	}

	static GDAttribute<Type>* Load(const char* fileName, const char* indentifier = 0)
	{
		GDAttribute<Type>* newAttributePtr = new GDAttribute<Type>;

		if( indentifier != 0 )
			newAttributePtr->attributeName = indentifier;

		newAttributePtr->Read(fileName);

		if( !newAttributePtr->values.empty() )
			return newAttributePtr;

		delete newAttributePtr;
		return 0;
	}	
};

