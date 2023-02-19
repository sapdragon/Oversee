#pragma once

class C_BoneAccessor
{
public:
	matrix3x4_t* GetBoneArrayForWrite()
	{
		return m_aBoneArray;
	}

	void SetBoneArrayForWrite(matrix3x4_t* bone_array)
	{
		m_aBoneArray = bone_array;
	}

	int GetReadableBones()
	{
		return m_ReadableBones;
	}

	void SetReadableBones(int flags)
	{
		m_ReadableBones = flags;
	}

	int GetWritableBones()
	{
		return m_WritableBones;
	}

	void SetWritableBones(int flags)
	{
		m_WritableBones = flags;
	}

	alignas(16) matrix3x4_t* m_aBoneArray;
	int m_ReadableBones;
	int m_WritableBones;
};
