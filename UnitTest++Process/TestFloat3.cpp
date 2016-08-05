#include "../UnitTest++/src/UnitTest++.h"

#include "../EDMath/Float3.h"
using namespace EDMath;

Float3 theTestObj(42.0f, 49.5f, .1f);

struct float3Fixture
{
	float3Fixture() {  }
	~float3Fixture() { theTestObj = Float3(42.0f, 49.5f, .1f); }
};

TEST(float3Constructor)
{
	CHECK_EQUAL(42.0f, theTestObj.x);
	CHECK_EQUAL(49.5f, theTestObj.y);
	CHECK_EQUAL(0.1f,  theTestObj.z);
}

TEST(float3UnionStructure)
{
	CHECK_CLOSE(42.0f, theTestObj.x, 0.001f);
	CHECK_CLOSE(42.0f, theTestObj.r, 0.001f);
	CHECK_CLOSE(42.0f, theTestObj.v[0], 0.001f);

	CHECK_CLOSE(49.5f, theTestObj.y, 0.001f);
	CHECK_CLOSE(49.5f, theTestObj.g, 0.001f);
	CHECK_CLOSE(49.5f, theTestObj.v[1], 0.001f);

	CHECK_CLOSE(.1f, theTestObj.z, 0.001f);
	CHECK_CLOSE(.1f, theTestObj.b, 0.001f);
	CHECK_CLOSE(.1f, theTestObj.v[2], 0.001f);
}

TEST(float3Assignment)
{
	Float3 leftHand;
	leftHand = theTestObj;

	CHECK_CLOSE(42.0f, leftHand.x, 0.001f);
	CHECK_CLOSE(49.5f, leftHand.y, 0.001f);
	CHECK_CLOSE(0.1f,  leftHand.z, 0.001f);
}

TEST(float3CopyConstructor)
{
	Float3 leftHand = theTestObj;

	CHECK_CLOSE(42.0f, leftHand.x, 0.001f);
	CHECK_CLOSE(49.5f, leftHand.y, 0.001f);
	CHECK_CLOSE(0.1f,  leftHand.z, 0.001f);
}

TEST(float3Add)
{
	Float3 rightHand(1.0f, 2.0f, 3.0f);
	Float3 leftHand = rightHand + theTestObj;

	CHECK_CLOSE(43.0f, leftHand.x, 0.001f);
	CHECK_CLOSE(51.5f, leftHand.y, 0.001f);
	CHECK_CLOSE(3.1f,  leftHand.z, 0.001f);
}

TEST(float3Subtract)
{
	Float3 rightHand(1.0f, 2.0f, 3.0f);
	Float3 leftHand = rightHand - theTestObj;

	CHECK_CLOSE(-41.0f, leftHand.x, 0.001f);
	CHECK_CLOSE(-47.5f, leftHand.y, 0.001f);
	CHECK_CLOSE(2.9f,   leftHand.z, 0.001f);
}

TEST(float3Multiply)
{
	Float3 rightHand(1.0f, 2.0f, 3.0f);
	Float3 leftHand(3.0f, 2.0f, 1.0f);
	Float3 result = leftHand * rightHand;

	CHECK_CLOSE(3.0f, result.x, 0.001f);
	CHECK_CLOSE(4.0f, result.y, 0.001f);
	CHECK_CLOSE(3.0f, result.z, 0.001f);
}

TEST(float3Divide)
{
	Float3 rightHand(2.0f, 8.0f, 8.0f);
	Float3 leftHand(2.0f, 2.0f, 4.0f);
	Float3 result = leftHand / rightHand;

	CHECK_CLOSE(1.0f,  result.x, 0.001f);
	CHECK_CLOSE(0.25f, result.y, 0.001f);
	CHECK_CLOSE(0.5f,  result.z, 0.001f);
}

