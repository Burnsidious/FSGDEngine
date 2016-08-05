#include "FSGDOcclusionQuery.h"

using namespace EDMath;
using namespace EDGOCA;

//void FSGDOcclusionQuery::Initialize(void)
//{
//	RenderController::GetInstance()->InitializeAVisibilityQuery(visQueryInterface);
//}
//
//void FSGDOcclusionQuery::Issue(const Aabb& aabb)
//{
//	Float4x4 transform;
//	transform.makeIdentity();
//
//	transform.WAxis = (aabb.max + aabb.min) * 0.5f;
//	transform.XAxis.x = aabb.max.x - transform.WAxis.x;
//	transform.YAxis.y = aabb.max.y - transform.WAxis.y;
//	transform.ZAxis.z = aabb.max.z - transform.WAxis.z;
//
//	//visQueryInterface.IssueQuery( transform );
//}
//
//int FSGDOcclusionQuery::GetQueryResult(void)
//{
//	return visQueryInterface.GetResult();
//}
//
//FSGDOcclusionQuery::~FSGDOcclusionQuery()
//{
//}
//
//IOcclusionQueryInterface* FSGDOcclusionQuery::creator(void)
//{
//	FSGDOcclusionQuery* occlusionQuery = new FSGDOcclusionQuery();
//
//	occlusionQuery->Initialize();
//
//	return occlusionQuery;
//}