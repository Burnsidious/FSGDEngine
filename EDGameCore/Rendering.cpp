#include "Rendering.h"
#include "Light.h"
#include "Camera.h"
#include "Renderer.h"

namespace EDGameCore
{
	std::set<Camera*, compstruct> Rendering::cameras;
	std::list<IDirectionalLight*> Rendering::directionalLightList;
	IPointLight* Rendering::currentLight = 0;
	unsigned int Rendering::frameId = 0;
	Rendering::RenderableTree Rendering::renderableTree;
	Rendering::LightTree Rendering::lightTree;
	int Rendering::windowWidth = 0;
	int Rendering::windowHeight = 0;
	ISkyBox* Rendering::skyBoxPtr = 0;
	Camera* Rendering::currentCamera = 0;

	void Rendering::Update(GameObject* obj)
	{
		Camera* camera = obj->GetCamera();
		if( camera != 0 && camera->isEnabled() )
		{
			cameras.insert( camera );
			camera->containingSet = &cameras;
		}

		IRenderer* renderer = obj->GetRenderer();
		if( renderer != 0 )
		{
			renderer->ClearLights();

			if( renderer->renderableTreeLeaf == 0 )
			{
				renderer->renderableTreeLeaf = renderableTree.Insert(renderer);
				renderer->containingTree = &renderableTree;
			}
			else
				renderableTree.Update( renderer->renderableTreeLeaf );
		}
		
		ILight* light = obj->GetLight();
		if( light != 0 )
		{
			light->Update();
					
			if( light->GetLightType() != ILight::DIRECTIONAL )
			{
				IPointLight* pointlight = (IPointLight*)light;
						
				if( pointlight->lightTreeLeaf == 0 )
				{
					pointlight->lightTreeLeaf = lightTree.Insert(pointlight);
					pointlight->containingTree = &lightTree;
				}
				else
					lightTree.Update( pointlight->lightTreeLeaf );
			}
			else
			{
				IDirectionalLight* directionalLight = (IDirectionalLight*)light;

				directionalLightList.push_back( directionalLight );
				directionalLight->containingList = &directionalLightList;
			}
		}
	}

	void Rendering::BeginFrame(void)
	{
		++frameId;
		currentLight = 0;

		while( !directionalLightList.empty() )
		{
			directionalLightList.front()->containingList = 0;
			directionalLightList.pop_front();
		}

		while( !cameras.empty() )
		{
			(*cameras.begin())->containingSet = 0;
			cameras.erase( cameras.begin() );
		}
	}

	void Rendering::PreLitTraversal(Camera* camera, EDSpatial::DynAabbTree::Node* node)
	{
		const Float4x4& worldMatrix = camera->GetTransform()->GetWorldMatrix();

		const EDMath::Aabb& aabb = node->GetAabb();

		for(int i = 0; i < 6; ++i)
		{
			float r = aabb.extents.x * abs(camera->frustum[i].normal.x) + 
				aabb.extents.y * abs(camera->frustum[i].normal.y) +
				aabb.extents.z * abs(camera->frustum[i].normal.z);

			float d = DotProduct(camera->frustum[i].normal, aabb.center) - camera->frustum[i].distance;

			if( d < -r )
				return;
		}
		
		VisibleTreeBranch* branch = (VisibleTreeBranch*)node->asBranch();
		RenderableTreeLeaf* leaf = (RenderableTreeLeaf*)node->asLeaf();

		if( branch )
		{
			PreLitTraversal( camera, branch->leftChild );
			PreLitTraversal( camera, branch->rightChild );
		}
		else if( leaf )
		{
			if( camera->CullRenderable( leaf->object ) )
			{
				leaf->object->ClearLights();
				ProcessRendererPreLit( leaf->object );
			}
		}
	}

	void Rendering::PostLitTraversal(Camera* camera, EDSpatial::DynAabbTree::Node* node)
	{
		const Float4x4& worldMatrix = camera->GetTransform()->GetWorldMatrix();

		const EDMath::Aabb& aabb = node->GetAabb();

		for(int i = 0; i < 6; ++i)
		{
			float r = aabb.extents.x * abs(camera->frustum[i].normal.x) + 
				aabb.extents.y * abs(camera->frustum[i].normal.y) +
				aabb.extents.z * abs(camera->frustum[i].normal.z);

			float d = DotProduct(camera->frustum[i].normal, aabb.center) - camera->frustum[i].distance;

			if( d < -r )
				return;
		}
		
		VisibleTreeBranch* branch = (VisibleTreeBranch*)node->asBranch();
		RenderableTreeLeaf* leaf = (RenderableTreeLeaf*)node->asLeaf();
		if( branch )
		{
			PostLitTraversal( camera, branch->leftChild );
			PostLitTraversal( camera, branch->rightChild );
		}
		else if( leaf )
		{
			if( camera->CullRenderable( leaf->object ) )
			{
				ProcessRendererPostLit( leaf->object );
			}
		}
	}

