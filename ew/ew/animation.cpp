#include "animation.h"
#include "importUtil.h"
#include <assert.h>
#include "ewMath/transformations.h"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>


namespace ew {
	//BONE
	Bone::Bone(const std::string& name, int ID, const aiNodeAnim* channel)
		:m_name(name), m_ID(ID)
	{
		m_localTransform = ew::IdentityMatrix();

		//Load position keyframes
		m_numPositions = channel->mNumPositionKeys;
		for (size_t i = 0; i < m_numPositions; i++)
		{
			KeyPosition data;
			data.position = aiVec3(channel->mPositionKeys[i].mValue);
			data.timeStamp = channel->mPositionKeys[i].mTime;
			m_Positions.push_back(data);
		}
		//Load rotation keyframes
		m_numRotations = channel->mNumRotationKeys;
		for (size_t i = 0; i < m_numRotations; i++)
		{
			KeyRotation data;
			data.orientation = aiQuat(channel->mRotationKeys[i].mValue);
			data.timeStamp = channel->mRotationKeys[i].mTime;
			m_Rotations.push_back(data);
		}
		//Load scale keyframes
		m_numScales = channel->mNumScalingKeys;
		for (size_t i = 0; i < m_numScales; i++)
		{
			KeyScale data;
			data.scale = aiVec3(channel->mScalingKeys[i].mValue);
			data.timeStamp = channel->mScalingKeys[i].mTime;
			m_scales.push_back(data);
		}
	}
	void Bone::Update(float time)
	{
		m_localTransform = InterpolatePosition(time) * InterpolateRotation(time) * InterpolateScale(time);
	}

	int Bone::GetPositionIndex(float time)
	{
		for (size_t i = 0; i < m_numPositions-1; i++)
		{
			if (time < m_Positions[i + 1].timeStamp)
				return i;
		}
		assert(0);
	}
	int Bone::GetRotationIndex(float time)
	{
		for (size_t i = 0; i < m_numRotations - 1; i++)
		{
			if (time < m_Rotations[i + 1].timeStamp)
				return i;
		}
		assert(0);
	}
	int Bone::GetScaleIndex(float time)
	{
		for (size_t i = 0; i < m_numScales - 1; i++)
		{
			if (time < m_scales[i + 1].timeStamp)
				return i;
		}
		assert(0);
	}
	Mat4 Bone::InterpolatePosition(float time)
	{
		if (m_numPositions == 1)
			return ew::TranslationMatrix(m_Positions[0].position);

		int p0Index = GetPositionIndex(time);
		int p1Index = p0Index + 1;
		//Normalized 0-1 time between keyframes
		float t = InverseLerp(m_Positions[p0Index].timeStamp, m_Positions[p1Index].timeStamp, time);
		Vec3 finalPosition = Lerp(m_Positions[p0Index].position, m_Positions[p1Index].position, t);
		return ew::TranslationMatrix(finalPosition);
	}
	Mat4 Bone::InterpolateRotation(float time)
	{
		if (m_numRotations == 1) {
			return ew::ToRotationMatrix(ew::Normalize(m_Rotations[0].orientation));
		}
		int p0Index = GetRotationIndex(time);
		int p1Index = p0Index + 1;
		//Normalized 0-1 time between keyframes
		float t = InverseLerp(m_Rotations[p0Index].timeStamp, m_Rotations[p1Index].timeStamp, time);
		Quaternion finalRotation = Slerp(m_Rotations[p0Index].orientation, m_Rotations[p1Index].orientation, t);
		finalRotation = Normalize(finalRotation);
		return ew::ToRotationMatrix(finalRotation);
	}
	Mat4 Bone::InterpolateScale(float time)
	{
		if (m_numScales == 1)
			return ew::ScaleMatrix(m_scales[0].scale);
		int p0Index = GetScaleIndex(time);
		int p1Index = p0Index + 1;
		float t = InverseLerp(m_scales[p0Index].timeStamp, m_scales[p1Index].timeStamp, time);
		Vec3 finalScale = Lerp(m_scales[p0Index].scale, m_scales[p1Index].scale, t);
		return ew::ScaleMatrix(finalScale);
	}

	//ANIMATION
	Animation::Animation(const char* filePath)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate);
		assert(scene && scene->mRootNode);
		auto animation = scene->mAnimations[0];
		m_duration = animation->mDuration;
		m_ticksPerSecond = animation->mTicksPerSecond;
		ReadHierarchyData(m_rootNode, scene->mRootNode);
	}
	void Animation::ReadHierarchyData(AssimpNodeData& dest, const aiNode* src)
	{
		assert(src);
		dest.name = src->mName.data;
		dest.transformation = aiMat4(src->mTransformation);
		dest.childrenCount = src->mNumChildren;
		for (size_t i = 0; i < src->mNumChildren; i++)
		{
			AssimpNodeData newData;
			ReadHierarchyData(newData, src->mChildren[i]);
			dest.children.push_back(newData);
		}
	}
	void Animation::ReadBones(const aiAnimation* animation, Model& model)
	{
		int size = animation->mNumChannels;
		auto& boneInfoMap = model.GetBoneInfoMap();
		int& boneCount = model.GetBoneCount();
		for (size_t i = 0; i < size; i++)
		{
			aiNodeAnim* channel = animation->mChannels[i];
			std::string boneName = channel->mNodeName.data;
			if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
				boneInfoMap[boneName].id = boneCount;
				boneCount++;
			}
			Bone bone = Bone(boneName, boneInfoMap[boneName].id, channel);
			m_bones.push_back(bone);
		}
		m_boneInfoMap = boneInfoMap;
	}
}