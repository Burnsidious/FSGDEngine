#include "Collidable.h"
#include "CollidableTypes.h"
#include "CollisionLibrary.h"

using namespace EDMath;
using namespace std;

namespace EDCollision
{
    bool Collidable::Collides(Collidable* inputPtr, CollisionResponseFunc func, CollisionFeedback *feedbackPtr)
    {
        CollisionFeedback defaultFeedback;
        if( feedbackPtr == 0 )
            feedbackPtr = &defaultFeedback;

        Collidable *invokingPtr = this;
        if( this->GetCOType() > inputPtr->GetCOType() )
        {
            invokingPtr = inputPtr;
            inputPtr = this;
        }

        feedbackPtr->collisionPair.collidablePtrs[0] = invokingPtr;
        feedbackPtr->collisionPair.collidablePtrs[1] = inputPtr;

        if( CollisionLibrary::GetInstance().GetPairCollisionFunc( invokingPtr->GetCOType(), inputPtr->GetCOType())(feedbackPtr) )
        {
            if( func != 0 )
                func( feedbackPtr );

            return true;
        }

        return false;
    }	

    void Collidable::InverseTransform(const Float4x4 &transform)
    {
        Float4x4 inverse = transform;
        inverse.transpose();
        inverse.WAxis.x = -DotProduct( transform.WAxis, transform.XAxis );
        inverse.WAxis.y = -DotProduct( transform.WAxis, transform.YAxis );
        inverse.WAxis.z = -DotProduct( transform.WAxis, transform.ZAxis );

        Transform(inverse);
    }

    void BoundingVolume::WriteBV(fstream &fout, BoundingVolume* bvPtr)
    {
        unsigned int bvType = bvPtr->GetCOType();
        fout.write( (char*)&bvType, sizeof(unsigned int) );

        switch(bvType)
        {
        case EDCO_SPHERE:
            {
                Sphere* spherePtr = (Sphere*)bvPtr;

                fout.write( (char*)spherePtr->center.v, sizeof(Float3) );
                fout.write( (char*)&spherePtr->radius, sizeof(float) );
            }
            break;
        case EDCO_AABB:
            {
                Aabb* aabbPtr = (Aabb*)bvPtr;

                fout.write( (char*)aabbPtr->min.v, sizeof(Float3) );
                fout.write( (char*)aabbPtr->max.v, sizeof(Float3) );
            }
            break;
        default:
            break;
        }	
    }

    void BoundingVolume::ReadBV(fstream &fin, BoundingVolume* &bvPtr)
    {
        unsigned int bvType;
        fin.read( (char*)&bvType, sizeof(unsigned int) );

        switch(bvType)
        {
        case EDCO_SPHERE:
            {
				bvPtr = new Sphere;
                Sphere* spherePtr = (Sphere*)bvPtr;

                fin.read( (char*)spherePtr->center.v, sizeof(Float3) );
                fin.read( (char*)&spherePtr->radius, sizeof(float) );
            }
            break;
        case EDCO_AABB:
            {
				bvPtr = new Aabb;
                Aabb* aabbPtr = (Aabb*)bvPtr;

                fin.read( (char*)aabbPtr->min.v, sizeof(Float3) );
                fin.read( (char*)aabbPtr->max.v, sizeof(Float3) );
            }
            break;
        default:
            break;
        }
    }

    void BoundingVolume::GetSplitAxis(const float *pointsPtr, unsigned int numPoints, 
		Float3 &splitAxis, float &_min, float &_max)
    {
        Float3 vMin( FLT_MAX, FLT_MAX, FLT_MAX );
        Float3 vMax( -FLT_MAX, -FLT_MAX, -FLT_MAX );

        for(unsigned int i = 0; i < numPoints; i +=3 )
            for(unsigned int j = 0; j < 3; ++j)
            {
                vMin.v[j] = min( vMin.v[j], pointsPtr[i+j] );
                vMax.v[j] = max( vMax.v[j], pointsPtr[i+j] );
            }

            unsigned int axis = 0;
            if( vMax.v[axis] - vMin.v[axis] < vMax.v[1] - vMin.v[1] )
                axis = 1;
            if( vMax.v[axis] - vMin.v[axis] < vMax.v[2] - vMin.v[2] )
                axis = 2;

            splitAxis.makeZero();
            splitAxis.v[axis] = 1.0f;

            _min = vMin.v[axis];
            _max = vMax.v[axis];
    }
}