	void Rendering::LightTraversal(Camera* camera, EDSpatial::DynAabbTree::Node* node )
	{
		const Float4x4& worldMatrix = camera->GetTransform()->GetWorldMatrix();

		const EDMath::Aabb& aabb = node->GetAabb();

		for(int i = 0; i < 6; ++i)
		{
			float r = aabb.extents.x * abs(camera->frustum[i].normal.x) + 
				aabb.extents.y * abs(camera->frustum[i].normal.y) +
				aabb.extents.z * abs(camera->frustum[i].normal.z);

			float d = DotProduct(camera->frustum[i].normal, aabb.center) - camera->frustum[i].distance;

			if( d < -r )
				return;
		}
		
		VisibleTreeBranch* branch = (VisibleTreeBranch*)node->asBranch();
		LightTreeLeaf* leaf = (LightTreeLeaf*)node->asLeaf();
		if( branch )
		{
			LightTraversal( camera, branch->leftChild );
			LightTraversal( camera, branch->rightChild );
		}
		else if( leaf )
		{
			if( camera->CullLight( leaf->object ) )
			{
				leaf->object->visibleSetHead = 0;
				
				LightSubTraversal( leaf->object, renderableTree.GetRoot() );

				ProcessLight( leaf->object );
			}
		}
	}

	void Rendering::LightSubTraversal(ILight* light, EDSpatial::DynAabbTree::Node* node )
	{
		if( !light->TestAabb( node->GetAabb() ) )
			return;

		VisibleTreeBranch* branch = (VisibleTreeBranch*)node->asBranch();
		RenderableTreeLeaf* leaf = (RenderableTreeLeaf*)node->asLeaf();
		if( branch )
		{
			LightSubTraversal( light, branch->leftChild );
			LightSubTraversal( light, branch->rightChild );
		}
		else if( leaf )
		{
			if( light->CullRenderable( leaf->object ) )
			{
				if( leaf->object->CastsShadows() )
				{
					leaf->object->visibleSetNext = light->visibleSetHead;
					light->visibleSetHead = leaf->object;
				}

				leaf->object->AddLight( light );
			}
		}
	}

	void Rendering::Render(void)
	{
		auto cameraIter = cameras.begin();
		auto cameraEnd = cameras.end();


		for(unsigned int i = 0; i < 32 && cameraIter != cameraEnd; ++i)
		{
			currentCamera = *cameraIter;

			ProcessCamera();

			//- For each visible renderer
			//	- PrelitProcess
			if( renderableTree.GetRoot() != 0 )
				PreLitTraversal( *cameraIter, renderableTree.GetRoot() );

			ProcessPreLitVisibleSet();

			//- For each visible light
			//	- Build visible renderer set
			//	- ProcessLight
			if( lightTree.GetRoot() != 0 )
				LightTraversal( *cameraIter, lightTree.GetRoot() );

			auto directionalIter = directionalLightList.begin();
			auto directionalEnd = directionalLightList.end();
			for(; directionalIter != directionalEnd; ++directionalIter)
			{
				IDirectionalLight* directionalLight = *directionalIter;

				if( !currentCamera->CullLight( directionalLight ) )
					continue;

				directionalLight->visibleSetHead = 0;
				directionalLight->SetAabb( currentCamera->GetFrustumAabb() );
				
				LightSubTraversal( directionalLight, renderableTree.GetRoot() );
				ProcessLight( directionalLight );
			}

			ProcessVisibleLightSet();

			//- For each visible renderer
			//	- PostLitProcess
			if( renderableTree.GetRoot() != 0 )
				PostLitTraversal( *cameraIter, renderableTree.GetRoot() );

			ProcessPostLitVisibleSet();

			PostProcessCamera();

			++cameraIter;
		}

		currentCamera = 0;
	}

	Rendering::RenderableTreeLeaf* Rendering::RenderableTree::createLeaf(void)const
	{
		return new RenderableTreeLeaf();
	}

	Rendering::VisibleTreeBranch* Rendering::RenderableTree::createBranch(void)const
	{
		return new VisibleTreeBranch();
	}

	Rendering::LightTreeLeaf* Rendering::LightTree::createLeaf(void)const
	{
		return new LightTreeLeaf();
	}

	Rendering::VisibleTreeBranch* Rendering::LightTree::createBranch(void)const
	{
		return new VisibleTreeBranch();
	}
}