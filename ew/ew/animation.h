#pragma once
#include "ewMath/vec3.h"
#include "ewMath/quaternion.h"
#include "ewMath/mat4.h"

#include <vector>
#include <string>
#include <map>

#include <assimp/anim.h>
#include "model.h"

namespace ew {
	struct KeyPosition {
		Vec3 position;
		float timeStamp; //seconds
	};
	struct KeyRotation {
		Quaternion orientation;
		float timeStamp;
	};
	struct KeyScale {
		Vec3 scale;
		float timeStamp;
	};
	class Bone {
	public:
		Bone(const std::string& name, int ID, const aiNodeAnim* channel);
		void Update(float time);
		int GetPositionIndex(float time);
		int GetRotationIndex(float time);
		int GetScaleIndex(float time);
		//Getters
		inline Mat4 GetLocalTransform() { return m_localTransform; }
		inline std::string GetBoneName() const { return m_name; }
		inline int GetBoneID() const { return m_ID; }
	private:
		Mat4 InterpolatePosition(float time);
		Mat4 InterpolateRotation(float time);
		Mat4 InterpolateScale(float time);
		std::vector<KeyPosition> m_Positions;
		std::vector<KeyRotation> m_Rotations;
		std::vector<KeyScale> m_scales;
		int m_numPositions;
		int m_numRotations;
		int m_numScales;
		Mat4 m_localTransform; //Full affine matrix for this bone
		std::string m_name;
		int m_ID;

	};
	struct AssimpNodeData {
		Mat4 transformation;
		std::string name;
		int childrenCount;
		std::vector<AssimpNodeData> children;
	};

	struct Animation {
	public:
		Animation() = default;
		Animation(const char* filePath);
		inline float GetTicksPerSecond() { return m_ticksPerSecond; }
		inline float GetDuration() { return m_duration; }
		inline const AssimpNodeData& GetRootNode() { return m_rootNode; }
		inline const std::map<std::string, BoneInfo>& GetBoneIDMap() { return m_boneInfoMap; }
	private:
		void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src);
		void ReadBones(const aiAnimation* animation, Model& model);

		float m_ticksPerSecond;
		float m_duration;
		std::vector<Bone> m_bones;
		AssimpNodeData m_rootNode;
		std::map<std::string, BoneInfo> m_boneInfoMap;
	};
}