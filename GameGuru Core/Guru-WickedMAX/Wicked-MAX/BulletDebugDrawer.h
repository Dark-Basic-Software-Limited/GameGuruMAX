#ifndef _H_BULLET_DEBUG_DRAWER
#define _H_BULLET_DEBUG_DRAWER

// We upgraded to BYLLET3 but this header does not know about WICKEDENGINE
#define BT_NO_SIMD_OPERATOR_OVERLOADS
#include "../../SDK/BULLET/bullet-3.19/src/LinearMath/btIDebugDraw.h"
#include <vector>

class btCollisionShape;

class BulletDebugDrawer : public btIDebugDraw
{
public:
	BulletDebugDrawer();
	void SetWorldScale(float scale);
	void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
	void drawTransform(const btTransform& transform, btScalar orthoLen) override;
	void /**/drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override;
	void /**/reportErrorWarning(const char* warningString) override;
	void /**/draw3dText(const btVector3& location, const char* textString) override;
	void setDebugMode(int debugMode) override;
	int	 getDebugMode() const override;
	inline int GetSize() { return elementCount; }
	inline void SetDrawTransforms(bool draw) { drawTransforms = draw; }

	int elementCount;
	int mode;
	bool drawTransforms;
	std::vector<float> lineData;
	// Separate storage for compound physics objects.
	std::vector<unsigned int> iRagdollID;

	// For drawing on a per-object basis.
	std::vector<unsigned int> iDrawObjects;
	float scale;
};

#endif