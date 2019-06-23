#pragma once

namespace COMPANY_NAME
{
	template<typename Type>
	void Idable<Type>::Init()
	{
		Meta::Cast<Type>(*this).Id() = object_id_count++;
	}

	template <typename Type1, typename Type2>
	bool operator<(const Idable<Type1>& idable1, const Idable<Type2>& idable2)
	{
		return Meta::Const_Cast<Type1>(idable1).Id() < Meta::Const_Cast<Type2>(idable2).Id();
	}

	template <typename Type1, typename Type2>
	bool operator<(const Statically_Idable<Type1>& idable1, const Statically_Idable<Type2>& idable2)
	{
		return Meta::Const_Cast<Type1>(idable1).Id() < Meta::Const_Cast<Type2>(idable2).Id();
	}
}