TEST(float3ScalarMultiply)
{
	Float3 leftHand(3.0f, 2.0f, 1.0f);
	Float3 result = leftHand * 5;

	CHECK_CLOSE(15.0f, result.x, 0.001f);
	CHECK_CLOSE(10.0f, result.y, 0.001f);
	CHECK_CLOSE(5.0f,  result.z, 0.001f);
}

TEST(float3ScalarDivide)
{
	Float3 leftHand(15.0f, 10.0f, 5.0f);
	Float3 result = leftHand / 5;

	CHECK_CLOSE(3.0f, result.x, 0.001f);
	CHECK_CLOSE(2.0f, result.y, 0.001f);
	CHECK_CLOSE(1.0f, result.z, 0.001f);
}

TEST_FIXTURE(float3Fixture, float3AddAssignment)
{
	Float3 rightHand(1.0f, 2.0f, 3.0f);
	theTestObj += rightHand;

	CHECK_CLOSE(43.0f, theTestObj.x, 0.001f);
	CHECK_CLOSE(51.5f, theTestObj.y, 0.001f);
	CHECK_CLOSE(3.1f,  theTestObj.z, 0.001f);
}

TEST_FIXTURE(float3Fixture, float3SubtractAssignment)
{
	Float3 rightHand(1.0f, 2.0f, 3.0f);
	theTestObj -= rightHand;

	CHECK_CLOSE(41.0f, theTestObj.x, 0.001f);
	CHECK_CLOSE(47.5f, theTestObj.y, 0.001f);
	CHECK_CLOSE(-2.9f, theTestObj.z, 0.001f);
}

TEST_FIXTURE(float3Fixture, float3ScalarAssignment)
{
	theTestObj *= 2.0f;

	CHECK_CLOSE(84.0f, theTestObj.x, 0.001f);
	CHECK_CLOSE(99.0f, theTestObj.y, 0.001f);
	CHECK_CLOSE(0.2f, theTestObj.z, 0.001f);
}

TEST_FIXTURE(float3Fixture, float3MakeZero)
{
	theTestObj.makeZero();

	CHECK_CLOSE(0.0f, theTestObj.x, 0.001f);
	CHECK_CLOSE(0.0f, theTestObj.y, 0.001f);
	CHECK_CLOSE(0.0f, theTestObj.z, 0.001f);
}

TEST_FIXTURE(float3Fixture, float3Negate)
{
	theTestObj.negate();

	CHECK_EQUAL(-42.0f, theTestObj.x);
	CHECK_EQUAL(-49.5f, theTestObj.y);
	CHECK_EQUAL(-0.1f,  theTestObj.z);
}

TEST(float3InvokingDotProduct)
{
	Float3 rightHand(1.0f, 2.0f, 3.0f);
	
	CHECK_CLOSE(141.3, theTestObj.dotProduct(rightHand), 0.001f);
}

TEST(float3DotProduct)
{
	Float3 rightHand(1.0f, 2.0f, 3.0f);
	
	CHECK_CLOSE(141.3, DotProduct(theTestObj, rightHand), 0.001f);
}

TEST(float3CrossProduct)
{
	Float3 rightHand(0.0f, 1.0f, 0.0f);
	Float3 leftHand(1.0f, 0.0f, 0.0f);
	Float3 result; 
	CrossProduct(result, leftHand, rightHand);

	CHECK_CLOSE(0, result.x, 0.001f);
	CHECK_CLOSE(0, result.y, 0.001f);
	CHECK_CLOSE(1, result.z, 0.001f);
}

TEST(float3Magnitude)
{
	CHECK_CLOSE(64.91733, theTestObj.magnitude(), 0.001f);
}

TEST_FIXTURE(float3Fixture, float3Normalize)
{
	Float3 returnResult = theTestObj.normalize();

	CHECK_CLOSE(1, theTestObj.magnitude(), 0.001f);
	CHECK_CLOSE(1, returnResult.magnitude(), 0.001f);
}
