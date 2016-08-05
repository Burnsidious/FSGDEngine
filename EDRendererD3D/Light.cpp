#include "Light.h"
#include "RenderModel.h"
#include "RenderShapeLight.h"

namespace EDRendererD3D
{
	Light::~Light()
	{
		delete lightModelPtr;
	}

	void Light::ApplyLight(RenderContext &context)
	{
		assert(0);
		//ID3DX11Effect *effectPtr = context.GetEffect();
  //      ID3DX11EffectVariable * theLight = effectPtr->GetVariableByName(name.c_str());

		//theLight->GetMemberByName("DiffuseColor")->AsVector()->SetRawValue(&diffuseColor, 0, sizeof(diffuseColor));

		//theLight->GetMemberByName("AmbientColor")->AsVector()->SetRawValue(&ambientColor, 0, sizeof(ambientColor));

		//theLight->GetMemberByName("SpecularColor")->AsVector()->SetRawValue(&specularColor, 0, sizeof(specularColor));

		//theLight->GetMemberByName("SpecularPower")->AsScalar()->SetFloat(specularPower);

		//theLight->GetMemberByName("SpecularIntensity")->AsScalar()->SetFloat(specularIntensity);

		//theLight->GetMemberByName("Enabled")->AsScalar()->SetBool(isEnabled);
	}

	void Light::AddToContextSet() 
	{ 
		lightModelPtr->GetShapes()[0]->AddToContextSet(); 
	}

	void Light::LoadModel(const char *fileName)
    {
		//delete lightModelPtr;

		//lightModelPtr = new (RenderModel)(fileName);

		//((RenderShapeLight *)lightModelPtr->GetShapes()[0])->SetLight(this);
    }
	
	XMFLOAT4X4 *Light::GetWorldMatrixPtr() 
	{ 
		return lightModelPtr->GetShapes()[0]->GetWorldMatrixPtr(); 
	}
	void Light::SetWorldMatrixPtr(XMFLOAT4X4 * mat)
	{
		lightModelPtr->GetShapes()[0]->SetWorldMatrix(mat);
	}
}
